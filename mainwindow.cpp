#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDesktopWidget>
#include <QMainWindow>
#include <QStyle>
#include <QDesktopWidget>
#include <payments.h>
#include "dbhelper.h"
#include <categories.h>
#include <userscategories.h>
#include <users.h>
#include <QApplication>
#include <QtWidgets/QMainWindow>
#include <QtCharts/QChartView>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QLegend>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QHorizontalStackedBarSeries>
#include <QtCharts/QLineSeries>
#include <QtCharts/QCategoryAxis>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <QPen>
#include <QtGui>
#include <iostream>
#include <map>
#include <list>

QT_CHARTS_USE_NAMESPACE

// initialize user Id variable to delete user
int userId;
QChartView *chartView;
// function to check duplicates before adding new category
// true - such category exists
// false - you can insert new one
bool MainWindow::checkDuplicatedCategories(QString category){

    QSqlQuery query(QSqlDatabase::database());

    // query
    query.prepare("select * from BudgetingDatabase.dbo.Categories categories where categories.Category = :category");
    query.bindValue(":category", category);

    if(query.exec()){
       if (query.first()){
           return true;
       } else{
           return false;
       }
    } else{
        QMessageBox::warning(this, "Database failed", "Error: The query hasn't executed.");
    }
}

// function to check duplicates before adding new user category
// true - such category exists
// false - you can insert new one
bool MainWindow::checkDuplicatedUsersCategories(int categoryId, int userId){

   QSqlQuery query(QSqlDatabase::database());

   // query
   query.prepare("select * from BudgetingDatabase.dbo.UsersCategories ucat where ucat.CategoryId = :categoryId and ucat.UserId = :userId");
   query.bindValue(":categoryId", categoryId);
   query.bindValue(":userId", userId);

   if (query.exec()){
        if(query.first()){
            return true;
        } else {
            return false;
        }
   } else {
        QMessageBox::warning(this, "Database failed", "Error: The query hasn't executed.");
   }
}

void MainWindow::openUserPage(){
    ui->stackedWidget->setCurrentIndex(3);
    resize(871, 502);
    qDebug()<<"Main window user balance"<<users::Balance;
    ui->userBalanceLabel->setText(users::Balance);
    this->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, ui->userPage->size(), qApp->desktop()->availableGeometry()));
    ui->userPageStackedWidget->setCurrentIndex(0);
    // set first button to active
    ui->personalStatisticButton->setStyleSheet("QPushButton{ background-color: black; border-style: outset; border-width: 1px; border-color:  black; border-radius: 20px; color: white; }");

    // others passive
    ui->billingReportButton->
            setStyleSheet("QPushButton{ "
                          "background-color: #F0F0F0;"
                          "border-style: outset;"
                          "border-width: 1px;"
                          "border-color: black;"
                          "border-radius: 20px;"
                          "color: black; }"
                          "QPushButton:hover { background-color: white;}");

    ui->financialHelperButton->
            setStyleSheet("QPushButton{ "
                          "background-color: #F0F0F0;"
                          "border-style: outset;"
                          "border-width: 1px;"
                          "border-color: black;"
                          "border-radius: 20px;"
                          "color: black; }"
                          "QPushButton:hover { background-color: white;}");
}


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    userBalanceWidget = new askForUserBalance();
    connect(userBalanceWidget, &askForUserBalance::openUserPage, this, &MainWindow::openUserPage);

    // set authorization widget as default
    ui->stackedWidget->setCurrentIndex(0);

    // setup date by default
    QDateTime setPaymentDateTime = QDateTime::currentDateTimeUtc();
    ui->paymentsCreateDateEdit->setDateTime(setPaymentDateTime);
    ui->paymentsUpdateDateEditField->setDateTime(setPaymentDateTime);

    // Some design features
    ui->createNewAccountPage->adjustSize();
    ui->authorizationPage->adjustSize();    
    adjustSize();
    ui->notificationLabel->show();
    ui->passwordEditField->setEchoMode(QLineEdit::Password);
    ui->createPasswordEditField->setEchoMode(QLineEdit::Password);
    messageBox = new QMessageBox();
    db = QSqlDatabase::addDatabase("QODBC");

    // db.setDatabaseName("DRIVER={SQL Server};SERVER=AMD_A10-7850K\\SQLEXPRESS;DATABASE=BudgetingDatabase;Trusted_Connection=true");

    db.setHostName("AMD_A10-7850K\\SQLEXPRESS");
    db.setDatabaseName("mydsn64");  
    db.open();

    QString stringTest;

    if (!db.open()){
        QMessageBox::warning(this, "Database failed", "There is no connection with database.");
    }

    //    this->model = new QSqlQueryModel();
    //    model->setQuery("SELECT pd.CreationDate, pd.Latitude, pd.Longitude, ud.District, ud.isAdmin FROM EcoServiceDB.dbo.UserDetails ud inner join EcoServiceDB.dbo.PollutionDetails pd on ud.UserId = pd.UserId");
    //    ui->tableView->setModel(model);

    // ******* DATABASE OPENING CHECKING *******
    //    if (db.open()){
    //        messageBox->setText("Success");
    //        qDebug()<<"Opened";
    //    }
    //    else{
    //        messageBox ->setText("Failure(");
    //        qDebug()<<db.lastError().text();
    //    }


    // ***** RELATIONAL MODEL *****

    // relationalModel = new QSqlRelationalTableModel(this);
    // relationalModel->setTable("EcoServiceDB.dbo.UserDetails");
    // relationalModel->setRelation(0, QSqlRelation("EcoServiceDB.dbo.PollutionDetails","UserId", "PollutionId"));
    // QSqlRelation("PollutionDetails", "CreationDate", "Latitude", "Longitude")
    // relationalModel->select();
    // ui->tableView->setModel(relationalModel);
    // messageBox->show();



    // ******** StyleSheet for CreateAccountButton ********
    // ui->createAccountButton->
    //        setStyleSheet("QPushButton{ background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 white, stop: 1 grey); border-radius: 10px; }"
    //                      "QPushButton:hover{ background-color: red; border-radius: 0 px; }");


}

MainWindow::~MainWindow()
{
    delete ui;  
}

void MainWindow::on_loginButton_clicked()
{    
    QString pwd = ui->passwordEditField->text();
    QString login = ui->loginEditField->text();
    bool isAdmin = false;

    // Check for null and set query
    if(pwd.length() != 0 && login.length() != 0 ){

        QSqlQuery query(QSqlDatabase::database());
        query.prepare(QString("select * from BudgetingDatabase.dbo.Users users where users.Login = :login and users.Password = :password" ));
        query.bindValue(":login", login);
        query.bindValue(":password", pwd);        

        // execute the query
        if (!query.exec()){
           QMessageBox::warning(this, "Database failed", "Error: The query hasn't executed.");
        } else {
            while(query.next()){
             if (query.first()){
                QMessageBox::information(this, "Authorization notification", "You are successfully logged up.");
                if(query.value(1).toString() == "admin" && query.value(2).toString() == "admin"){                    
                    isAdmin = true;

                    // special admin id
                    users::Id = 999;
                 } else {
                    // setup user Id                    
                    users::Id = query.value(0).toInt();

                    double checkUserBalance = query.value(3).toDouble();
                    QString balance = query.value(3).toString();
                    if (checkUserBalance == 0){
                        userBalanceWidget->show();
                    } else {
                        users::Balance = query.value(3).toString();
                        openUserPage();
                    }
                }
              }
            }
            try {
                if (isAdmin == true){

                    ui->stackedWidget->setCurrentIndex(2);

                    // ui->administratorPage->setMinimumSize(851, 671);
                    // ui->administratorPage->setMaximumSize(851, 671);

                    // ui->administratorPage->setSizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);
                    // ui->administratorPage->adjustSize();
                    // adjustSize();

                    resize(872, 752);

                    // initialization payment table with QSqlQuery method
                    // QSqlQueryModel *paymentModel = new QSqlQueryModel();
                    // QSqlQuery paymentQuery(QSqlDatabase::database());

                    // paymentQuery.prepare("select * from BudgetingDatabase.dbo.Payments");
                    // paymentQuery.exec();
                    // paymentModel->setQuery(paymentQuery);
                    // ui->paymentsTableView->setModel(paymentModel);

                    // set categories button active by default
                    ui->categoriesOpenTableBtn->
                                setStyleSheet("QPushButton{ background-color: black; border-style: outset; border-width: 1px; border-color:  black; border-radius: 20px; color: white; }");

                    // others to passive
                    ui->usersOpenTableBtn->
                            setStyleSheet("QPushButton{ "
                                          "background-color: #F0F0F0;"
                                          "border-style: outset;"
                                          "border-width: 1px;"
                                          "border-color: black;"
                                          "border-radius: 20px;"
                                          "color: black; }"
                                          "QPushButton:hover { background-color: white; }");

                    ui->paymentsOpenTableBtn->
                            setStyleSheet("QPushButton{ "
                                          "background-color: #F0F0F0;"
                                          "border-style: outset;"
                                          "border-width: 1px;"
                                          "border-color: black;"
                                          "border-radius: 20px;"
                                          "color: black; }"
                                          "QPushButton:hover { background-color: white; }");

                    ui->usersCategoriesOpenTableBtn->
                            setStyleSheet("QPushButton{ "
                                          "background-color: #F0F0F0;"
                                          "border-style: outset;"
                                          "border-width: 1px;"
                                          "border-color: black;"
                                          "border-radius: 20px;"
                                          "color: black; }"
                                          "QPushButton:hover { background-color: white; }");

                    ui->TablesStackedWidget->setCurrentIndex(0);
                    // *********** CATEGORIES ***********
                    // categories model
                    categoriesModel = new QSqlTableModel();
                    categoriesModel->setTable("BudgetingDatabase.dbo.Categories");
                    categoriesModel->select();
                    // disable categories triggers
                    ui->categoriesTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
                    // set category model to tableView
                    ui->categoriesTableView->setModel(categoriesModel);
                    // *********** CATEGORIES ***********

                    // *********** USERS CATEGORIES ***********
                    // user categories model
                    userCategoriesModel = new QSqlTableModel();
                    userCategoriesModel->setTable("BudgetingDatabase.dbo.UsersCategories");
                    userCategoriesModel->select();
                    // set users categories model to tableView
                    ui->usersCategoriesTableView->setModel(userCategoriesModel);
                    // *********** USERS CATEGORIES ***********

                    // *********** USERS ***********
                    userModel = new QSqlTableModel();
                    userModel->setTable("BudgetingDatabase.dbo.Users");
                    userModel->select();
                    // disable users triggers
                    ui->usersTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
                    // set users model to tableView
                    ui->usersTableView->setModel(userModel);
                    // *********** USERS ***********

                    // *********** PAYMENT ***********
                    // payment model
                    paymentModel = new QSqlTableModel();
                    paymentModel->setTable("BudgetingDatabase.dbo.Payments");
                    paymentModel->select();
                    // disable payment triggers
                    ui->paymentsTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
                    // set payment model to tableView
                    ui->paymentsTableView->setModel(paymentModel);
                    // *********** PAYMENT ***********

                    this->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, ui->administratorPage->size(), qApp->desktop()->availableGeometry()));
                }

                if (!query.first()){
                    QMessageBox::critical(this, "Authorization failed", "Error: Please, check your credentials.");
                }
            } catch (std::exception & e) {
                qDebug() << &e;
            }

        }
    } else {
        QMessageBox::warning(this, "Authorization failed", "Error: Please, enter your credentials.");
    }

}

void MainWindow::on_createAccountButton_clicked()
{

//    QStackedWidget *stack = new QStackedWidget();
//    stack->addWidget(ui->createNewAccountPage);
//    QVBoxLayout *layout = new QVBoxLayout(stack);

//    layout->addWidget(stack);
//    ui->stackedWidget->setCurrentIndex(1);
//    ui->createNewAccountPage->setFixedSize(500, 1000);
//    stack->setCurrentWidget(ui->createNewAccountPage);
//    this->setFixedSize(stack->currentWidget()->size());

    ui->stackedWidget->setCurrentIndex(1);
//    ui->authorizationPage->setSizePolicy(QSizePolicy::Policy::Ignored, QSizePolicy::Policy::Ignored);
//    ui->createNewAccountPage->setSizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred);
//    ui->stackedWidget->setFixedSize(433, 485);
//    ui->stackedWidget->adjustSize();
//    adjustSize();

//    QMainWindow w;
//    QDesktopWidget dw;

//    int ox = dw.width();
//    int oy = dw.height();

//    qDebug()<<ox;
//    qDebug()<<oy;



//    w.setFixedSize(ox, oy);


}

void MainWindow::on_passwordEditField_textChanged(const QString &pwd)
{
    QString login = ui->loginEditField->text();

    if (pwd.length() != 0 && login.length() != 0){
        ui->notificationLabel->hide();
    } else if(pwd.length() == 0 && login.length() == 0){
        ui->notificationLabel->setText("Empty fields");
        ui->notificationLabel->show();
    } else if (pwd.length() != 0 ){
        ui->notificationLabel->setText("Please, enter your login");
    } else{
        ui->notificationLabel->setText("Please, enter your password");
        ui->notificationLabel->show();
    }
}

void MainWindow::on_loginEditField_textChanged(const QString &login)
{
    QString pwd = ui->passwordEditField->text();

    if (login.length() != 0 && pwd.length() !=0) {
        ui->notificationLabel->hide();
    } else if(login.length() == 0 && pwd.length() == 0){
                ui->notificationLabel->setText("Empty fields");
                ui->notificationLabel->show();
    } else if (login.length() != 0){
        ui->notificationLabel->setText("Please, enter your password");
        ui->notificationLabel->show();
    }  else{
        ui->notificationLabel->setText("Please, enter your login");
        ui->notificationLabel->show();
    }
}

void MainWindow::on_createAccountButton_2_clicked()
{
    QString pwd = ui->createPasswordEditField->text();
    QString login = ui->createLoginEditField->text();
    double balance = ui->balanceEditField->text().toDouble();
    QVariant currency = ui->currencyComboBox->itemText(ui->currencyComboBox->currentIndex());
    bool checker = false;
    int isChecked = 0;

    // no need at showing dialog for users who have balance.
    if (balance > 0){
        checker = true;
        isChecked = 1;
    }else {
        checker = false;
        isChecked = 0;
    }

    qint32 id = 0;

    if (pwd.length() != 0 && login.length() != 0){

        QSqlQuery query(QSqlDatabase::database());
        query.prepare(QString("select distinct(users.Login) from BudgetingDatabase.dbo.Users users where users.Login = :login" ));
        query.bindValue(":login", login);

        // execute the query
        if (!query.exec()){
           QMessageBox::warning(this, "Database failed", "Error: The query hasn't executed.");
        } else {
            while(query.next()){
             // throw an exception and write id variable for further interaction with it.
             if (query.value(0).toInt() > 0){               
                id = query.value(0).toInt();                
                QMessageBox::information(this, "Authorization notification", "User with such login is already exists!");
              }
             }

            if (id == 0){
                // if we didn't found the user, we can create the new one.
                QSqlQuery query1(QSqlDatabase::database("MyConnect"));
                query1.prepare(QString("insert into BudgetingDatabase.dbo.Users (Balance, Login, Password, Currency, IsChecked) Values (:balance, :login, :password, :currency, :isChecked)" ));
                query1.bindValue(":balance", balance);
                query1.bindValue(":login", login);
                query1.bindValue(":password", pwd);
                query1.bindValue(":currency", currency);
                query1.bindValue(":isChecked", isChecked);

                // check for query execution.
                if (!query1.exec()){
                   QMessageBox::warning(this, "Database failed", "Error: The query hasn't executed.");
                } else {
                        QMessageBox::information(this, "Create account notification", "You are successfully create account.");
                        ui->stackedWidget->setCurrentIndex(0);
                      }
            }
           }
    } else {
        QMessageBox::warning(this, "Account creation failed", "Empty fields");
    }



}

void MainWindow::on_createPasswordEditField_textChanged(const QString &pwd)
{
    QString login = ui->createLoginEditField->text();

    if (pwd.length() != 0 && login.length() != 0){
        ui->createNotificationLabel->hide();
    } else if(pwd.length() == 0 && login.length() == 0){
        ui->createNotificationLabel->setText("Empty fields");
        ui->createNotificationLabel->show();
    } else if (pwd.length() != 0 ){
        ui->createNotificationLabel->setText("Please, enter your login");
    } else{
        ui->createNotificationLabel->setText("Please, enter your password");
        ui->createNotificationLabel->show();
    }

}

void MainWindow::on_createLoginEditField_textChanged(const QString &login)
{
    QString pwd = ui->createPasswordEditField->text();

    if (login.length() != 0 && pwd.length() !=0) {
        ui->createNotificationLabel->hide();
    } else if(login.length() == 0 && pwd.length() == 0){
                ui->createNotificationLabel->setText("Empty fields");
                ui->createNotificationLabel->show();
    } else if (login.length() != 0){
        ui->createNotificationLabel->setText("Please, enter your password");
        ui->createNotificationLabel->show();
    }  else{
        ui->createNotificationLabel->setText("Please, enter your login");
        ui->createNotificationLabel->show();
    }
}

void MainWindow::on_goBackToLoginButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);    
}


void MainWindow::on_paymentsTableView_activated(const QModelIndex &index)
{
    QString checkedColumnData = ui->paymentsTableView->model()->data(index).toString();

    qDebug()<< index.column();
    qDebug()<< index.row();
    qDebug()<< index;
    QSqlQuery query(QSqlDatabase::database());
    DbHelper::Row = index.row();
    qDebug()<< Payments::Id;

    QModelIndex customIndex = ui->paymentsTableView->model()->index(index.row(), 0);

    int id = ui->paymentsTableView->model()->data(customIndex).toInt();
    query.prepare(QString("select * from BudgetingDatabase.dbo.Payments payments where payments.Id = :value"));
    query.bindValue(":value", id);
    Payments::Id = id;
    if(query.exec()){
        while (query.next()) {
            // fill update edit fields
            ui->paymentsIdEditField->setText(query.value(0).toString());
            ui->paymentsCostEditField->setText(query.value(1).toString());
            ui->paymentsUserIdEditField->setText(query.value(2).toString());
            ui->paymentsDescriptionEditField->setText(query.value(3).toString());
            ui->paymentsCategoryIdEditField->setText(query.value(4).toString());
            ui->paymentsUpdateDateEditField->setDate(query.value(5).toDate());

            // fill create edit fields
//            ui->paymentsCostEditField_2->setText(query.value(1).toString());
//            ui->paymentsUserIdEditField_2->setText(query.value(2).toString());
//            ui->paymentsDescriptionEditField_2->setText(query.value(3).toString());
//            ui->paymentsCategoryEditField_2->setText(query.value(4).toString());
//            ui->paymentsDateEditField_2->setDate(query.value(5).toDate());

            // fill delete id field
            ui->DeletePaymentIdEdit->setText(query.value(0).toString());
        }
    } else {
        QMessageBox::warning(this, "Database failed", "Error: The query hasn't executed.");
    }

}

void MainWindow::on_paymentDeleteBtn_clicked()
{
    QMessageBox::StandardButton reply = QMessageBox::warning(this, "Delete warning", "Attention: Do you really want to delete this entity?",
                                                             QMessageBox::Ok | QMessageBox::Cancel);

    if (reply == QMessageBox::Ok){
        // delete row
    } else {
       // nothing
    }
}

void MainWindow::on_paymentUpdateBtn_clicked()
{
    QMessageBox::StandardButton reply = QMessageBox::warning(this, "Update warning", "Attention: Do you really want to update this entity?",
                                                             QMessageBox::Ok | QMessageBox::Cancel); 
    if (reply == QMessageBox::Ok){
        // update row
        qDebug()<<Payments::Id;
        qDebug()<<Payments::Date;
    } else {
       // nothing
    }
}

void MainWindow::on_DeletePaymentButton_clicked()
{
    QMessageBox::StandardButton reply = QMessageBox::warning(this, "Delete warning", "Attention: Do you really want to delete this entity?",
                                       QMessageBox::Ok | QMessageBox::Cancel);

    QSqlQuery query(QSqlDatabase::database());
    if (reply == QMessageBox::Ok){
        // delete row
        int id = Payments::Id;
        int manualId = ui->DeletePaymentIdEdit->text().toInt();
        qDebug()<<"Payments::Id "<<id;
        query.prepare(QString("Delete from BudgetingDatabase.dbo.Payments where Id = :value"));
        if (manualId != 0){
            query.bindValue(":value", manualId);
        } else {
            query.bindValue(":value", id);
        }

        if (!query.exec()){
            QMessageBox::warning(this, "Database failed", "Error: The query hasn't executed.");
        } else {
            // update the table
            // ui->paymentsTableView->hideRow(DbHelper::Row);
            paymentModel->select();
        }
    } else {
       // nothing
    }
}


void MainWindow::on_paymentCreateBtn_2_clicked()
{
    QSqlQuery query(QSqlDatabase::database());

    // query
    query.prepare("INSERT INTO BudgetingDatabase.dbo.Payments (Cost, UserId, Description, CategoryId, Date) VALUES (:cost, :userId, :description, :categoryId, :date)");

    // values
    double cost = ui->paymentsCostEditField_2->text().toDouble();
    int userId = ui->paymentsUserIdEditField_2->text().toInt();
    QString description = ui->paymentsDescriptionEditField_2->text();
    int categoryId = ui->paymentsCategoryIdEditField_2->text().toInt();
    QDateTime date = ui->paymentsCreateDateEdit->dateTime();

    // bind values
    query.bindValue(":cost", cost);
    query.bindValue(":userId", userId);
    query.bindValue(":description", description);
    query.bindValue(":categoryId", categoryId);
    query.bindValue(":date", date);

    if(!query.exec()){
        QMessageBox::warning(this, "Database failed", "Error: The query hasn't executed.");
    } else {
        paymentModel->select();
    }
}

void MainWindow::on_paymentsTableView_doubleClicked(const QModelIndex &index)
{
    QString checkedColumnData = ui->paymentsTableView->model()->data(index).toString();

    QSqlQuery query(QSqlDatabase::database());
    DbHelper::Row = index.row();  

    QModelIndex customIndex = ui->paymentsTableView->model()->index(index.row(), 0);

    int id = ui->paymentsTableView->model()->data(customIndex).toInt();
    query.prepare(QString("select * from BudgetingDatabase.dbo.Payments payments where payments.Id = :value"));
    query.bindValue(":value", id);
    Payments::Id = id;
    if(query.exec()){
        while (query.next()) {
            // fill update edit fields
            ui->paymentsIdEditField->setText(query.value(0).toString());
            ui->paymentsCostEditField->setText(query.value(1).toString());
            ui->paymentsUserIdEditField->setText(query.value(2).toString());
            ui->paymentsDescriptionEditField->setText(query.value(3).toString());
            ui->paymentsCategoryIdEditField->setText(query.value(4).toString());
            ui->paymentsUpdateDateEditField->setDate(query.value(5).toDate());

            // fill create edit fields
//            ui->paymentsCostEditField_2->setText(query.value(1).toString());
//            ui->paymentsUserIdEditField_2->setText(query.value(2).toString());
//            ui->paymentsDescriptionEditField_2->setText(query.value(3).toString());
//            ui->paymentsCategoryIdEditField_2->setText(query.value(4).toString());
//            ui->paymentsDateEditField_2->setDate(query.value(5).toDate());

            // fill delete id field
            ui->DeletePaymentIdEdit->setText(query.value(0).toString());
        }
    } else {
        QMessageBox::warning(this, "Database failed", "Error: The query hasn't executed.");
    }

}

void MainWindow::on_paymentUpdateButton_clicked()
{
    QSqlQuery query(QSqlDatabase::database());

    // query
    query.prepare("UPDATE BudgetingDatabase.dbo.Payments SET Cost = :cost, UserId = :userId, Description = :description, CategoryId = :categoryId, Date = :date WHERE Id = :id");

    // values
    int id = ui->paymentsIdEditField->text().toInt();
    double cost = ui->paymentsCostEditField->text().toDouble();
    int userId = ui->paymentsUserIdEditField->text().toInt();
    int categoryId = ui->paymentsCategoryIdEditField->text().toInt();
    QString description = ui->paymentsDescriptionEditField->text();
    QDateTime date = ui->paymentsUpdateDateEditField->dateTime();

    // bind values
    query.bindValue(":cost", cost);
    query.bindValue(":userId", userId);
    query.bindValue(":categoryId", categoryId);
    query.bindValue(":date", date);
    query.bindValue(":id", id);
    query.bindValue(":description", description);

    if(!query.exec()){
        QMessageBox::warning(this, "Database failed", "Error: The query hasn't executed.");
    } else{
        paymentModel->select();
        QMessageBox::information(this,"Database notification", "Entity has been successfully updated.");
    }
}

void MainWindow::on_categoriesOpenTableBtn_clicked()
{
    ui->TablesStackedWidget->setCurrentIndex(0);

    // reserve color is #A1ACB3
    // set categories button with active color
    ui->categoriesOpenTableBtn->
            setStyleSheet("QPushButton{ background-color: black;"
                          "border-style: outset; border-width: 1px;"
                          "border-color:  black; border-radius: 20px;"
                          "color: white; }");

    // others to passive
    ui->usersOpenTableBtn->
            setStyleSheet("QPushButton{ "
                          "background-color: #F0F0F0;"
                          "border-style: outset;"
                          "border-width: 1px;"
                          "border-color: black;"
                          "border-radius: 20px;"
                          "color: black; }"
                          "QPushButton:hover { background-color: white; }");

    ui->paymentsOpenTableBtn->
            setStyleSheet("QPushButton{ "
                          "background-color: #F0F0F0;"
                          "border-style: outset;"
                          "border-width: 1px;"
                          "border-color: black;"
                          "border-radius: 20px;"
                          "color: black; }"
                          "QPushButton:hover { background-color: white; }");

    ui->usersCategoriesOpenTableBtn->
            setStyleSheet("QPushButton{ "
                          "background-color: #F0F0F0;"
                          "border-style: outset;"
                          "border-width: 1px;"
                          "border-color: black;"
                          "border-radius: 20px;"
                          "color: black; }"
                          "QPushButton:hover { background-color: white; }");
}

void MainWindow::on_paymentsOpenTableBtn_clicked()
{
    ui->TablesStackedWidget->setCurrentIndex(1);

    // reserve color is #A1ACB3
    // set categories button with active color
    ui->paymentsOpenTableBtn->
                setStyleSheet("QPushButton{ background-color: black;"
                              "border-style: outset; border-width: 1px;"
                              "border-color:  black; border-radius: 20px;"
                              "color: white; }");

    // others to passive
    ui->usersOpenTableBtn->
            setStyleSheet("QPushButton{ "
                          "background-color: #F0F0F0;"
                          "border-style: outset;"
                          "border-width: 1px;"
                          "border-color: black;"
                          "border-radius: 20px;"
                          "color: black; }"
                          "QPushButton:hover { background-color: white; }");

    ui->categoriesOpenTableBtn
            ->setStyleSheet("QPushButton{ "
                            "background-color: #F0F0F0;"
                            "border-style: outset;"
                            "border-width: 1px;"
                            "border-color: black;"
                            "border-radius: 20px;"
                            "color: black; }"
                            "QPushButton:hover { background-color: white; }");

    ui->usersCategoriesOpenTableBtn
            ->setStyleSheet("QPushButton{ "
                            "background-color: #F0F0F0;"
                            "border-style: outset;"
                            "border-width: 1px;"
                            "border-color: black;"
                            "border-radius: 20px;"
                            "color: black; }"
                            "QPushButton:hover { background-color: white; }");
}

void MainWindow::on_categoriesTableView_doubleClicked(const QModelIndex &index)
{
    QString checkedColumnData = ui->paymentsTableView->model()->data(index).toString();

    QSqlQuery query(QSqlDatabase::database());
    DbHelper::Row = index.row();
    QModelIndex customIndex = ui->categoriesTableView->model()->index(index.row(), 0);

    int id = ui->categoriesTableView->model()->data(customIndex).toInt();
    Categories::Id = id;
    query.prepare(QString("select * from BudgetingDatabase.dbo.Categories categories where categories.Id = :value"));
    query.bindValue(":value", id);
    if(query.exec()){
        while (query.next()) {
            // fill update edit fields
            ui->categoriesIdEditField->setText(query.value(0).toString());
            ui->categoriesCategoryEditField->setText(query.value(1).toString());

            // fill create edit fields
            // ui->createCategoriesCostEditField->setText(query.value(1).toString());

            // fill delete id field
            ui->deleteCategoriesIdEditField->setText(query.value(0).toString());
        }
    } else {
        QMessageBox::warning(this, "Database failed", "Error: The query hasn't executed.");
    }
}

void MainWindow::on_categoriesCreateButton_clicked()
{

    // values
    QString category = ui->createCategoriesCostEditField->text();    

    // if there is no such category (equals false result) we have opportunity to insert
    if(checkDuplicatedCategories(category) == false){
        // query
        QSqlQuery query(QSqlDatabase::database());
        query.prepare("INSERT INTO BudgetingDatabase.dbo.Categories (Category) VALUES (:category)");
        query.bindValue(":category", category);

        if(!query.exec()){
            QMessageBox::warning(this, "Database failed", "Error: The query hasn't executed.");
        } else {
            categoriesModel->select();
        }
    } else {
        QMessageBox::warning(this, "Database failed", "Error: Such category already exists.");
    }

}

void MainWindow::on_categoriesDeleteButton_clicked()
{
    QMessageBox::StandardButton reply = QMessageBox::warning(this, "Delete warning", "Attention: Do you really want to delete this category?",
                                       QMessageBox::Ok | QMessageBox::Cancel);

    QSqlQuery query(QSqlDatabase::database());
    if (reply == QMessageBox::Ok){
        // delete row
        int id = Categories::Id;
        int manualId = ui->deleteCategoriesIdEditField->text().toInt();
        query.prepare(QString("Delete from BudgetingDatabase.dbo.Categories where Id = :value"));
        if (manualId != 0){
            query.bindValue(":value", manualId);
        } else {
            query.bindValue(":value", id);
        }

        if (!query.exec()){
            QMessageBox::warning(this, "Database failed", "Error: The query hasn't executed.");
        } else {
            // update the table
            categoriesModel->select();
        }
    } else {
       // Nothing happens if we answer no...
    }
}

void MainWindow::on_categoriesUpdateButton_clicked()
{
    QSqlQuery query(QSqlDatabase::database());

    // query
    query.prepare("UPDATE BudgetingDatabase.dbo.Categories SET Category = :category WHERE Id = :id");

    // values
    QString category = ui->categoriesCategoryEditField->text();
    int id = ui->categoriesIdEditField->text().toInt();

    // bind values
    query.bindValue(":category", category);
    query.bindValue(":id", id);

    if(!query.exec()){
        QMessageBox::warning(this, "Database failed", "Error: The query hasn't executed.");
    } else{
        categoriesModel->select();
        QMessageBox::information(this,"Database notification", "Entity has been successfully updated.");
    }
}

void MainWindow::on_usersCategoriesOpenTableBtn_clicked()
{
    ui->TablesStackedWidget->setCurrentIndex(2);

    // set users categories button with active color
    ui->usersCategoriesOpenTableBtn->
                setStyleSheet("QPushButton{ "
                              "background-color: black;"
                              "border-style: outset;"
                              "border-width: 1px;"
                              "border-color: black;"
                              "border-radius: 20px;"
                              "color: white; }");

    // others to passive
    ui->usersOpenTableBtn->
            setStyleSheet("QPushButton{ "
                          "background-color: #F0F0F0;"
                          "border-style: outset;"
                          "border-width: 1px;"
                          "border-color: black;"
                          "border-radius: 20px;"
                          "color: black; }"
                          "QPushButton:hover { background-color: white; }");

    ui->categoriesOpenTableBtn->
            setStyleSheet("QPushButton{ "
                          "background-color: #F0F0F0;"
                          "border-style: outset;"
                          "border-width: 1px;"
                          "border-color: black;"
                          "border-radius: 20px;"
                          "color: black; }"
                          "QPushButton:hover { background-color: white; }");

    ui->paymentsOpenTableBtn->
            setStyleSheet("QPushButton{ "
                          "background-color: #F0F0F0;"
                          "border-style: outset;"
                          "border-width: 1px;"
                          "border-color: black;"
                          "border-radius: 20px;"
                          "color: black; }"
                          "QPushButton:hover { background-color: white; }");
}

void MainWindow::on_usersOpenTableBtn_clicked()
{
    ui->TablesStackedWidget->setCurrentIndex(3);
    // set users categories button with active color
    ui->usersOpenTableBtn->
            setStyleSheet("QPushButton{ "
                          "background-color: black;"
                          "border-style: outset;"
                          "border-width: 1px;"
                          "border-color: black;"
                          "border-radius: 20px;"
                          "color: white; }");

    // others to passive
    ui->usersCategoriesOpenTableBtn->
            setStyleSheet("QPushButton{ "
                          "background-color: #F0F0F0;"
                          "border-style: outset;"
                          "border-width: 1px;"
                          "border-color: black;"
                          "border-radius: 20px;"
                          "color: black; }"
                          "QPushButton:hover { background-color: white; }");

    ui->categoriesOpenTableBtn->
            setStyleSheet("QPushButton{ "
                          "background-color: #F0F0F0;"
                          "border-style: outset;"
                          "border-width: 1px;"
                          "border-color: black;"
                          "border-radius: 20px;"
                          "color: black; }"
                          "QPushButton:hover { background-color: white; }");

    ui->paymentsOpenTableBtn->
            setStyleSheet("QPushButton{ "
                          "background-color: #F0F0F0;"
                          "border-style: outset;"
                          "border-width: 1px;"
                          "border-color: black;"
                          "border-radius: 20px;"
                          "color: black; }"
                          "QPushButton:hover { background-color: white; }");
}

void MainWindow::on_usersCategoriesCreateButton_clicked()
{

    int categoryId = ui->usersCategoriesCategoryIdCreateEditField->text().toInt();
    int userId = ui->usersCategoriesUserIdCreateEditField->text().toInt();
    if (checkDuplicatedUsersCategories(categoryId, userId) == false){
        QSqlQuery query(QSqlDatabase::database());
        // query
        query.prepare("INSERT INTO BudgetingDatabase.dbo.UsersCategories (UserId, CategoryId) VALUES (:userId, :categoryId)");

        // bind values
        query.bindValue(":categoryId", categoryId);
        query.bindValue(":userId", userId);
        if(!query.exec()){
            QMessageBox::warning(this, "Database failed", "Error: The query hasn't executed.");
        } else {
            userCategoriesModel->select();
        }
    } else {
        QMessageBox::warning(this, "Database failed", "Error: Such user category already exists.");
    }

}

void MainWindow::on_usersCategoriesTableView_doubleClicked(const QModelIndex &index)
{
    QString checkedColumnData = ui->usersCategoriesTableView->model()->data(index).toString();

    QSqlQuery query(QSqlDatabase::database());
    DbHelper::Row = index.row();
    QModelIndex customIndex = ui->categoriesTableView->model()->index(index.row(), 0);

    int id = ui->usersCategoriesTableView->model()->data(customIndex).toInt();
    usersCategories::UserId = id;
    query.prepare(QString("select * from BudgetingDatabase.dbo.UsersCategories usersCategories where usersCategories.UserId = :value"));
    query.bindValue(":value", id);
    if(query.exec()){
        while (query.next()) {
            // fill delete id field
//            ui->usersCategoriesUserIdDeleteEditField->setText(query.value(0).toString());
//            ui->usersCategoriesCategoryIdDeleteEditField->setText(query.value(1).toString());
        }
    } else {
        QMessageBox::warning(this, "Database failed", "Error: The query hasn't executed.");
    }
}

void MainWindow::on_usersCategoriesDeleteButton_clicked()
{
    QMessageBox::StandardButton reply = QMessageBox::warning(this, "Delete warning", "Attention: Do you really want to delete this entity?",
                                       QMessageBox::Ok | QMessageBox::Cancel);

    QSqlQuery query(QSqlDatabase::database());
    if (reply == QMessageBox::Ok){
        // delete row
        int userId = usersCategories::UserId;
        int manualId = ui->usersCategoriesUserIdDeleteEditField->text().toInt();
        int categoryId = ui->usersCategoriesCategoryIdDeleteEditField->text().toInt();
        query.prepare(QString("Delete from BudgetingDatabase.dbo.UsersCategories where UserId = :userId and CategoryId = :categoryId"));
        if (manualId != 0){
            query.bindValue(":userId", manualId);
            query.bindValue(":categoryId", categoryId);
        } else {
            query.bindValue(":userId", userId);
            query.bindValue(":categoryId", categoryId);
        }

        if (!query.exec()){
            QMessageBox::warning(this, "Database failed", "Error: The query hasn't executed.");
        } else {
            // update the table
        }
        userCategoriesModel->select();
    } else {
       // Nothing happens if we answer no...
    }
}

void MainWindow::on_usersCategoriesTableView_2_doubleClicked(const QModelIndex &index)
{
    QString checkedColumnData = ui->usersCategoriesTableView->model()->data(index).toString();

    QSqlQuery query(QSqlDatabase::database());
    DbHelper::Row = index.row();
    QModelIndex customIndex = ui->usersCategoriesTableView->model()->index(index.row(), 0);

    int id = ui->usersCategoriesTableView->model()->data(customIndex).toInt();
    usersCategories::UserId = id;
    query.prepare(QString("select * from BudgetingDatabase.dbo.UsersCategories usersCategories where usersCategories.UserId = :value"));
    query.bindValue(":value", id);
    if(query.exec()){
        while (query.next()) {
            // fill delete id field
            ui->usersCategoriesUserIdDeleteEditField->setText(query.value(0).toString());
            ui->usersCategoriesCategoryIdDeleteEditField->setText(query.value(1).toString());
        }
    } else {
        QMessageBox::warning(this, "Database failed", "Error: The query hasn't executed.");
    }
}

void MainWindow::on_usersTableView_doubleClicked(const QModelIndex &index)
{
    QString checkedColumnData = ui->usersTableView->model()->data(index).toString();

    QSqlQuery query(QSqlDatabase::database());
    DbHelper::Row = index.row();
    QModelIndex customIndex = ui->usersTableView->model()->index(index.row(), 0);

    int id = ui->usersTableView->model()->data(customIndex).toInt();
    userId = id;
    query.prepare(QString("select * from BudgetingDatabase.dbo.Users users where users.Id = :value"));
    query.bindValue(":value", id);
    if(query.exec()){
        while (query.next()) {
            // fill delete id field
            ui->userDeleteIdEditField->setText(query.value(0).toString());

            // fill update fields
            ui->usersIdUpdateEditField->setText(query.value(0).toString());
            ui->usersLoginUpdateEditField->setText(query.value(1).toString());
            ui->usersPasswordUpdateEditField->setText(query.value(2).toString());
            ui->usersBalanceUpdateEditField->setText(query.value(3).toString());
            int index = ui->usersUpdateCurrencyComboBox->findText(query.value(4).toString());
            uint checker = query.value(5).toUInt();
            if (index >= 0){
                ui->usersUpdateCurrencyComboBox->setCurrentIndex(index);
            } else {}
            if(checker == 1){
                ui->usersUpdateCheckBox->setChecked(1);
            } else if(checker == 0) {
                ui->usersUpdateCheckBox->setChecked(0);
            }

        }
    } else {
        QMessageBox::warning(this, "Database failed", "Error: The query hasn't executed.");
    }
}

void MainWindow::on_usersDeleteButton_clicked()
{
    QMessageBox::StandardButton reply = QMessageBox::warning(this, "Delete warning", "Attention: Do you really want to delete this entity?",
                                       QMessageBox::Ok | QMessageBox::Cancel);
    qDebug()<< "Check global user variable"<<userId;
    QSqlQuery query(QSqlDatabase::database());
    if (reply == QMessageBox::Ok){
        // delete row
        int id = userId;
        int manualId = ui->userDeleteIdEditField->text().toInt();
        query.prepare(QString("Delete from BudgetingDatabase.dbo.Users where Id = :value"));
        if (manualId != 0){
            query.bindValue(":value", manualId);
        } else {
            query.bindValue(":value", id);
        }

        if (!query.exec()){
            QMessageBox::warning(this, "Database failed", "Error: The query hasn't executed.");
        } else {
            // update the table
            // ui->paymentsTableView->hideRow(DbHelper::Row);
            userModel->select();
        }
    } else {
       // nothing
    }
}

void MainWindow::on_usersUpdateButton_clicked()
{
    QSqlQuery query(QSqlDatabase::database());

    // query
    query.prepare("UPDATE BudgetingDatabase.dbo.Users SET Login = :login, Password = :password, Balance = :balance WHERE Id = :id");

    // values
    int id = ui->usersIdUpdateEditField->text().toInt();
    QString login = ui->usersLoginUpdateEditField->text();
    QString password = ui->usersPasswordUpdateEditField->text();
    double balance = ui->usersBalanceUpdateEditField->text().toDouble();

    // bind values
    query.bindValue(":login", login);
    query.bindValue(":password", password);
    query.bindValue(":balance", balance);
    query.bindValue(":id", id);
    if(!query.exec()){
        QMessageBox::warning(this, "Database failed", "Error: The query hasn't executed.");
    } else{
        userModel->select();
        QMessageBox::information(this,"Database notification", "Entity has been successfully updated.");
    }
}

void MainWindow::on_usersCreateButton_clicked()
{
    QSqlQuery query(QSqlDatabase::database());

    // query
    //    query.prepare("INSERT INTO BudgetingDatabase.dbo.Users (Login, Password, Balance) VALUES (:login, :password, :balance)");
    query.prepare(QString("insert into BudgetingDatabase.dbo.Users (Balance, Login, Password, Currency, IsChecked) Values (:balance, :login, :password, :currency, :isChecked)" ));

    // values
    QString login = ui->usersLoginCreateEditField->text();
    QString password = ui->usersPasswordCreateEditField->text();
    double balance = ui->usersBalanceCreateEditField->text().toDouble();
    QString currency = ui->userCreateCurrenciesComboBox->itemText(ui->currencyComboBox->currentIndex());
    bool checker = false;
    int isChecked = 0;

    if (balance > 0){
        checker = true;
        isChecked = 1;
    } else{
        checker = false;
        isChecked = 0;
    }

    // bind values
    query.bindValue(":login", login);
    query.bindValue(":password", password);
    query.bindValue(":balance", balance);
    query.bindValue(":currency", currency);
    query.bindValue(":isChecked", isChecked);

    if(!query.exec()){
        QMessageBox::warning(this, "Database failed", "Error: The query hasn't executed.");
    } else {
        userModel->select();
    }
}

void MainWindow::on_stackedWidget_currentChanged(int arg1)
{
    qDebug()<< "Stacked widget current change event: " << arg1;

    // find current index
    // ui->TablesStackedWidget->currentIndex()
    if (arg1 == 3){

        ui->userBalanceLabel->setText(users::Balance);

        // draw chart if stacked widget index is on user page.
        QPieSeries *series = new QPieSeries();
        series->append("Category1", .2);
        series->append("Category2", .20);
        series->append("Category3", .50);
        series->append("Category4", .20);

//        QPieSlice *slice0 = series->slices().at(0);
//        slice0->setLabelVisible();

//        QPieSlice *slice1 = series->slices().at(1);
//        slice1->setExploded();
//        slice1->setLabelVisible();
//        slice1->setPen(QPen(Qt::darkCyan, 2));
//        slice1->setBrush(Qt::green);

//        QPieSlice *slice2 = series->slices().at(2);
//        slice2->setLabelVisible();

//        QPieSlice *slice3 = series->slices().at(3);
//        slice3->setLabelVisible();

        QSqlQuery getUserCategoriesIdsQuery(QSqlDatabase::database());
        getUserCategoriesIdsQuery.prepare("SELECT * FROM BudgetingDatabase.dbo.UsersCategories ucat where ucat.UserId = :userId");
        getUserCategoriesIdsQuery.bindValue(":userId", users::Id);
        QList<int> userCategoriesIds;
        if (getUserCategoriesIdsQuery.exec()){
            while (getUserCategoriesIdsQuery.next()) {
                userCategoriesIds.append(getUserCategoriesIdsQuery.value(1).toInt());
            }
        } else {
           QMessageBox::warning(this, "Database failed.", "Error: Doesn't match users categories id");
        }

        for (int var = 0; var < userCategoriesIds.size(); ++var) {
            qDebug()<<userCategoriesIds[var];
        }

        for (int i = 0; i < 4; i++){
            QPieSlice *slice = series->slices().at(i);
            slice->setLabelVisible();
        }


        QChart *chart = new QChart();
        chart->addSeries(series);
        chart->setTitle("TestChart");

        // chart->legend()->hide();

        chartView = new QChartView(chart);
        chartView->setRenderHint(QPainter::Antialiasing);

        QMainWindow window;
        ui->gridLayout->addWidget(chartView, 1, 2);
    }

}

void MainWindow::on_personalStatisticButton_clicked()
{
   ui->userPageStackedWidget->setCurrentIndex(0);

   // set button active
   ui->personalStatisticButton->
           setStyleSheet("QPushButton{ "
                         "background-color: black;"
                         "border-style: outset;"
                         "border-width: 1px;"
                         "border-color: black;"
                         "border-radius: 20px;"
                         "color: white; }");
   // others passive
   ui->billingReportButton->
           setStyleSheet("QPushButton{ "
                         "background-color: #F0F0F0;"
                         "border-style: outset;"
                         "border-width: 1px;"
                         "border-color: black;"
                         "border-radius: 20px;"
                         "color: black; }"
                         "QPushButton:hover { background-color: white; }");

   ui->financialHelperButton->
           setStyleSheet("QPushButton{ "
                         "background-color: #F0F0F0;"
                         "border-style: outset;"
                         "border-width: 1px;"
                         "border-color: black;"
                         "border-radius: 20px;"
                         "color: black; }"
                         "QPushButton:hover { background-color: white; }");
}

void MainWindow::on_billingReportButton_clicked()
{
    ui->userPageStackedWidget->setCurrentIndex(1);

    // set button active
    ui->billingReportButton->
            setStyleSheet("QPushButton{ "
                          "background-color: black;"
                          "border-style: outset;"
                          "border-width: 1px;"
                          "border-color: black;"
                          "border-radius: 20px;"
                          "color: white; }");

    // others passive
    ui->personalStatisticButton->
            setStyleSheet("QPushButton{ "
                          "background-color: #F0F0F0;"
                          "border-style: outset;"
                          "border-width: 1px;"
                          "border-color: black;"
                          "border-radius: 20px;"
                          "color: black; }"
                          "QPushButton:hover { background-color: white; }");
    ui->financialHelperButton->
            setStyleSheet("QPushButton{ "
                          "background-color: #F0F0F0;"
                          "border-style: outset;"
                          "border-width: 1px;"
                          "border-color: black;"
                          "border-radius: 20px;"
                          "color: black; }"
                          "QPushButton:hover { background-color: white; }");
}

void MainWindow::on_financialHelperButton_clicked()
{
    ui->userPageStackedWidget->setCurrentIndex(2);

    // set button active
    ui->financialHelperButton->
            setStyleSheet("QPushButton{ "
                          "background-color: black;"
                          "border-style: outset;"
                          "border-width: 1px;"
                          "border-color: black;"
                          "border-radius: 20px;"
                          "color: white; }");

    // others passive
    ui->personalStatisticButton->
            setStyleSheet("QPushButton{ "
                          "background-color: #F0F0F0;"
                          "border-style: outset;"
                          "border-width: 1px;"
                          "border-color: black;"
                          "border-radius: 20px;"
                          "color: black; }"
                          "QPushButton:hover { background-color: white; }");

    ui->billingReportButton->
            setStyleSheet("QPushButton{ "
                          "background-color: #F0F0F0;"
                          "border-style: outset;"
                          "border-width: 1px;"
                          "border-color: black;"
                          "border-radius: 20px;"
                          "color: black; }"
                          "QPushButton:hover { background-color: white; }");
}

void MainWindow::on_actionLog_out_triggered()
{
    // we logged out the user
    if (users::Id != 0){
        users::Id = 0;
        ui->stackedWidget->setCurrentIndex(0);
        ui->loginEditField->clear();
        ui->passwordEditField->clear();
        resize(416, 387);
        this->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, ui->authorizationPage->size(), qApp->desktop()->availableGeometry()));
        // adjustSize();
    } else{
        QMessageBox::information(this, "User notification", "<p align=\"center\">You have no opportunity to log out wihtout being logged in.\nPlease log in at first </p>");
    }
}


void MainWindow::on_userPageStackedWidget_currentChanged(int arg1)
{

}
