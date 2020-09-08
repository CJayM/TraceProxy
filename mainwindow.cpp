#include "mainwindow.h"

#include <QPushButton>
#include <QSplitter>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    auto wdgt = new QWidget();
    auto root = new QVBoxLayout();
    wdgt->setLayout(root);
    setCentralWidget(wdgt);

    root->addWidget(new QPushButton("Btn1"));
    root->addWidget(new QPushButton("Btn2"));
    root->addWidget(new QPushButton("Btn3"));
    root->addWidget(new QPushButton("Btn4"));
    root->addWidget(new QPushButton("Btn5"));

    QSplitter* split = new QSplitter();

    root->addWidget(split);
    split->addWidget(makeLeftPanel());
    split->addWidget(makeCentralPanel());
    split->addWidget(makeRightPanel());
    decorateSplitter(split, 0);
    decorateSplitter(split, 1);
    decorateSplitter(split, 2);
}

MainWindow::~MainWindow()
{
}

QWidget* MainWindow::makeLeftPanel()
{
    QWidget* res = new QWidget();
    auto vbox = new QVBoxLayout();
    res->setLayout(vbox);
    vbox->addWidget(new QPushButton("Left panel"));
    return res;
}

QWidget *MainWindow::makeCentralPanel()
{
    QWidget* res = new QWidget();
    auto vbox = new QVBoxLayout();
    res->setLayout(vbox);
    vbox->addWidget(new QPushButton("Central panel"));
    return res;
}

QWidget* MainWindow::makeRightPanel()
{
    QWidget* res = new QWidget();
    auto vbox = new QVBoxLayout();
    res->setLayout(vbox);
    vbox->addWidget(new QPushButton("Right panel"));
    return res;
}

void MainWindow::decorateSplitter(QSplitter* splitter, int index)
{
    Q_ASSERT(splitter != nullptr);

    const int gripLength = 18;
    const int gripWidth = 1;
    const int grips = 2;

    splitter->setOpaqueResize(false);
    splitter->setChildrenCollapsible(false);

    splitter->setHandleWidth(7);

    QSplitterHandle* handle = splitter->handle(index);
    auto orientation = splitter->orientation();

    QHBoxLayout* layout = new QHBoxLayout(handle);
    layout->setSpacing(0);
    layout->setMargin(0);

    if (orientation == Qt::Horizontal) {
        for (int i = 0; i < grips; ++i) {
            QFrame* line = new QFrame(handle);
            line->setMinimumWidth(1);
            line->setMaximumWidth(1);
            line->setFrameShape(QFrame::StyledPanel);
            layout->addWidget(line);
        }
    }else{
        layout->addStretch();
        QVBoxLayout* vbox = new QVBoxLayout();
        for(int i =0; i<grips; ++i){
            QFrame* line = new QFrame(handle);
            line->setMinimumHeight(1);
            line->setMaximumHeight(1);
//            line->setMinimumSize(gripWidth, gripLength);
//            line->setMaximumSize(gripWidth, gripLength);
            line->setFrameShape(QFrame::StyledPanel);
            vbox->addWidget(line);
        }
        layout->addLayout(vbox);
        layout->addStretch();
    }
}
