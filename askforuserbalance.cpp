#include "askforuserbalance.h"
#include "ui_askforuserbalance.h"
#include <QSqlQuery>
#include <QMessageBox>
#include "users.h"
#include <QDebug>

askForUserBalance::askForUserBalance(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::askForUserBalance)
{
    ui->setupUi(this);
}

askForUserBalance::~askForUserBalance()
{
    delete ui;
}

void askForUserBalance::on_buttonBox_accepted()
{
    QSqlQuery query(QSqlDatabase::database());
    query.prepare(QString("update BudgetingDatabase.dbo.Users SET Balance = :balance, isChecked = :isChecked where Id = :userId" ));
    qDebug() << users::Id;
    bool isChecked = ui->checkBox->checkState();
    double balance = ui->doubleSpinBox->value();
    query.bindValue(":isChecked", isChecked);
    query.bindValue(":balance", balance);
    query.bindValue(":userId", users::Id);
    if (!query.exec()){
      QMessageBox::warning(this, "Database failed", "Error: The query hasn't executed.");
    } else {
        // setup user Id
      QMessageBox::information(this, "User notification.", "You data has successfully updated");
    }

}
