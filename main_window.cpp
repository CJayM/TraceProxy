#include "main_window.h"
#include "ui_main_window.h"

#include <QAbstractSocket>
#include <QNetworkInterface>
#include <QTimer>

#include "utils.h"

const int DELTA_TIME = 5000;

MainWindow::MainWindow(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->proxyPortSpinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [&](int val) {
        asClientPort_ = val;
    });

    connect(ui->btnStart, &QPushButton::clicked, this, &MainWindow::onStartClicked);

    connect(ui->serverIpEdit, &QLineEdit::textChanged, [&](QString val) {
        serverIp_ = val;
    });

    connect(ui->serverPortSpinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [&](int val) {
        serverPort_ = val;
    });

    decorateSplitter(ui->horSplitter, 1);
    decorateSplitter(ui->vertSplitter, 1);

    processQueue();
}

MainWindow::~MainWindow()
{
    delete ui;
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
    ui->logEdit->append("Клиент подключился");
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

    ui->logEdit->append("Клиент отключён");

    disconnectFromServer();
}

void MainWindow::onDataRead()
{
    auto data = toServerSocket_->readAll();
    toServerQueue_.append(std::make_pair(QDateTime::currentDateTimeUtc(), data));
    ui->logEdit->append(QString("Пришли данные %1").arg(data.length()));
    ui->logEdit->append(data.toHex());
}

void MainWindow::onServerDataRead()
{
    auto data = tcpSocket->readAll();
    toClientQueue_.append(std::make_pair(QDateTime::currentDateTimeUtc(), data));
    ui->logEdit->append(QString("Пришли данные %1").arg(data.length()));
    ui->logEdit->append(data.toHex());
}

void MainWindow::onConnectedToServer()
{
    hasServerConnection_ = true;
    ui->logEdit->append("Создано подключение");
    //    ui->logEdit->setEnabled(true);
    ui->logEdit->setText("Подключено");
}

void MainWindow::displayError(QAbstractSocket::SocketError socketError)
{
    qWarning() << "Not implemented MainWindow::displayError";
}

void MainWindow::processQueue()
{
    if (toServerQueue_.isEmpty() == false) {
        if (hasServerConnection_) {
            while (toServerQueue_.isEmpty() == false) {
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
                if (toServerSocket_->isOpen() == false)
                    break;

                auto pair = toClientQueue_.dequeue();
                toServerSocket_->write(pair.second);
            }
        }
    }

    QTimer::singleShot(DELTA_TIME, this, &MainWindow::processQueue);
}

void MainWindow::startProxy()
{
    ui->btnStart->setEnabled(false);
    ui->btnStart->setText("Запуск...");
    isProxyStarted_ = false;

    QTimer::singleShot(1, [&]() {
        tcpServer = new QTcpServer(this);
        connect(tcpServer, &QTcpServer::newConnection, this, &MainWindow::onClientConnected);

        if (!tcpServer->listen(QHostAddress::Any, asClientPort_)) {
            ui->logEdit->append(QString("Не удалось запустить сервер: %1.")
                                    .arg(tcpServer->errorString()));
            ui->btnStart->setEnabled(true);
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
        ui->logEdit->append(QString("Сервер запущен по адресу: %1:%2")
                                .arg(ipAddress)
                                .arg(tcpServer->serverPort()));
        ui->btnStart->setEnabled(true);
        isProxyStarted_ = true;
        ui->btnStart->setText("Остановить");
    });
}

void MainWindow::stopProxy()
{
    tcpServer->close();

    ui->btnStart->setEnabled(true);
    ui->btnStart->setText("Запустить прокси");
    isProxyStarted_ = false;

    ui->logEdit->append("Сервер остановлен");
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

    ui->logEdit->append("Отключён");
    ui->connectLabel->setText("Отключён");
}
