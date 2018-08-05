#ifndef BGWIDGET_H
#define BGWIDGET_H

#include <QWidget>

class BGWidget : public QWidget
{
    Q_OBJECT
public:
    explicit BGWidget(QWidget *parent = 0);
    ~BGWidget();

private:
    void paintEvent(QPaintEvent *event);
};

#endif //BGWIDGET_H
