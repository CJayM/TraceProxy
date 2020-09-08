#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSpinBox>
#include <QSplitter>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QWidget* makeLeftPanel();
    QWidget* makeRightPanel();

    void decorateSplitter(QSplitter* splitter, int index);


private:
    int asServerPort_ = 3031;
    int asClientPort_ = 3030;
    QString asClientIp_ = "127.0.0.1";

    QSpinBox* spinInPort_ = nullptr;
    QLineEdit* editOutIp_ = nullptr;
    QSpinBox* spinOutPort_ = nullptr;
};
#endif // MAINWINDOW_H
