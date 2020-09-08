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
    split->addWidget(&asProxy);
    split->addWidget(&asClient);
    decorateSplitter(split, 1);
}

MainWindow::~MainWindow()
{
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
