#ifndef GAMEWIDGET_H
#define GAMEWIDGET_H

#include <QWidget>
#include <QLabel>
#include <time.h>
#include <QKeyEvent>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QCloseEvent>
#include <QPropertyAnimation>
#include <QFileDialog>

#include "mylabel.h"
#include "bgwidget.h"

namespace Ui {
class Widget;
}


class GameWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GameWidget(QWidget *parent = 0);
    ~GameWidget();

    enum GestureDirect{
        LEFT,   //左
        RIGHT,  //右
        DOWN,   //上
        UP      //下
    };

    //随机生成标签
    void createLabel();
    //初始化游戏
    void initGame();
    //合并,并将合并成功与否的结果返回
    bool merge(MyLabel *temp,int row,int col);
    //是否能合并
    bool isMerge();
    //游戏结束
    bool gameOver();
    //保存当前游戏进度
    void saveGame();
    //释放资源
    void releaseRes();

    void closeEvent(QCloseEvent *event);

    void keyPressEvent(QKeyEvent *event);

public slots:
    //设置分数
    void setScore();
    //重新开始
    //void reStartGame();
    //移动
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
    //改变分数
    void ScoreChange();

private:
    Ui::Widget *ui;
    int m_score = 0;                    //当前得分
    int m_highScore = 0;                //历史最高得分
    int m_labelCount = 0;           //标签的数量
    const int m_colWidth = 75;      //列宽
    const int m_rowHeight = 75;     //行高
    const int m_xOffset = 10;       //偏移距离
    const int m_yOffset = 10;
    MyLabel *labels[4][4];          //标签
    BGWidget * m_bgWidget;      //主游戏窗口
};

#endif // GAMEWIDGET_H
