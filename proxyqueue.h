#ifndef PROXYQUEUE_H
#define PROXYQUEUE_H

#include "client.h"
#include "server.h"

#include <QDateTime>
#include <QQueue>

class ProxyQueue : public QObject {
    Q_OBJECT
public:
    ProxyQueue(QObject* parent = nullptr);

    void sendToServer(QByteArray data, QDateTime time);
    void sendToClient(QByteArray data, QDateTime time);
    void setClient(Client*);
    void setServer(Server*);

public slots:
    void onClientDataReceived(QByteArray data);
    void onServerDataReceived(QByteArray data);

private slots:
    void processQueues();

private:
    QQueue<std::pair<QDateTime, QByteArray>> toServerQueue_;
    QQueue<std::pair<QDateTime, QByteArray>> toClientQueue_;

    Client* client = nullptr;
    Server* server = nullptr;
};

#endif // PROXYQUEUE_H
