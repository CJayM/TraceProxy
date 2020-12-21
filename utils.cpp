#include "utils.h"

#include <QVBoxLayout>

void decorateSplitter(QSplitter* splitter, int index)
{
    Q_ASSERT(splitter != nullptr);

    splitter->setOpaqueResize(true);
    splitter->setChildrenCollapsible(false);

    splitter->setHandleWidth(7);

    QSplitterHandle* handle = splitter->handle(index);
    auto orientation = splitter->orientation();

    if (orientation == Qt::Horizontal) {
        auto layout = new QHBoxLayout(handle);
        layout->setSpacing(0);
        layout->setMargin(0);

        QFrame* line = new QFrame(handle);
        line->setMinimumWidth(1);
        line->setMaximumWidth(1);
        line->setFrameShape(QFrame::StyledPanel);
        layout->addWidget(line);
    } else {
        auto layout = new QVBoxLayout(handle);
        layout->setSpacing(0);
        layout->setMargin(0);

        layout->addStretch();
        QVBoxLayout* vbox = new QVBoxLayout();

        QFrame* line = new QFrame(handle);
        line->setMinimumHeight(1);
        line->setMaximumHeight(1);
        line->setFrameShape(QFrame::StyledPanel);
        vbox->addWidget(line);

        layout->addLayout(vbox);
        layout->addStretch();
    }
}
