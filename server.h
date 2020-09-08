#ifndef SERVER_H
#define SERVER_H

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTextEdit>
#include <QWidget>

class Server : public QWidget {
    Q_OBJECT
public:
    Server(QWidget* parent = nullptr);

private slots:
    void onStartClicked();
    void onClientConnected();
    void onDisconnectClient();

private:
    int asClientPort_ = 3031;
    QLineEdit* editOutIp_ = nullptr;
    QSpinBox* spinOutPort_ = nullptr;
    QPushButton* btnStart_ = nullptr;
    QTcpServer *tcpServer = nullptr;
    QTextEdit* log_ = nullptr;

    QTcpSocket *clientConnection_ = nullptr;

    void initUi();
};

#endif // SERVER_H
