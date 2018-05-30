#include "widget.h"
#include "ui_widget.h"

#include <QPropertyAnimation>

#include <QDebug>

//生成的值
int CREATE_NUMBER[2] = {
    2,
    4
};

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    this->setWindowTitle("2048");

    //设置随机种子
    srand((size_t)time(NULL));

    //初始化主游戏场景
    m_gameWidget = new GameWidget(this);
    m_gameWidget->setGeometry(19,200,310,310);

    //初始化游戏
    startGame();

    //移动
    connect(this,&Widget::GestureMove,this,&Widget::moveLabel);
}

Widget::~Widget()
{
    delete ui;

    for(int col = 0;col <4;++col)
    {
        for(int row = 0;row < 4;++row)
        {
            if(labels[row][col] != NULL)
            {
                delete labels[row][col];
                labels[row][col] = NULL;
            }
        }
    }

    delete m_gameWidget;
}

void Widget::createLabel()
{
    while(true){
        int row = rand() % 4;
        int col = rand() % 4;

        int index = rand() % 2;

        //去重
        if(labels[row][col] != NULL){
            continue;
        }
        //背景方框
        MyLabel *label=new MyLabel(CREATE_NUMBER[index]);
        int x = col * m_colWidth + m_xOffset;
        int y = row * m_rowHeight +m_yOffset;
        label->setGeometry(x,y,65,65);
        label->setParent(m_gameWidget);

        labels[row][col] = label;
        labels[row][col]->show();

        break;
    }

    ++m_labelCount;
}

void Widget::startGame()
{
    //初始化分数为0
    m_score = 0;
    m_highScore = 0;

    //最高分设置，读取文件


    //初始化
    for(int row = 0;row < 4;++row)
    {
        for(int col = 0;col <4;++col)
        {
             labels[row][col] = NULL;
        }
    }

    //初始化两个标签
    for(int i=0;i<2;i++){
       createLabel();
    }
}

bool Widget::merge(MyLabel *temp, int row, int col)
{
    if(temp != NULL)
    {
        //判断两个值是否相等，是合并
        if(temp->text() == labels[row][col]->text())
        {
            int x = labels[row][col]->x();
            int y = row * m_rowHeight + m_yOffset;//y轴偏移

            //动画让label往上升
            QPropertyAnimation *animation = new QPropertyAnimation(temp,"geometry");
            //开始值和结束值
            animation->setStartValue(temp->geometry());
            animation->setEndValue(QRect(x,y,temp->width(),temp->height()));
            animation->setDuration(200);//动画播放时长
            //设置动画的运动曲线
            animation->setEasingCurve(QEasingCurve::Linear);
            animation->start(QAbstractAnimation::DeleteWhenStopped);

            labels[row][col]->reSetText(2*temp->text().toInt());

            --m_labelCount;

            delete temp;
            temp = NULL;
            return true;
        }
    }
    return false;
}

bool Widget::isMerge()
{
    for(int row = 0;row < 4;++row)
    {
        for(int col = 0;col <4;++col)
        {
              if(isMergeUp(row,col) || isMergeDown(row,col) || isMergeLeft(row,col) || isMergeRight(row,col))
              {
                return true;
              }
        }
    }

    return false;
}

void Widget::gameOver()
{
    //如果格子全满（m_labelCount == 16）
    if(m_labelCount == 16 && isMerge())
    {
        //广度，没有可以合并的标签，显示失败
        //QMessageBox("失败",);
        QMessageBox::information(0, "信息", "失败!");
        qDebug()<<"失败";
    }
    else
    {
        //最大数出现2048，则显示胜利
        for(int row = 0;row < 4;++row)
        {
            for(int col = 0;col <4;++col)
            {
                if(labels[row][col]->text() == "2048")
                {
                    //QMessageBox("你赢了",);
                    QMessageBox::information(0, "信息", "恭喜，你赢了!");
                     qDebug()<<"成功";
                     break;
                }
            }
        }
    }
}

void Widget::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
        case Qt::Key_Left:
            emit GestureMove(GestureDirect::LEFT);
            break;
        case Qt::Key_Right:
            emit GestureMove(GestureDirect::RIGHT);
            break;
        case Qt::Key_Down:
            emit GestureMove(GestureDirect::DOWN);
            break;
        case Qt::Key_Up:
           emit GestureMove(GestureDirect::UP);
            break;
        default:
            break;
    }
    QWidget::keyPressEvent(event);
}

void Widget::moveLabel(GestureDirect direction)
{
    bool isMove;
    switch (direction) {
    case LEFT:
        isMove = moveLeft();
        break;
    case RIGHT:
        isMove = moveRight();
        break;
    case UP:
        isMove = moveUp();
        break;
    case DOWN:
        isMove = moveDown();
        break;
    default:
        break;
    }

    //能移动才创建，不能移动不创建新的标签
    if(isMove){
        createLabel();
    }
}

bool Widget::moveUp()
{
    bool isMove = false; //是否能移动
    int i , j ; //i:记录行
                //j:记录当前元素是否合并过的行
    for(int col = 0;col < 4;++col)
    {
        j = -1;
        for(int row = 0;row < 4;++row)
        {
            //找到的第一个不为null的label
            if(labels[row][col] == NULL)
            {
                i = row + 1;
                while(i < 4)
                {
                    MyLabel *temp = labels[i][col];
                    if(temp != NULL)
                    {
                        isMove = true;
                        bool flag = isMergeUp(i,col);
                        if(flag && j != (row-1))
                        {
                            --row;
                            j = row;
                            --m_labelCount;
                        }else
                        {
                            //交换两个元素
                            labels[row][col] = temp;
                            j = -1; //将j重置为 -1;
                        }

                        int x = temp->x();
                        int y = row * m_rowHeight + m_yOffset;//y轴偏移

                        //动画让label往上升
                        QPropertyAnimation *animation = new QPropertyAnimation(temp,"geometry");
                        //开始值和结束值
                        animation->setStartValue(temp->geometry());
                        animation->setEndValue(QRect(x,y,temp->width(),temp->height()));
                        animation->setDuration(200);//动画播放时长
                        //设置动画的运动曲线
                        animation->setEasingCurve(QEasingCurve::Linear);
                        animation->start(QAbstractAnimation::DeleteWhenStopped);

                        if(flag){
                            delete temp;
                            temp = NULL;

                            labels[row][col]->reSetText(2*labels[row][col]->text().toInt());
                        }

                        labels[i][col] = NULL;
                        ++row;
                    }
                    ++i;
                }
            }
            else if(row + 1 < 4)
            {
                //如果该位置上有值，并且相邻的位置依然有值
                MyLabel *temp = labels[row + 1][col];
                bool flag = merge(temp,row,col);
                if(flag)
                {
                    isMove = true;
                    j = row + 1;
                    --row;
                }
            }
        }
    }

    return isMove;
}

bool Widget::moveDown()
{
    bool isMove = false; //是否能移动，不能移动
    int i , j ; //i:记录行
                //j:记录当前元素是否合并过的行
    for(int col = 0;col <4;++col)
    {
        for(int row = 3;row > -1;--row)
         {
            //找到的第一个不为null的label
            if(labels[row][col] == NULL)
            {
                i =row-1;
                while(i > -1)
                {
                    MyLabel *temp = labels[i][col];
                    if(temp != NULL)
                    {
                        isMove = true;

                        bool flag = isMergeDown(i,col);
                        if(flag && j != (row + 1))
                        {
                            ++row;
                            j = row;
                            --m_labelCount;
                        }else
                        {
                            //交换两个元素
                            labels[row][col] = temp;
                            j = -1;
                       }

                        int x= temp->x();
                        int y = row * m_rowHeight + m_yOffset;
                        //动画让label往下降
                        QPropertyAnimation *animation = new QPropertyAnimation(temp,"geometry");
                        //开始值和结束值
                        animation->setStartValue(temp->geometry());
                        animation->setEndValue(QRect(x,y,temp->width(),temp->height()));
                        animation->setDuration(200);//动画播放时长为
                        //设置动画的运动曲线
                        animation->setEasingCurve(QEasingCurve::Linear);
                        animation->start(QAbstractAnimation::DeleteWhenStopped);

                        if(flag){
                            delete temp;
                            temp = NULL;

                            labels[row][col]->reSetText(2*labels[row][col]->text().toInt());
                        }

                        labels[i][col] = NULL;
                        --row;
                        //this->repaint();
                    }
                    --i;
                }
            }
            else if(row-1>=0)
            {
                MyLabel *temp = labels[row-1][col];
                bool flag = merge(temp,row,col);
                if(flag)
                {
                    isMove = true;
                    j = row - 1;
                    ++row;
                }
            }
        }
    }
    return isMove;
}

bool Widget::moveLeft()
{
    bool isMove = false; //是否能移动
    int i , j ; //i:记录行
                //j:记录当前元素是否合并过的行
    for(int row = 0;row < 4;++row)
    {
        j = -1;
        for(int col = 0;col < 4;++col)
        {
            //找到的第一个不为null的label
            if(labels[row][col] == NULL)
            {
                i = col + 1;
                while(i < 4)
                {
                    MyLabel *temp = labels[row][i];
                    if(temp != NULL)
                    {
                        isMove = true;
                        bool flag = isMergeUp(row,i);
                        if(flag && j != (col-1))
                        {
                            --col;
                            j = col;
                            --m_labelCount;
                        }else
                        {
                            //交换两个元素
                            labels[row][col] = temp;
                            j = -1; //将j重置为 -1;
                        }

                        int x = col * m_colWidth + m_xOffset;
                        int y = temp->y();

                        //动画让label往左
                        QPropertyAnimation *animation = new QPropertyAnimation(temp,"geometry");
                        //开始值和结束值
                        animation->setStartValue(temp->geometry());
                        animation->setEndValue(QRect(x,y,temp->width(),temp->height()));
                        animation->setDuration(200);//动画播放时长
                        //设置动画的运动曲线
                        animation->setEasingCurve(QEasingCurve::Linear);
                        animation->start(QAbstractAnimation::DeleteWhenStopped);

                        if(flag){
                            delete temp;
                            temp = NULL;

                            labels[row][col]->reSetText(2*labels[row][col]->text().toInt());
                        }

                        labels[row][i] = NULL;
                        ++col;
                    }
                    ++i;
                }
            }
            else if(col + 1 < 4)
            {
                MyLabel *temp = labels[row][col+1];
                bool flag = merge(temp,row,col);
                if(flag)
                {
                    isMove = true;
                    j = col + 1;
                    --col;
                }
            }
        }
    }

    return isMove;
}

bool Widget::moveRight()
{
    bool isMove = false; //是否能移动，不能移动
    int i , j ; //i:记录行
                //j:记录当前元素是否合并过的行
    for(int row = 0;row < 4;++row)
     {
        for(int col = 3; col > -1;--col)
        {
            //找到的第一个不为null的label
            if(labels[row][col] == NULL)
            {
                i = col - 1;
                while(i > -1)
                {
                    MyLabel *temp = labels[row][i];
                    if(temp != NULL)
                    {
                        isMove = true;

                        bool flag = isMergeDown(row,i);
                        if(flag && j != (col + 1))
                        {
                            ++col;
                            j = col;
                            --m_labelCount;
                        }else
                        {
                            //交换两个元素
                            labels[row][col] = temp;
                            j = -1;
                        }

                        int x = col * m_colWidth + m_xOffset;
                        int y = temp->y();

                        //动画让label往右
                        QPropertyAnimation *animation = new QPropertyAnimation(temp,"geometry");
                        //开始值和结束值
                        animation->setStartValue(temp->geometry());
                        animation->setEndValue(QRect(x,y,temp->width(),temp->height()));
                        animation->setDuration(200);//动画播放时长为
                        //设置动画的运动曲线
                        animation->setEasingCurve(QEasingCurve::Linear);
                        animation->start(QAbstractAnimation::DeleteWhenStopped);

                        if(flag){
                            delete temp;
                            temp = NULL;

                            labels[row][col]->reSetText(2*labels[row][col]->text().toInt());
                        }

                        labels[row][i] = NULL;
                        --col;
                        //this->repaint();
                    }
                    --i;
                }
            }
            else if(col-1>=0) //这里可以不用判断是否越界，因为找到的是第一个不为空的元素
            {
                MyLabel *temp = labels[row][col-1];
                bool flag = merge(temp,row,col);
                if(flag)
                {
                    isMove = true;
                    j = col - 1;
                    ++col;
                }
            }
        }
    }
    return isMove;
}

bool Widget::isMergeUp(int row, int col)
{
    if(row != 0)
    {
        int up = row - 1;
        while((up >= 0) && (labels[up][col]==NULL))
        {
            --up;
        }

        if(up >=0 && labels[up][col]->text() == labels[row][col]->text())
        {
            return true;
        }
    }

    return false;
}

bool Widget::isMergeDown(int row, int col)
{
    if(row != 3)
    {
        int down = row + 1;
        while((down < 4) && (labels[down][col] == NULL))
        {
            ++down;
        }
        if(down < 4 && labels[down][col]->text() == labels[row][col]->text())
        {
            return true;
        }
    }

    return false;
}

bool Widget::isMergeLeft(int row, int col)
{
    if(col != 0)
    {
        int left = col - 1;
        while((left >=0 ) && (labels[row][left] == NULL))
        {
            --left;
        }

        if(left >= 0 && labels[row][left]->text() == labels[row][col]->text())
        {
           return true;
        }
    }

    return false;
}

bool Widget::isMergeRight(int row, int col)
{
    if(col != 3)
    {
        int right = col + 1;
        while((right < 4) && (labels[row][right] == NULL))
        {
            ++right;
        }
        if(right < 4 && labels[row][right]->text() == labels[row][col]->text())
        {
            return true;
        }
    }

    return false;
}
