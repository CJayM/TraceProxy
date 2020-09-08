#include "server.h"

#include <QNetworkInterface>
#include <QTimer>
#include <QVBoxLayout>

Server::Server(QWidget* parent)
    : QWidget(parent)
{
    initUi();
    connect(btnStart_, &QPushButton::clicked, this, &Server::onStartClicked);
    connect(tcpServer, &QTcpServer::newConnection, this, &Server::onClientConnected);
}

void Server::onStartClicked()
{
    if (isStarted_) {
        stopServer();
    } else {
        startServer();
    }
}

void Server::onClientConnected()
{
    log_->append("Клиент подключился");
    clientConnection_ = tcpServer->nextPendingConnection();
    connect(clientConnection_, &QAbstractSocket::disconnected,
        this, &Server::onDisconnectClient);
}

void Server::onDisconnectClient()
{
    clientConnection_->deleteLater();
    clientConnection_ = nullptr;
    log_->append("Клиент отключён");
}

void Server::initUi()
{
    auto vbox = new QVBoxLayout();
    setLayout(vbox);
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
    log_ = new QTextEdit();
    vbox->addWidget(log_);
}

void Server::startServer()
{
    btnStart_->setEnabled(false);
    btnStart_->setText("Запуск...");
    isStarted_ = false;

    QTimer::singleShot(1, [&]() {
        tcpServer = new QTcpServer(this);
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
        isStarted_ = true;
        btnStart_->setText("Остановить");
    });
}

void Server::stopServer()
{
    tcpServer->close();

    btnStart_->setEnabled(true);
    btnStart_->setText("Запустить прокси");
    isStarted_ = false;

    log_->append("Сервер остановлен");
}
