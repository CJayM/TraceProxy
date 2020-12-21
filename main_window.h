#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QWidget>
#include <QByteArray>
#include <QQueue>
#include <QDateTime>
#include <QTcpServer>
#include <QTcpSocket>
#include "QHexView.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private slots:
    void onStartClicked();
    void onClientConnected();
    void onDisconnectClient();
    void onDataRead();

    // to server
    void onServerDataRead();
    void onConnectedToServer();
    void displayError(QAbstractSocket::SocketError socketError);

    void processQueue();

private:
    // proxy
    int asClientPort_ = 3030;
//    QLineEdit* editOutIp_ = nullptr;
    QTcpServer* tcpServer = nullptr;
    QTcpSocket* toServerSocket_ = nullptr;
//    QTextEdit* log_ = nullptr;
    bool isProxyStarted_ = false;
    bool isConnected_ = false;

    QHexView* hexView_ = nullptr;

    void initUi();
    void startProxy();
    void stopProxy();

    // end proxy

    QQueue<std::pair<QDateTime, QByteArray>> toClientQueue_;
    QQueue<std::pair<QDateTime, QByteArray>> toServerQueue_;

    // client to server
//    QTextEdit* fromServerlog_ = nullptr;

    int serverPort_ = 3031;
    QString serverIp_ = "127.0.0.1";

    QByteArray hexData_;

    QTcpSocket* tcpSocket = nullptr;
    bool hasServerConnection_ = false;

    void clientConnect();
    void clientDisconnect();
    void connectToServer();
    void disconnectFromServer();
    // end client to server

private:
    Ui::MainWindow *ui;
};

#endif // MAIN_WINDOW_H
