#include "mainwindow.h"

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QSplitter>
#include <QTextEdit>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    auto wdgt = new QWidget();
    auto root = new QVBoxLayout();
    wdgt->setLayout(root);
    setCentralWidget(wdgt);

    QSplitter* split = new QSplitter();

    root->addWidget(split);
    split->addWidget(makeLeftPanel());
    split->addWidget(makeRightPanel());
    decorateSplitter(split, 1);
}

MainWindow::~MainWindow()
{
}

QWidget* MainWindow::makeLeftPanel()
{
    QWidget* res = new QWidget();
    auto vbox = new QVBoxLayout();
    res->setLayout(vbox);
    {
        auto hbox = new QHBoxLayout();
        hbox->addWidget(new QLabel("Input Port:"));
        spinInPort_ = new QSpinBox();
        spinInPort_->setRange(1024, 0xffff);
        hbox->addWidget(spinInPort_);
        spinInPort_->setValue(asServerPort_);

        hbox->addStretch();
        hbox->addWidget(new QPushButton("Start server"));
        vbox->addLayout(hbox);
    }
    vbox->addWidget(new QTextEdit());
    return res;
}

QWidget* MainWindow::makeRightPanel()
{
    QWidget* res = new QWidget();
    auto vbox = new QVBoxLayout();
    res->setLayout(vbox);
    {
        auto hbox = new QHBoxLayout();
        hbox->addWidget(new QLabel("Output Ip:"));
        editOutIp_ = new QLineEdit(asClientIp_);
        hbox->addWidget(editOutIp_);

        hbox->addWidget(new QLabel("Port:"));
        spinOutPort_ = new QSpinBox();
        spinOutPort_->setRange(1024, 0xffff);
        hbox->addWidget(spinOutPort_);
        spinOutPort_->setValue(asClientPort_);

        hbox->addStretch();
        hbox->addWidget(new QPushButton("Connect"));
        vbox->addLayout(hbox);
    }
    vbox->addWidget(new QTextEdit());
    return res;
}

void MainWindow::decorateSplitter(QSplitter* splitter, int index)
{
    Q_ASSERT(splitter != nullptr);

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
    } else {
        layout->addStretch();
        QVBoxLayout* vbox = new QVBoxLayout();
        for (int i = 0; i < grips; ++i) {
            QFrame* line = new QFrame(handle);
            line->setMinimumHeight(1);
            line->setMaximumHeight(1);
            line->setFrameShape(QFrame::StyledPanel);
            vbox->addWidget(line);
        }
        layout->addLayout(vbox);
        layout->addStretch();
    }
}
