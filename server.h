#ifndef CLIENT_H
#define CLIENT_H

#include <QDateTime>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QQueue>
#include <QSpinBox>
#include <QTcpSocket>
#include <QTextEdit>
#include <QWidget>

class Server : public QWidget {
    Q_OBJECT
public:
    Server(QWidget* parent = nullptr);
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
    QLabel* lblConnection_ = nullptr;

    int serverPort_ = 3030;
    QString serverIp_ = "127.0.0.1";

    QTcpSocket* tcpSocket = nullptr;
    bool isConnected_ = false;

    QQueue<std::pair<QDateTime, QByteArray>> toClientQueue_;

    void initUi();
    void clientConnect();
    void clientDisconnect();
};

#endif // CLIENT_H
