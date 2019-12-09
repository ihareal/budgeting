#ifndef PAYMENTS_H
#define PAYMENTS_H
#include <QString>
#include <QDate>

class Payments
{
public:
    static int Id;
    static float Cost;
    static int UserId;
    static QString Description;
    static int Category;
    static QDate Date;
    Payments();
};

#endif // PAYMENTS_H
