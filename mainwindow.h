#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QAbstractSocket>
#include <QByteArray>
#include <QDateTime>
#include <QLabel>
#include <QMainWindow>
#include <QPushButton>
#include <QQueue>
#include <QSpinBox>
#include <QSplitter>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTextEdit>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

    void decorateSplitter(QSplitter* splitter, int index);

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
    int sendDelay_ = 100;

    // proxy
    int asClientPort_ = 3031;
    QLineEdit* editOutIp_ = nullptr;
    QSpinBox* spinOutPort_ = nullptr;
    QPushButton* btnStart_ = nullptr;
    QTcpServer* tcpServer = nullptr;
    QTcpSocket* toServerSocket_ = nullptr;
    QTextEdit* log_ = nullptr;
    bool isProxyStarted_ = false;
    bool isConnected_ = false;

    void initUi();
    void startProxy();
    void stopProxy();
    QWidget* makeLeftPanel();
    // end proxy

    QQueue<std::pair<QDateTime, QByteArray>> toClientQueue_;
    QQueue<std::pair<QDateTime, QByteArray>> toServerQueue_;

    // client to server
    QSpinBox* spinPort_ = nullptr;
    QLineEdit* editIp_ = nullptr;
    QTextEdit* fromServerlog_ = nullptr;
    QLabel* lblConnection_ = nullptr;

    int serverPort_ = 3030;
    QString serverIp_ = "127.0.0.1";

    QTcpSocket* tcpSocket = nullptr;
    bool hasServerConnection_ = false;

    QWidget* makeRightPanel();
    void clientConnect();
    void clientDisconnect();
    void connectToServer();
    void disconnectFromServer();
    // end client to server
};
#endif // MAINWINDOW_H
