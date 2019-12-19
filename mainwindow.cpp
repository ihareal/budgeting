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

QT_CHARTS_USE_NAMESPACE

// initialize user Id variable to delete user
int userId;
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // set authorization widget as default
    ui->stackedWidget->setCurrentIndex(0);

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
                 } else{
                    // setup user Id
                    users::Id = query.value(0).toInt();
                }

              }
            }
            try {
                if (isAdmin == true){
                    ui->stackedWidget->setCurrentIndex(2);

                    ui->administratorPage->setMinimumSize(851, 671);
                    ui->administratorPage->setMaximumSize(851, 671);

                    ui->administratorPage->setSizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);
                    ui->administratorPage->adjustSize();
                    adjustSize();

                    // initialization payment table with QSqlQuery method
                    // QSqlQueryModel *paymentModel = new QSqlQueryModel();
                    // QSqlQuery paymentQuery(QSqlDatabase::database());

                    // paymentQuery.prepare("select * from BudgetingDatabase.dbo.Payments");
                    // paymentQuery.exec();
                    // paymentModel->setQuery(paymentQuery);
                    // ui->paymentsTableView->setModel(paymentModel);

                    // set categories button active by default
                    ui->categoriesOpenTableBtn->
                                setStyleSheet("QPushButton{ background-color: #808080; color: white; }");

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
                } else{
                    // TODO: create singletone service to store user data (balance & id)
                    ui->userPage->setMinimumSize(895, 520);
                    this->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, ui->userPage->size(), qApp->desktop()->availableGeometry()));
                    ui->stackedWidget->setCurrentIndex(3);
                    ui->userPageStackedWidget->setCurrentIndex(0);
                    ui->personalStatisticButton->setStyleSheet("QPushButton{ background-color: #808080; color: white; }");

                    // check the user id
                    qDebug()<< users::Id;
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
                query1.prepare(QString("insert into BudgetingDatabase.dbo.Users (Balance, Login, Password) Values (0, :login, :password)" ));
                query1.bindValue(":login", login);
                query1.bindValue(":password", pwd);

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
            ui->paymentsDateEditField->setDate(query.value(5).toDate());

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
    QString date = ui->paymentsDateEditField_2->text();

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
            ui->paymentsDateEditField->setDate(query.value(5).toDate());

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
    QString date = ui->paymentsDateEditField->text();

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
                setStyleSheet("QPushButton{ background-color: #808080; color: white; }");

    // others to passive
    ui->usersOpenTableBtn->
            setStyleSheet("QPushButton{background-color: #F0F0F0; color: black}");
    ui->paymentsOpenTableBtn
            ->setStyleSheet("QPushButton{background-color: #F0F0F0; color: black}");
    ui->usersCategoriesOpenTableBtn
            ->setStyleSheet("QPushButton{background-color: #F0F0F0; color: black}");
}

void MainWindow::on_paymentsOpenTableBtn_clicked()
{
    ui->TablesStackedWidget->setCurrentIndex(1);

    // reserve color is #A1ACB3
    // set categories button with active color
    ui->paymentsOpenTableBtn->
                setStyleSheet("QPushButton{ background-color: #808080; color: white; }");

    // others to passive
    ui->usersOpenTableBtn->
            setStyleSheet("QPushButton{background-color: #F0F0F0; color: black}");
    ui->categoriesOpenTableBtn
            ->setStyleSheet("QPushButton{background-color: #F0F0F0; color: black}");
    ui->usersCategoriesOpenTableBtn
            ->setStyleSheet("QPushButton{background-color: #F0F0F0; color: black}");
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
    QSqlQuery query(QSqlDatabase::database());

    // query
    query.prepare("INSERT INTO BudgetingDatabase.dbo.Categories (Category) VALUES (:category)");

    // values
    QString category = ui->createCategoriesCostEditField->text();
    query.bindValue(":category", category);

    if(!query.exec()){
        QMessageBox::warning(this, "Database failed", "Error: The query hasn't executed.");
    } else {
        categoriesModel->select();
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
                setStyleSheet("QPushButton{ background-color: #808080; color: white; }");

    // others to passive
    ui->usersOpenTableBtn->
            setStyleSheet("QPushButton{background-color: #F0F0F0; color: black}");
    ui->categoriesOpenTableBtn
            ->setStyleSheet("QPushButton{background-color: #F0F0F0; color: black}");
    ui->paymentsOpenTableBtn
            ->setStyleSheet("QPushButton{background-color: #F0F0F0; color: black}");
}

void MainWindow::on_usersOpenTableBtn_clicked()
{
    ui->TablesStackedWidget->setCurrentIndex(3);
    // set users categories button with active color
    ui->usersOpenTableBtn->
                setStyleSheet("QPushButton{ background-color: #808080; color: white; }");

    // others to passive
    ui->usersCategoriesOpenTableBtn->
            setStyleSheet("QPushButton{background-color: #F0F0F0; color: black}");
    ui->categoriesOpenTableBtn
            ->setStyleSheet("QPushButton{background-color: #F0F0F0; color: black}");
    ui->paymentsOpenTableBtn
            ->setStyleSheet("QPushButton{background-color: #F0F0F0; color: black}");
}

void MainWindow::on_usersCategoriesCreateButton_clicked()
{
    QSqlQuery query(QSqlDatabase::database());

    // query
    query.prepare("INSERT INTO BudgetingDatabase.dbo.UsersCategories (UserId, CategoryId) VALUES (:userId, :categoryId)");

    // values
    int categoryId = ui->usersCategoriesCategoryIdCreateEditField->text().toInt();
    int userId = ui->usersCategoriesUserIdCreateEditField->text().toInt();
    query.bindValue(":categoryId", categoryId);
    query.bindValue(":userId", userId);

    if(!query.exec()){
        QMessageBox::warning(this, "Database failed", "Error: The query hasn't executed.");
    } else {
        userCategoriesModel->select();
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

void MainWindow::on_usersCategoriesDeleteButton_4_clicked()
{
    QSqlQuery query(QSqlDatabase::database());

    // query
    query.prepare("INSERT INTO BudgetingDatabase.dbo.Payments (Cost, UserId, Description, CategoryId, Date) VALUES (:cost, :userId, :description, :categoryId, :date)");

    // values
    double cost = ui->paymentsCostEditField_2->text().toDouble();
    qDebug()<< cost;
    int userId = ui->paymentsUserIdEditField_2->text().toInt();
    QString description = ui->paymentsDescriptionEditField_2->text();
    int categoryId = ui->paymentsCategoryIdEditField_2->text().toInt();
    QString date = ui->paymentsDateEditField_2->text();

    // bind values
    query.bindValue(":cost", cost);
    query.bindValue(":userId", userId);
    query.bindValue(":description", description);
    query.bindValue(":categoryId", categoryId);
    query.bindValue(":date", date);

    if(!query.exec()){
        QMessageBox::warning(this, "Database failed", "Error: The query hasn't executed.");
    } else {
        userModel->select();
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
    query.prepare("INSERT INTO BudgetingDatabase.dbo.Users (Login, Password, Balance) VALUES (:login, :password, :balance)");

    // values
    QString login = ui->usersLoginCreateEditField->text();
    QString password = ui->usersPasswordCreateEditField->text();
    double balance = ui->usersBalanceCreateEditField->text().toDouble();

    // bind values
    query.bindValue(":login", login);
    query.bindValue(":password", password);
    query.bindValue(":balance", balance);

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
        // draw chart if stacked widget index is on user page.
        QPieSeries *series = new QPieSeries();
        series->append("Category1", .2);
        series->append("Category2", .20);
        series->append("Category3", .50);
        series->append("Category4", .28);

        QPieSlice *slice0 = series->slices().at(0);
        slice0->setLabelVisible();

        QPieSlice *slice1 = series->slices().at(1);
        slice1->setExploded();
        slice1->setLabelVisible();
        slice1->setPen(QPen(Qt::darkCyan, 2));
        slice1->setBrush(Qt::green);

        QPieSlice *slice2 = series->slices().at(2);
        slice2->setLabelVisible();

        QPieSlice *slice3 = series->slices().at(3);
        slice3->setLabelVisible();

        QChart *chart = new QChart();
        chart->addSeries(series);
        chart->setTitle("TestChart");

        chart->legend()->hide();

        QChartView *chartView = new QChartView(chart);
        chartView->setRenderHint(QPainter::Antialiasing);

        QMainWindow window;
        ui->gridLayout_2->addWidget(chartView, 1, 2);
    }

}

void MainWindow::on_personalStatisticButton_clicked()
{
   ui->userPageStackedWidget->setCurrentIndex(0);

   // set button active
   ui->personalStatisticButton->setStyleSheet("QPushButton{ background-color: #808080; color: white; }");

   // others passive
   ui->billingReportButton->setStyleSheet("QPushButton{ background-color: #F0F0F0; color: black }");
   ui->financialHelperButton->setStyleSheet("QPushButton{ background-color: #F0F0F0; color: black }");
}

void MainWindow::on_billingReportButton_clicked()
{
    ui->userPageStackedWidget->setCurrentIndex(1);

    // set button active
    ui->billingReportButton->setStyleSheet("QPushButton{ background-color: #808080; color: white; }");

    // others passive
    ui->personalStatisticButton->setStyleSheet("QPushButton{ background-color: #F0F0F0; color: black }");
    ui->financialHelperButton->setStyleSheet("QPushButton{ background-color: #F0F0F0; color: black }");
}

void MainWindow::on_financialHelperButton_clicked()
{
    ui->userPageStackedWidget->setCurrentIndex(2);

    // set button active
    ui->financialHelperButton->setStyleSheet("QPushButton{ background-color: #808080; color: white; }");

    // others passive
    ui->personalStatisticButton->setStyleSheet("QPushButton{ background-color: #F0F0F0; color: black }");
    ui->billingReportButton->setStyleSheet("QPushButton{ background-color: #F0F0F0; color: black }");
}
