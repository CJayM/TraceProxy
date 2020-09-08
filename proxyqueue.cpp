#include "proxyqueue.h"

#include <QTimer>

const int DELTA_MSECS = 10;

ProxyQueue::ProxyQueue(QObject* parent)
    : QObject(parent)
{
    processQueues();
}

void ProxyQueue::sendToClient(QByteArray data, QDateTime time)
{
    toClientQueue_.append(std::make_pair(time, data));
}

void ProxyQueue::setClient(Client* clnt)
{
    client = clnt;
    connect(client, &Client::dataReceived, this, &ProxyQueue::onClientDataReceived);
}

void ProxyQueue::setServer(Server* srv)
{
    server = srv;
    connect(server, &Server::dataReceived, this, &ProxyQueue::onServerDataReceived);
}

void ProxyQueue::onClientDataReceived(QByteArray data)
{
    sendToServer(data, QDateTime::currentDateTimeUtc());
}

void ProxyQueue::onServerDataReceived(QByteArray data)
{
    sendToClient(data, QDateTime::currentDateTimeUtc());
}

void ProxyQueue::processQueues()
{
    auto currentTime = QDateTime::currentDateTimeUtc();
    auto pastTime = currentTime.addMSecs(-DELTA_MSECS);
    if (toClientQueue_.isEmpty() == false) {
        if (client) {
            if (client->isConnected()) {
                while ((toClientQueue_.isEmpty() == false) && (toClientQueue_.head().first > pastTime)) {
                    auto item = toClientQueue_.dequeue();
                    client->sendData(item.second);
                }
            }
        }
    }

    if (toServerQueue_.isEmpty() == false) {
        if (server) {
            if (server->isConnected()) {
                while ((toServerQueue_.isEmpty() == false) && (toServerQueue_.head().first > pastTime)) {
                    auto item = toServerQueue_.dequeue();
                    server->sendData(item.second);
                }
            }
        }
    }

    QTimer::singleShot(DELTA_MSECS, this, &ProxyQueue::processQueues);
}

void ProxyQueue::sendToServer(QByteArray data, QDateTime time)
{
    toServerQueue_.append(std::make_pair(time, data));
}
