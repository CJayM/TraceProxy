#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "server.h"

#include <QMainWindow>
#include <QSpinBox>
#include <QSplitter>
#include <QTcpServer>

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

private:
    Server asClient;

    // proxy
    int asClientPort_ = 3031;
    QLineEdit* editOutIp_ = nullptr;
    QSpinBox* spinOutPort_ = nullptr;
    QPushButton* btnStart_ = nullptr;
    QTcpServer* tcpServer = nullptr;
    QTextEdit* log_ = nullptr;
    bool isStarted_ = false;
    bool isConnected_ = false;

    QTcpSocket* clientConnection_ = nullptr;
    QQueue<std::pair<QDateTime, QByteArray>> toServerQueue_;

    void initUi();
    void startServer();
    void stopServer();
    bool isConnected() const;
    QWidget* makeLeftPanel();
    // end proxy
};
#endif // MAINWINDOW_H
