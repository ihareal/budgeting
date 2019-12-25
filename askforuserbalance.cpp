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

void askForUserBalance::on_okButton_clicked()
{
    QSqlQuery query(QSqlDatabase::database());
    query.prepare(QString( "update BudgetingDatabase.dbo.Users SET Balance = :balance, isChecked = :isChecked where Id = :userId" ));
    bool isChecked = ui->checkBox->checkState();
    QString textBalance = ui->balanceLineEdit->text();
    double balance = ui->balanceLineEdit->text().toDouble();
    query.bindValue(":isChecked", isChecked);
    query.bindValue(":balance", balance);
    query.bindValue(":userId", users::Id);
    if (!query.exec()){
      QMessageBox::warning(this, "Database failed", "Error: The query hasn't executed.");
    } else {
      QMessageBox::information(this, "User notification.", "You data has successfully updated");
        users::Balance = balance;
        emit openUserPage();
        this->close();
    }
}

void askForUserBalance::on_pushButton_2_clicked()
{
    users::Balance = 0;
    this->close();
    emit openUserPage();
}

void askForUserBalance::on_askForUserBalance_rejected()
{
    users::Balance = 0;
    this->close();
    emit openUserPage();
}
