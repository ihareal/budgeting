#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QMainWindow>
#include <QMessageBox>
#include <QtSql>
#include <QtCore>
#include "askforuserbalance.h"
#include <map>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QMessageBox *messageBox;

    bool checkDuplicatedUsersCategories(int categoryId, int userId);

    bool checkDuplicatedCategories(QString str);

private slots:

    void on_loginButton_clicked();

    void on_createAccountButton_clicked();

    void on_passwordEditField_textChanged(const QString &arg1);

    void on_loginEditField_textChanged(const QString &arg1);

    void on_createAccountButton_2_clicked();

    void on_createPasswordEditField_textChanged(const QString &arg1);

    void on_createLoginEditField_textChanged(const QString &arg1);
    void on_goBackToLoginButton_clicked();

    void on_paymentsTableView_activated(const QModelIndex &index);

    void on_paymentDeleteBtn_clicked();

    void on_paymentUpdateBtn_clicked();

    void on_DeletePaymentButton_clicked();

    void on_paymentCreateBtn_2_clicked();

    void on_paymentsTableView_doubleClicked(const QModelIndex &index);

    void on_paymentUpdateButton_clicked();

    void on_categoriesOpenTableBtn_clicked();

    void on_paymentsOpenTableBtn_clicked();

    void on_categoriesTableView_doubleClicked(const QModelIndex &index);

    void on_categoriesCreateButton_clicked();

    void on_categoriesDeleteButton_clicked();

    void on_categoriesUpdateButton_clicked();

    void on_usersCategoriesOpenTableBtn_clicked();

    void on_usersOpenTableBtn_clicked();

    void on_usersCategoriesCreateButton_clicked();

    void on_usersCategoriesTableView_doubleClicked(const QModelIndex &index);

    void on_usersCategoriesDeleteButton_clicked();

    void on_usersCategoriesTableView_2_doubleClicked(const QModelIndex &index);

    void on_usersTableView_doubleClicked(const QModelIndex &index);

    void on_usersDeleteButton_clicked();

    void on_usersUpdateButton_clicked();

    void on_usersCreateButton_clicked();

    void on_stackedWidget_currentChanged(int arg1);

    void on_personalStatisticButton_clicked();

    void on_billingReportButton_clicked();

    void on_financialHelperButton_clicked();

    void on_actionLog_out_triggered();

    void openUserPage();

    void on_userPageStackedWidget_currentChanged(int arg1);

private:
    Ui::MainWindow *ui;
    QSqlQueryModel *model;
    QSqlTableModel *paymentModel;
    QSqlTableModel *userModel;
    QSqlTableModel *categoriesModel;
    QSqlTableModel *userCategoriesModel;
    QSqlDatabase db;
    QSqlRelationalTableModel *relationalModel;
    askForUserBalance *userBalanceWidget;
};
#endif // MAINWINDOW_H
