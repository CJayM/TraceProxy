#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSplitter>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QWidget* makeLeftPanel();
    QWidget* makeCentralPanel();
    QWidget* makeRightPanel();

    void decorateSplitter(QSplitter* splitter, int index);
};
#endif // MAINWINDOW_H
