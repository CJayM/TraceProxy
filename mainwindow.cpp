#include "mainwindow.h"

#include "utils.h"
#include <QAbstractSocket>
#include <QLabel>
#include <QLineEdit>
#include <QNetworkInterface>
#include <QPushButton>
#include <QSpinBox>
#include <QSplitter>
#include <QTextEdit>
#include <QTimer>
#include <QVBoxLayout>

const int DELTA_TIME = 5000;

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    auto wdgt = new QWidget();
    auto root = new QVBoxLayout();
    wdgt->setLayout(root);
    setCentralWidget(wdgt);

    QSplitter* split = new QSplitter();

    makeLeftPanel();

    root->addWidget(split);
    split->addWidget(makeLeftPanel());
    split->addWidget(makeRightPanel());
    decorateSplitter(split, 1);

    processQueue();
}

void MainWindow::onStartClicked()
{
    if (isProxyStarted_) {
        stopProxy();
    } else {
        startProxy();
    }
}

void MainWindow::onClientConnected()
{
    log_->append("Клиент подключился");
    isConnected_ = true;
    toServerSocket_ = tcpServer->nextPendingConnection();
    connect(toServerSocket_, &QAbstractSocket::disconnected,
        this, &MainWindow::onDisconnectClient);
    connect(toServerSocket_, &QIODevice::readyRead, this, &MainWindow::onDataRead);

    connectToServer();
}

void MainWindow::onDisconnectClient()
{
    if (toServerSocket_) {
        toServerSocket_->deleteLater();
        toServerSocket_ = nullptr;
    }
    isConnected_ = false;

    if (log_)
        log_->append("Клиент отключён");

    disconnectFromServer();
}

void MainWindow::onDataRead()
{
    auto data = toServerSocket_->readAll();
    toServerQueue_.append(std::make_pair(QDateTime::currentDateTimeUtc(), data));
    setRightQueuSizeLabel(toServerQueue_.length());
    log_->append(QString("Пришли данные %1").arg(data.length()));
    log_->append(data.toHex());
}

void MainWindow::onServerDataRead()
{
    auto data = tcpSocket->readAll();
    toClientQueue_.append(std::make_pair(QDateTime::currentDateTimeUtc(), data));
    setLeftQueuSizeLabel(toClientQueue_.length());
    fromServerlog_->append(QString("Пришли данные %1").arg(data.length()));
    fromServerlog_->append(data.toHex());
}

void MainWindow::onConnectedToServer()
{
    hasServerConnection_ = true;
    fromServerlog_->append("Создано подключение");
    fromServerlog_->setEnabled(true);
    lblConnection_->setText("Подключено");
}

void MainWindow::displayError(QAbstractSocket::SocketError socketError)
{
    qWarning() << "Not implemented MainWindow::displayError";
}

void MainWindow::processQueue()
{
    auto currentTime = QDateTime::currentDateTimeUtc();
    auto past = currentTime.addMSecs(-sendDelay_);

    if (toServerQueue_.isEmpty() == false) {
        if (hasServerConnection_) {
            while (toServerQueue_.isEmpty() == false) {                
                if (toServerQueue_.head().first > past)
                    break;

                if (tcpSocket->isOpen() == false)
                    break;

                auto pair = toServerQueue_.dequeue();
                setRightQueuSizeLabel(toServerQueue_.length());
                tcpSocket->write(pair.second);
            }
        }
    }

    if (toClientQueue_.isEmpty() == false) {
        if (isConnected_) {
            while (toClientQueue_.isEmpty() == false) {
                if (toClientQueue_.head().first > past)
                    break;

                if (toServerSocket_->isOpen() == false)
                    break;

                auto pair = toClientQueue_.dequeue();
                setLeftQueuSizeLabel(toClientQueue_.length());
                toServerSocket_->write(pair.second);
            }
        }
    }

    QTimer::singleShot(DELTA_TIME, this, &MainWindow::processQueue);
}

void MainWindow::startProxy()
{
    btnStart_->setEnabled(false);
    btnStart_->setText("Запуск...");
    isProxyStarted_ = false;

    QTimer::singleShot(1, [&]() {
        tcpServer = new QTcpServer(this);
        connect(tcpServer, &QTcpServer::newConnection, this, &MainWindow::onClientConnected);

        if (!tcpServer->listen(QHostAddress::Any, asClientPort_)) {
            log_->append(QString("Не удалось запустить сервер: %1.")
                             .arg(tcpServer->errorString()));
            btnStart_->setEnabled(true);
            return;
        }

        QString ipAddress;
        QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
        for (int i = 0; i < ipAddressesList.size(); ++i) {
            if (ipAddressesList.at(i) != QHostAddress::LocalHost && ipAddressesList.at(i).toIPv4Address()) {
                ipAddress = ipAddressesList.at(i).toString();
                break;
            }
        }
        // if we did not find one, use IPv4 localhost
        if (ipAddress.isEmpty())
            ipAddress = QHostAddress(QHostAddress::LocalHost).toString();
        log_->append(QString("Сервер запущен по адресу: %1:%2")
                         .arg(ipAddress)
                         .arg(tcpServer->serverPort()));
        btnStart_->setEnabled(true);
        isProxyStarted_ = true;
        btnStart_->setText("Остановить");
    });
}

void MainWindow::stopProxy()
{
    tcpServer->close();

    btnStart_->setEnabled(true);
    btnStart_->setText("Запустить прокси");
    isProxyStarted_ = false;

    log_->append("Сервер остановлен");
}

void MainWindow::setLeftQueuSizeLabel(int size)
{
    leftQueuSizeLbl_->setText(QString("В очереди пачек: %1").arg(size));
}

void MainWindow::setRightQueuSizeLabel(int size)
{
    rightQueuSizeLbl_->setText(QString("В очереди пачек: %1 ").arg(size));
}

QWidget* MainWindow::makeLeftPanel()
{
    QWidget* res = new QWidget();
    auto vbox = new QVBoxLayout();
    res->setLayout(vbox);
    {
        auto hbox = new QHBoxLayout();
        hbox->addWidget(new QLabel("Port:"));
        spinOutPort_ = new QSpinBox();
        spinOutPort_->setRange(1024, 0xffff);
        spinOutPort_->setValue(asClientPort_);
        spinOutPort_->setMinimumWidth(70);
        spinOutPort_->setMaximumWidth(70);

        hbox->addWidget(spinOutPort_);
        connect(spinOutPort_, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [&](int val) {
            asClientPort_ = val;
        });

        btnStart_ = new QPushButton("Запустить прокси");
        hbox->addWidget(btnStart_);

        hbox->addStretch();

        hbox->addWidget(new QLabel("Задержка:"));
        auto delaySpin = new QSpinBox();
        hbox->addWidget(delaySpin);
        delaySpin->setMinimumWidth(70);
        delaySpin->setMaximumWidth(70);
        delaySpin->setRange(0, 30000);
        delaySpin->setValue(sendDelay_);

        connect(delaySpin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [&](int val) {
            sendDelay_ = val;
        });

        vbox->addLayout(hbox);
    }

    {
        auto hbox = new QHBoxLayout();
        vbox->addLayout(hbox);
        leftQueuSizeLbl_ = new QLabel();
        hbox->addWidget(leftQueuSizeLbl_);
    }

    log_ = new QTextEdit(res);
    vbox->addWidget(log_);
    log_->setReadOnly(true);

    connect(btnStart_, &QPushButton::clicked, this, &MainWindow::onStartClicked);

    return res;
}

QWidget* MainWindow::makeRightPanel()
{
    QWidget* res = new QWidget();

    auto vbox = new QVBoxLayout();
    res->setLayout(vbox);

    {
        auto hbox = new QHBoxLayout();
        hbox->addWidget(new QLabel("Server IP:"));
        editIp_ = new QLineEdit();
        editIp_->setMaximumWidth(120);
        editIp_->setText(serverIp_);        
        hbox->addWidget(editIp_);

        connect(editIp_, &QLineEdit::textChanged, [&](QString val) {
            serverIp_ = val;
        });

        hbox->addWidget(new QLabel("Port:"));
        spinPort_ = new QSpinBox();
        spinPort_->setMaximumWidth(70);
        spinPort_->setRange(1024, 0xffff);
        spinPort_->setValue(serverPort_);
        hbox->addWidget(spinPort_);

        connect(spinPort_, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [&](int val) {
            serverPort_ = val;
        });


        hbox->addStretch(0);
        lblConnection_ = new QLabel("Не подключён");
        hbox->addWidget(lblConnection_);
        vbox->addLayout(hbox);
    }

    {
        auto hbox = new QHBoxLayout();
        vbox->addLayout(hbox);
        rightQueuSizeLbl_ = new QLabel();
        hbox->addWidget(rightQueuSizeLbl_);
    }


    fromServerlog_ = new QTextEdit();
    fromServerlog_->setReadOnly(true);
    fromServerlog_->setEnabled(false);
    vbox->addWidget(fromServerlog_);

    return res;
}

void MainWindow::connectToServer()
{
    tcpSocket = new QTcpSocket(this);
    connect(tcpSocket, &QIODevice::readyRead, this, &MainWindow::onServerDataRead);
    connect(tcpSocket, &QTcpSocket::connected, this, &MainWindow::onConnectedToServer);
    connect(tcpSocket, static_cast<void (QAbstractSocket::*)(QAbstractSocket::SocketError)>(&QAbstractSocket::error),
        this, &MainWindow::displayError);

    tcpSocket->connectToHost(serverIp_, serverPort_);
}

void MainWindow::disconnectFromServer()
{
    if (tcpSocket) {
        tcpSocket->close();
        tcpSocket->deleteLater();
        tcpSocket = nullptr;
    }

    fromServerlog_->append("Отключён");
    fromServerlog_->setEnabled(false);
    lblConnection_->setText("Отключён");
}
