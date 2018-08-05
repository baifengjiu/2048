#include "mylabel.h"
#include <QDebug>
#include <QString>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>

//每个数字对应的背景颜色
QString digitBkg[11] = {
    "rgb(238,228,218)",//2
    "rgb(237,224,200)",//4
    "rgb(242,177,121)",//8
    "rgb(245,149,99)",//16
    "rgb(246,124,95)",//32
    "rgb(246,94,59)",//64
    "rgb(237,207,114)",//128
    "rgb(237,204,97)",//256
    "rgb(237,200,80)",//512
    "rgb(237,197,63)",//1024
    "rgb(237,193,49)"//2048
};

//求解一个数是2的多少次方
int log_2(int value)
{
    if(value == 1){
        return 0;
    }else{
     return 1+log_2(value>>1);
    }
}

MyLabel::MyLabel(int text)
{
    this->setText(QString::number(text));
    this->setAlignment(Qt::Alignment(Qt::AlignCenter));
    this->setFont(QFont("Gadugi", 20, QFont::Bold));

     //初始化样式
    int index = log_2(text) - 1; //计算背景数组索引值
    QString fontColor = "color:rgb(255,255,255);";
    if(index < 8)
    {
        fontColor = "color:rgb(119,110,101);";
    }
    QString bgColor = QString("QLabel{background-color:%1;border-radius:5px;%2}").arg(digitBkg[index]).arg(fontColor);
    this->setStyleSheet(bgColor);

     //透明度
     QGraphicsOpacityEffect *m_pGraphicsOpacityEffect = new QGraphicsOpacityEffect(this);
     m_pGraphicsOpacityEffect->setOpacity(1);
     this->setGraphicsEffect(m_pGraphicsOpacityEffect);

     //动画让label慢慢出现
    QPropertyAnimation *animation = new QPropertyAnimation(m_pGraphicsOpacityEffect,"opacity",this);
    animation->setEasingCurve(QEasingCurve::Linear);
    animation->setDuration(400);
    animation->setStartValue(0);
    animation->setEndValue(1);
    animation->start(QAbstractAnimation::KeepWhenStopped);
}

void MyLabel::reSetText(int text)
{
    this->setText(QString::number(text));
    int index = log_2(text) - 1; //计算背景数组索引值
    QString fontColor = "color:rgb(255,255,255);";
    if(index < 8)
    {
        fontColor = "color:rgb(119,110,101);";
    }
    QString bgColor = QString("QLabel{background-color:%1;border-radius:5px;%2}").arg(digitBkg[index]).arg(fontColor);
    this->setStyleSheet(bgColor);
    this->show();
    this->repaint();
}

