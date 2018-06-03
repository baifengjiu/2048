#include "bgwidget.h"
#include <QDebug>

BGWidget::BGWidget(QWidget *parent): QWidget(parent)
{
    this->setStyleSheet("background-color:rgb(187, 173, 160);border-radius:5px;");
}

BGWidget::~BGWidget()
{

}

void BGWidget::paintEvent(QPaintEvent *event)
{
    QStylePainter painter(this);
    //用style画背景 (会使用setstylesheet中的内容)
    QStyleOption opt;
    opt.initFrom(this);
    opt.rect=rect();
    painter.drawPrimitive(QStyle::PE_Widget, opt);

    painter.setPen(QColor(204,192,180));
    painter.setBrush(QColor(204,192,180));

    //4*4的背景矩阵
    const int colWidth = 75;
    const int rowHeight = 75;
    const int xOffset = 10;
    const int yOffset = 10;

    for(int row = 0; row < 4;++row)
    {
        for(int col = 0; col < 4; ++col)
        {
            //背景方框
            int x = col * colWidth + xOffset;
            int y = row * rowHeight +yOffset;
            painter.drawRoundRect(x,y,65,65,10,10);
        }
    }

    QWidget::paintEvent(event);
}
