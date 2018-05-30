#ifndef MYLABEL_H
#define MYLABEL_H

#include <QString>
#include <QLabel>

class MyLabel : public QLabel
{
public:
    MyLabel(int name);

    void reSetText(int text);
};

#endif // MYLABEL_H
