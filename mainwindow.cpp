#include "mainwindow.h"

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
    split->addWidget(makeServerPanel());
    decorateSplitter(split, 1);

    processQueue();
}

MainWindow::~MainWindow()
{
}

void MainWindow::decorateSplitter(QSplitter* splitter, int index)
{
    Q_ASSERT(splitter != nullptr);

    const int grips = 2;

    splitter->setOpaqueResize(false);
    splitter->setChildrenCollapsible(false);

    splitter->setHandleWidth(7);

    QSplitterHandle* handle = splitter->handle(index);
    auto orientation = splitter->orientation();

    QHBoxLayout* layout = new QHBoxLayout(handle);
    layout->setSpacing(0);
    layout->setMargin(0);

    if (orientation == Qt::Horizontal) {
        for (int i = 0; i < grips; ++i) {
            QFrame* line = new QFrame(handle);
            line->setMinimumWidth(1);
            line->setMaximumWidth(1);
            line->setFrameShape(QFrame::StyledPanel);
            layout->addWidget(line);
        }
    } else {
        layout->addStretch();
        QVBoxLayout* vbox = new QVBoxLayout();
        for (int i = 0; i < grips; ++i) {
            QFrame* line = new QFrame(handle);
            line->setMinimumHeight(1);
            line->setMaximumHeight(1);
            line->setFrameShape(QFrame::StyledPanel);
            vbox->addWidget(line);
        }
        layout->addLayout(vbox);
        layout->addStretch();
    }
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
    clientConnection_ = tcpServer->nextPendingConnection();
    connect(clientConnection_, &QAbstractSocket::disconnected,
        this, &MainWindow::onDisconnectClient);
    connect(clientConnection_, &QIODevice::readyRead, this, &MainWindow::onDataRead);

    connectToServer();
}

void MainWindow::onDisconnectClient()
{
    if (clientConnection_) {
        clientConnection_->deleteLater();
        clientConnection_ = nullptr;
    }
    isConnected_ = false;

    if (log_)
        log_->append("Клиент отключён");

    disconnectFromServer();
}

void MainWindow::onDataRead()
{
    auto data = clientConnection_->readAll();
    qDebug() << QDateTime::currentDateTimeUtc();
    toServerQueue_.append(std::make_pair(QDateTime::currentDateTimeUtc().addMSecs(DELTA_TIME), data));
    log_->append("Пришли данные");
}

void MainWindow::onServerDataRead()
{
    auto data = tcpSocket->readAll();
    toClientQueue_.append(std::make_pair(QDateTime::currentDateTimeUtc(), data));
    fromServerlog_->append("Пришли данные");
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

    if (toServerQueue_.isEmpty() == false) {
        if (hasServerConnection_) {
            while (toServerQueue_.isEmpty() == false) {
                if (toServerQueue_.head().first > currentTime)
                    break;

                if (tcpSocket->isOpen() == false)
                    break;

                auto pair = toServerQueue_.dequeue();
                tcpSocket->write(pair.second);
            }
        }
    }

    if (toClientQueue_.isEmpty() == false) {
        if (isConnected_) {
            while (toClientQueue_.isEmpty() == false) {
                if (toClientQueue_.head().first > currentTime)
                    break;

                if (clientConnection_->isOpen() == false)
                    break;

                auto pair = toClientQueue_.dequeue();
                clientConnection_->write(pair.second);
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

bool MainWindow::isClientConnected() const
{
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
        hbox->addWidget(spinOutPort_);
        spinOutPort_->setValue(asClientPort_);

        hbox->addStretch();
        btnStart_ = new QPushButton("Запустить прокси");
        hbox->addWidget(btnStart_);
        vbox->addLayout(hbox);
    }
    log_ = new QTextEdit(res);
    vbox->addWidget(log_);
    log_->setReadOnly(true);

    connect(btnStart_, &QPushButton::clicked, this, &MainWindow::onStartClicked);

    return res;
}

QWidget* MainWindow::makeServerPanel()
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

        hbox->addWidget(new QLabel("Port:"));
        spinPort_ = new QSpinBox();
        spinPort_->setMaximumWidth(70);
        spinPort_->setRange(1024, 0xffff);
        hbox->addWidget(spinPort_);
        spinPort_->setValue(serverPort_);

        hbox->addStretch(0);
        lblConnection_ = new QLabel("Не подключён");
        hbox->addWidget(lblConnection_);
        vbox->addLayout(hbox);
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
