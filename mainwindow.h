#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QMainWindow>
#include <QMessageBox>
#include <QtSql>
#include <QtCore>

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

private:
    Ui::MainWindow *ui;
    QSqlQueryModel *model;
    QSqlDatabase db;
    QSqlRelationalTableModel *relationalModel;
};
#endif // MAINWINDOW_H
