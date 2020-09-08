#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "client.h"
#include "proxy.h"

#include <QMainWindow>
#include <QSpinBox>
#include <QSplitter>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

    void decorateSplitter(QSplitter* splitter, int index);

private:
    Client asClient;
    Proxy asProxy;
};
#endif // MAINWINDOW_H
