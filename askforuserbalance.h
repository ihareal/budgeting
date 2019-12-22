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

signals:
     void openUserPage();

private slots:
    void on_okButton_clicked();

    void on_pushButton_2_clicked();

    void on_askForUserBalance_rejected();

private:
    Ui::askForUserBalance *ui;
};

#endif // ASKFORUSERBALANCE_H
