#ifndef MYLABEL_H
#define MYLABEL_H

#include <QLabel>

class MyLabel : public QLabel
{
public:
    MyLabel(int text);

    void reSetText(int text);
};

#endif // MYLABEL_H
