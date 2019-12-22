#ifndef ASKFORUSERBALANCE_H
#define ASKFORUSERBALANCE_H

#include <QDialog>

namespace Ui {
class askForUserBalance;
}

class askForUserBalance : public QDialog
{
    Q_OBJECT

public:
    explicit askForUserBalance(QWidget *parent = nullptr);
    ~askForUserBalance();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::askForUserBalance *ui;
};

#endif // ASKFORUSERBALANCE_H
