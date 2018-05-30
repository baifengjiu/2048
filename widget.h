#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QLabel>
#include <time.h>
#include <QKeyEvent>
#include <QMessageBox>

#include "mylabel.h"
#include "gamewidget.h"

namespace Ui {
class Widget;
}


class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

    enum GestureDirect{
        LEFT,   //左
        RIGHT,  //右
        DOWN,   //上
        UP      //下
    };

    //随机生成标签
    void createLabel();
    //开始游戏
    void startGame();
    //合并,并将合并成功与否的结果返回
    bool merge(MyLabel *temp,int row,int col);
    //是否能合并
    bool isMerge();
    //游戏结束
    void gameOver();

    void keyPressEvent(QKeyEvent *event);

public slots:
    void moveLabel(GestureDirect direction);

private:
    bool moveUp();
    bool moveDown();
    bool moveLeft();
    bool moveRight();

    bool isMergeUp(int row,int col);
    bool isMergeDown(int row,int col);
    bool isMergeLeft(int row,int col);
    bool isMergeRight(int row,int col);

signals:
    //按键时触发该信号
    void GestureMove(GestureDirect direction);

private:
    Ui::Widget *ui;
    int m_score;                    //当前得分
    int m_highScore;                //历史最高得分
    int m_labelCount;               //标签的数量
    const int m_colWidth = 75;      //列宽
    const int m_rowHeight = 75;     //行高
    const int m_xOffset = 10;       //偏移距离
    const int m_yOffset = 10;
    MyLabel *labels[4][4];          //标签
    GameWidget * m_gameWidget;      //主游戏窗口



};

#endif // WIDGET_H
