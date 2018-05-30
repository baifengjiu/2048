#ifndef GAMEWIDGET_H
#define GAMEWIDGET_H

#include <QWidget>
#include <QStylePainter>
#include <QStyleOption>


class GameWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GameWidget(QWidget *parent = 0);
    ~GameWidget();


private:
    void paintEvent(QPaintEvent *event);
};

#endif //GAMEWIDGET_H
