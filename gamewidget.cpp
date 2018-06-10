#include "GameWidget.h"
#include "ui_widget.h"
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

//生成的值
int CREATE_NUMBER[2] = {
    2,
    4
};

const int colWidth = 75;      //列宽
const int rowHeight = 75;     //行高
const int xOffset = 10;       //偏移距离
const int yOffset = 10;

GameWidget::GameWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    this->setWindowTitle("2048");

    //设置随机种子
    srand((size_t)time(NULL));

    //初始化主游戏场景
    m_bgWidget = new BGWidget(this);
    m_bgWidget->setGeometry(19,200,310,310);

    this->m_labelCount = 0;           //标签的数量
    //初始化游戏
    initGame();

    setScore();

    //移动
    connect(this,&GameWidget::GestureMove,this,&GameWidget::moveLabel);
    //设置分数
    connect(this,&GameWidget::ScoreChange,this,&GameWidget::setScore);
}

GameWidget::~GameWidget()
{
    delete ui;

    releaseRes();

    delete m_bgWidget;
}

void GameWidget::createLabel()
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
        int x = col * colWidth + xOffset;
        int y = row * rowHeight +yOffset;
        label->setGeometry(x,y,66,66);
        label->setParent(m_bgWidget);

        labels[row][col] = label;
        labels[row][col]->show();

        break;
    }

    ++m_labelCount;
}

void GameWidget::initGame()
{
    //初始化分数为0
    m_score = 0;
    m_highScore = 0;

    //初始化
    for(int row = 0;row < 4;++row)
    {
        for(int col = 0;col <4;++col)
        {
             labels[row][col] = NULL;
        }
    }

    //读取文件，最高分设置
    //ReadOnly文件不存在，打开失败
    //WriteOnly文件不存在，会自动创建文件
    //ReadWrite文件不存在，会自动创建文件
    //Append文件不存在，会自动创建文件
    //Truncate文件不存在，打开失败
    //Text文件不存在，打开失败
    //Unbuffered文件不存在，打开失败
    QFile *file = new QFile("data.json");
    if(file->open(QIODevice::ReadOnly))
    {
        QByteArray ba = file->readAll();

        QJsonDocument d = QJsonDocument::fromJson(ba);
        QJsonObject json = d.object();
        QJsonValue value = json.value(QString("m_highScore"));
        QJsonValue score = json.value(QString("m_score"));

        //最高分数
        m_highScore = value.toVariant().toInt();
        this->ui->best_2->setText(QString::number(m_highScore));

        //当前分数
        m_score = score.toVariant().toInt();
        this->ui->score_3->setText(QString::number(m_score));

        //文件保存的进度
        QJsonValue labelsArr = json.value(QString("labels"));
        QJsonArray arr = labelsArr.toArray();
        for(int i = 0;i< arr.size(); i++)
        {
            QJsonValue labelValue = arr.at(i);
            QJsonArray arr1 = labelValue.toArray();
            for(int j = 0; j< arr1.size(); j++)
            {
                QJsonValue arrValue =  arr1.at(j);
                int oldValue = arrValue.toVariant().toInt();
                if(oldValue != 0)
                {
                    MyLabel *label=new MyLabel(oldValue);
                    int x = j * colWidth + xOffset;
                    int y = i * rowHeight +yOffset;
                    label->setGeometry(x,y,66,66);
                    label->setParent(m_bgWidget);

                    labels[i][j] = label;
                    labels[i][j]->show();

                    ++m_labelCount;
                }
            }
        }

        file->close();

        // 判断读取的文件是否游戏结束
        // 这里可以不用判断，为了防止游戏在结束的时候程序意外关闭
        // gameOver();
    }
    else
    {
        //初始化两个标签
        for(int i=0;i<2;i++){
           createLabel();
        }
    }
}

bool GameWidget::merge(MyLabel *temp, int row, int col)
{
    if(temp != NULL)
    {
        //判断两个值是否相等，是合并
        if(temp->text() == labels[row][col]->text())
        {
            int x = labels[row][col]->x();
            int y = row * rowHeight + yOffset;//y轴偏移

            //动画效果
            QPropertyAnimation *animation = new QPropertyAnimation(temp,"geometry");
            animation->setStartValue(temp->geometry());
            animation->setEndValue(QRect(x,y,temp->width(),temp->height()));
            animation->setDuration(100);
            animation->setEasingCurve(QEasingCurve::Linear);
            animation->start(QAbstractAnimation::DeleteWhenStopped);

            int score = 2*labels[row][col]->text().toInt();
            labels[row][col]->reSetText(score);

            m_score += score;
            emit ScoreChange();

            --m_labelCount;

            return true;
        }
    }
    return false;
}

bool GameWidget::isMerge()
{
    for(int row = 0;row < 4;++row)
    {
        for(int col = 0;col <4;++col)
        {
              if(isMergeDown(row,col) || isMergeRight(row,col))
              {
                return true;
              }
        }
    }

    return false;
}

bool GameWidget::gameOver()
{    
    bool flag = false;
    //如果格子全满（m_labelCount == 16）
    if(m_labelCount == 16)
    {
        bool isWin = isMerge();
        if(!isWin){
            //没有可以合并的标签，显示失败
            QMessageBox::about(this, "信息", "失败!");
            flag = true;
        }
    }
    else
    {
        //最大数出现2048，则显示胜利
        for(int row = 0;row < 4;++row)
        {
            for(int col = 0;col <4;++col)
            {
                if(labels[row][col] != NULL && labels[row][col]->text() == "2048")
                {
                     QMessageBox::about(this, "信息", "恭喜，你赢了!");
                     flag = true;
                     break;
                }
            }
            if(flag)
            {
                break;
            }
        }
    }

    if(flag)
    {
        //删除数组，从头开始
        releaseRes();
        m_labelCount = 0;

        m_score = 0;
        emit ScoreChange();

        for(int i=0;i<2;i++){
           createLabel();
        }
    }

    return flag;
}

void GameWidget::saveGame()
{
    QFile *file = new QFile("data.json");
    if(file->open(QIODevice::WriteOnly))
    {
        QJsonDocument d;
        QJsonObject json = d.object();

        //进度
        QJsonArray arr;
        for(int i = 0;i< 4; i++)
        {
            QJsonArray arr1;
            for(int j = 0; j< 4; j++)
            {
                if(labels[i][j] != NULL)
                {
                    arr1.append(labels[i][j]->text());
                }
                else
                {
                    arr1.append(0);
                }
            }
            arr.append(arr1);
        }

        json.insert("labels",arr);
        //分数
        json.insert("m_highScore",m_highScore);
        json.insert("m_score",m_score);
        d.setObject(json);
        QByteArray ba = d.toJson(QJsonDocument::Indented);

        //将数据写入文件
        file->write(ba);
        file->close();
    }
}

void GameWidget::releaseRes()
{
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
}

void GameWidget::closeEvent(QCloseEvent *event)
{
    saveGame();
}

void GameWidget::setScore()
{
    this->ui->score_3->setText(QString::number(m_score));
    if(m_score > m_highScore)
    {
        m_highScore = m_score;
        this->ui->best_2->setText(QString::number(m_highScore));
    }
}

void GameWidget::keyPressEvent(QKeyEvent *event)
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

void GameWidget::moveLabel(GestureDirect direction)
{
    bool isMove = false;
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

    //游戏胜利结束
    bool isOver = gameOver();

    //能移动才创建，不能移动并且游戏结束不创建新的标签
    if(isMove && !isOver){
        createLabel();
    }

    //游戏失败结束
    if(!isOver)
    {
        gameOver();
    }
}

bool GameWidget::moveUp()
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
                        bool flag = false;
                        if(j != (row-1))
                        {
                            flag= isMergeUp(i,col);
                        }
                        if(flag)
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
                        int y = row * rowHeight + yOffset;//y轴偏移

                        //动画让label往上升
                        QPropertyAnimation *animation = new QPropertyAnimation(temp,"geometry");
                        //开始值和结束值
                        animation->setStartValue(temp->geometry());
                        animation->setEndValue(QRect(x,y,temp->width(),temp->height()));
                        animation->setDuration(100);//动画播放时长
                        //设置动画的运动曲线
                        animation->setEasingCurve(QEasingCurve::Linear);
                        animation->start(QAbstractAnimation::DeleteWhenStopped);

                        //改变标签值，并改变分数
                        if(flag){
                            int score = 2*labels[row][col]->text().toInt();
                            labels[row][col]->reSetText(score);

                            m_score += score;
                            emit ScoreChange();

                            delete temp;
                            temp = NULL;
                        }

                        labels[i][col] = NULL;
                        ++row;
                    }
                    ++i;
                }
            }
            else if(row + 1 < 4)
            {
                //当标签第一个不为空的时候
                //如果该位置上有值，并且相邻的位置依然有值
                MyLabel *temp = labels[row + 1][col];
                bool flag = merge(temp,row,col);
                if(flag)
                {
                    isMove = true;
                    j = row;

                    delete labels[row + 1][col];
                    labels[row + 1][col] = NULL;
                }
            }
        }
    }

    return isMove;
}

bool GameWidget::moveDown()
{
    bool isMove = false; //是否能移动，不能移动
    int i , j ; //i:记录行
                //j:记录当前元素是否合并过的行
    for(int col = 0;col <4;++col)
    {
        j = -1;
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

                        bool flag = false;
                        if(j != (row + 1))
                        {
                            flag= isMergeDown(i,col);
                        }
                        if(flag)
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
                        int y = row * rowHeight + yOffset;
                        //动画让label往下降
                        QPropertyAnimation *animation = new QPropertyAnimation(temp,"geometry");
                        //开始值和结束值
                        animation->setStartValue(temp->geometry());
                        animation->setEndValue(QRect(x,y,temp->width(),temp->height()));
                        animation->setDuration(100);//动画播放时长为
                        //设置动画的运动曲线
                        animation->setEasingCurve(QEasingCurve::Linear);
                        animation->start(QAbstractAnimation::DeleteWhenStopped);

                        //改变标签值，并改变分数
                        if(flag){
                            int score = 2*labels[row][col]->text().toInt();
                            labels[row][col]->reSetText(score);

                            m_score += score;
                            emit ScoreChange();

                            delete temp;
                            temp = NULL;
                        }

                        labels[i][col] = NULL;
                        --row;
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
                    j = row;

                    delete labels[row-1][col];
                    labels[row-1][col] = NULL;
                }
            }
        }
    }
    return isMove;
}

bool GameWidget::moveLeft()
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
                        bool flag = false;
                        if(j != (col-1))
                        {
                            flag= isMergeLeft(row,i);
                        }
                        if(flag)
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

                        int x = col * colWidth + xOffset;
                        int y = temp->y();

                        //动画让label往左
                        QPropertyAnimation *animation = new QPropertyAnimation(temp,"geometry");
                        //开始值和结束值
                        animation->setStartValue(temp->geometry());
                        animation->setEndValue(QRect(x,y,temp->width(),temp->height()));
                        animation->setDuration(100);//动画播放时长
                        //设置动画的运动曲线
                        animation->setEasingCurve(QEasingCurve::Linear);
                        animation->start(QAbstractAnimation::DeleteWhenStopped);

                        if(flag){
                            int score = 2*labels[row][col]->text().toInt();
                            labels[row][col]->reSetText(score);

                            m_score += score;
                            emit ScoreChange();

                            delete temp;
                            temp = NULL;

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
                    j = col;
                    delete labels[row][col+1];
                    labels[row][col+1] = NULL;
                }
            }
        }
    }

    return isMove;
}

bool GameWidget::moveRight()
{
    bool isMove = false; //是否能移动，不能移动
    int i , j ; //i:记录行
                //j:记录当前元素是否合并过的行
    for(int row = 0;row < 4;++row)
     {
        j=-1;
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
                        bool flag = false;
                        //判断当前的值是否被移动过，如果没有被移动过，判断是否能合并
                        if(j != (col + 1)){
                            flag = isMergeRight(row,i);
                        }

                        if(flag)
                        {
                            ++col;
                            j = col;
                            --m_labelCount;
                        }
                        else
                        {
                            //交换两个元素
                            labels[row][col] = temp;
                            j = -1;
                        }

                        int x = col * colWidth + xOffset;
                        int y = temp->y();

                        //动画让label往右
                        QPropertyAnimation *animation = new QPropertyAnimation(temp,"geometry");
                        //开始值和结束值
                        animation->setStartValue(temp->geometry());
                        animation->setEndValue(QRect(x,y,temp->width(),temp->height()));
                        animation->setDuration(100);//动画播放时长为
                        //设置动画的运动曲线
                        animation->setEasingCurve(QEasingCurve::Linear);
                        animation->start(QAbstractAnimation::DeleteWhenStopped);

                        if(flag){
                            int score = 2*labels[row][col]->text().toInt();
                            labels[row][col]->reSetText(score);

                            m_score += score;
                            emit ScoreChange();

                            delete temp;
                            temp = NULL;
                        }

                        labels[row][i] = NULL;
                        --col;
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
                    j = col;

                    delete labels[row][col-1];
                    labels[row][col-1] = NULL;
                }
            }
        }
    }
    return isMove;
}

bool GameWidget::isMergeUp(int row, int col)
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

bool GameWidget::isMergeDown(int row, int col)
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

bool GameWidget::isMergeLeft(int row, int col)
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

bool GameWidget::isMergeRight(int row, int col)
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
