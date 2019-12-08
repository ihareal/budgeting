#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDesktopWidget>
#include <QMainWindow>
#include <QStyle>
#include <QDesktopWidget>

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
                 }
              }
            }
            try {
                if (isAdmin == true){
                    ui->stackedWidget->setCurrentIndex(2);

                    ui->administratorPage->setMinimumSize(828, 678);
                    ui->administratorPage->adjustSize();
                    adjustSize();

                    // initialization payment table
                    QSqlQueryModel *paymentModel = new QSqlQueryModel();
                    QSqlQuery paymentQuery(QSqlDatabase::database());

                    paymentQuery.prepare("select * from BudgetingDatabase.dbo.Payments");
                    paymentQuery.exec();
                    paymentModel->setQuery(paymentQuery);
                    ui->paymentsTableView->setModel(paymentModel);

                    this->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, ui->administratorPage->size(), qApp->desktop()->availableGeometry()));
                } else{
                    // TODO: create singletone service to store user data (balance & id)
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
    QSqlQuery query(QSqlDatabase::database("newConnection"));

    if (index.column() == 5){
        query.prepare(QString("select * from BudgetingDatabase.dbo.Payments payments where payments.Id = :value or payments.Cost = :value or payments.UserId = :value or payments.Description = :value or payments.Category = :value or payments.Date = :value"));
        query.bindValue(":value", checkedColumnData);
    }
    query.prepare(QString("select * from BudgetingDatabase.dbo.Payments payments where payments.Id = :value or payments.Cost = :value or payments.UserId = :value or payments.Description = :value or payments.Category = :value or payments.Date = :value"));
    query.bindValue(":value", checkedColumnData);
    if(query.exec()){
        while (query.next()) {
            ui->paymentsIdEditField->setText(query.value(0).toString());
            ui->paymentsCostEditField->setText(query.value(1).toString());
            ui->paymentsUserIdEditField->setText(query.value(2).toString());
            ui->paymentsDescriptionEditField->setText(query.value(3).toString());
            ui->paymentsCategoryEditField->setText(query.value(4).toString());
            ui->paymentsDateEditField->setDate(query.value(5).toDate());
        }
    } else{
        QMessageBox::warning(this, "Database failed", "Error: The query hasn't executed.");
    }
    qDebug()<< index;
}
