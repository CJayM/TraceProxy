#include "client.h"

#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QTextEdit>
#include <QTimer>
#include <QVBoxLayout>

Client::Client(QWidget* parent)
    : QWidget(parent)
{
    initUi();
    tcpSocket = new QTcpSocket(this);
    connect(tcpSocket, &QIODevice::readyRead, this, &Client::onDataRead);
    connect(tcpSocket, &QTcpSocket::connected, this, &Client::onConnected);
    connect(tcpSocket, static_cast<void (QAbstractSocket::*)(QAbstractSocket::SocketError)>(&QAbstractSocket::error),
        this, &Client::displayError);
    connect(btnConnect_, &QAbstractButton::clicked,
        this, &Client::onConnectBtnClicked);
}

bool Client::isConnected() const
{
    return isConnected_;
}

void Client::sendData(QByteArray data)
{
    if (isConnected_ == false)
        return;

    if (tcpSocket->isOpen() == false)
        return;

    tcpSocket->write(data);
    log_->append(data.toHex());
}

void Client::initUi()
{
    auto vbox = new QVBoxLayout();
    setLayout(vbox);
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
        btnConnect_ = new QPushButton("Подключить");
        hbox->addWidget(btnConnect_);
        vbox->addLayout(hbox);
    }
    log_ = new QTextEdit();
    vbox->addWidget(log_);
}

void Client::clientConnect()
{
    btnConnect_->setText("Подключение...");
    btnConnect_->setEnabled(false);
    tcpSocket->abort();
    tcpSocket->connectToHost(serverIp_, serverPort_);
}

void Client::clientDisconnect()
{
    btnConnect_->setText("Отключение...");
    btnConnect_->setEnabled(false);

    tcpSocket->abort();
    log_->append("Отключено");
    isConnected_ = false;

    btnConnect_->setText("Подключить");
    btnConnect_->setEnabled(true);
}

void Client::onConnectBtnClicked()
{
    if (isConnected_)
        clientDisconnect();
    else
        clientConnect();
}

void Client::onDataRead()
{
    auto data = tcpSocket->readAll();
    emit dataReceived(data);
    log_->append("Пришли данные");
}

void Client::onConnected()
{
    isConnected_ = true;
    btnConnect_->setText("Отключить");
    btnConnect_->setEnabled(true);
    log_->append("Подключено");
}

void Client::displayError(QAbstractSocket::SocketError socketError)
{
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        break;
    case QAbstractSocket::HostNotFoundError:
        log_->append("Сервер не найден. Проверьте правильность указанных IP и порта.");
        break;
    case QAbstractSocket::ConnectionRefusedError:
        log_->append("Подключение отклонено.");
        break;
    default:
        log_->append(QString("Произошла следующая ошибка: %1.")
                             .arg(tcpSocket->errorString()));
    }

    btnConnect_->setEnabled(true);
}
