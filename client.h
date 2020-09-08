#ifndef CLIENT_H
#define CLIENT_H

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QTcpSocket>
#include <QTextEdit>
#include <QWidget>

class Client : public QWidget {
    Q_OBJECT
public:
    Client(QWidget* parent = nullptr);
    bool isConnected() const;

public slots:
    void sendData(QByteArray data);

private slots:
    void onConnectBtnClicked();
    void onDataRead();
    void onConnected();
    void displayError(QAbstractSocket::SocketError socketError);

signals:
    void dataReceived(QByteArray);

private:
    QSpinBox* spinPort_ = nullptr;
    QLineEdit* editIp_ = nullptr;
    QTextEdit* log_ = nullptr;
    QPushButton* btnConnect_ = nullptr;

    int serverPort_ = 3030;
    QString serverIp_ = "127.0.0.1";

    QTcpSocket* tcpSocket = nullptr;
    bool isConnected_ = false;

    void initUi();
    void clientConnect();
    void clientDisconnect();
};

#endif // CLIENT_H