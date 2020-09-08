#ifndef CLIENT_H
#define CLIENT_H


#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QTcpSocket>
#include <QTextEdit>
#include <QWidget>

class Client: public QWidget
{
public:
    Client(QWidget* parent=nullptr);

private slots:
    void requestNewConnection();
    void onDataRead();
    void displayError(QAbstractSocket::SocketError socketError);

private:
    QLabel *statusLabel = nullptr;
    QSpinBox* spinPort_ = nullptr;
    QLineEdit* editIp_ = nullptr;
    QTextEdit* logEdit_ = nullptr;
    QPushButton* btnConnect_ = nullptr;

    int serverPort_ = 3030;
    QString serverIp_ = "127.0.0.1";

    QTcpSocket *tcpSocket = nullptr;

    void initUi();
};

#endif // CLIENT_H
