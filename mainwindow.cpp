#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "mainapplication.h"
#include "student.h"

#include <QVBoxLayout>
#include <qmessagebox.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    mainWindowInit();
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::mainWindowInit(){
    ui->groupBoxLogin->setMinimumSize(300, 300);
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(ui->groupBoxLogin); // Use ui->groupBoxLogin to access the group box
    layout->setAlignment(ui->groupBoxLogin, Qt::AlignCenter);

    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);
    ui->lineEditPassword->setPlaceholderText("Enter your password");
    ui->lineEditUsername->setPlaceholderText("Enter your username");
}


//TODO check from staffDataBase admin can
/*
    Admin password is hardcodded to software
    Admin can add Staff username and password
    and also can change username and password

*/


// void MainWindow::on_pushButtonLogin_clicked()
// {
//     QString username = ui->lineEditUsername->text();
//     QString userPassword = ui->lineEditPassword->text();

//     if(username == "admin" && userPassword =="admin" ){
//          QMessageBox::information(this, "Login","Username and password is correct");
//          mainApplication *mainWindow = new mainApplication;
//          mainWindow->show();
//          this->close();
//     }
//     else if(username =="test" && userPassword == "test"){
//         QMessageBox::information(this, "Login","Username and password is correct");
//        student * studentScreen = new student;
//        studentScreen->show();
//        this->close();

//     }
//     else{
//          QMessageBox::warning(this,"Login","Username or password in correct");
//     }




// }


void MainWindow::on_pushButtonLogin_clicked()
{
    QString username = ui->lineEditUsername->text();
    QString userPassword = ui->lineEditPassword->text();

    // Validate inputs
    if (username.isEmpty() || userPassword.isEmpty()) {
        QMessageBox::warning(this, "Login", "Username and password cannot be empty.");
        return;
    }

    QSqlQuery query;

    // Check if the input is numeric (student_id or professor_id)
    bool isNumeric = false;
    username.toInt(&isNumeric); // Check if `username` can be converted to an integer

    if (isNumeric) {
        // Query for numeric inputs (student_id or professor_id)
        query.prepare("SELECT password_hash, role FROM auth_users "
                      "WHERE student_id = :username::INTEGER "
                      "OR professor_id = :username::INTEGER");
    } else {
        // Query for alphanumeric input (username)
        query.prepare("SELECT password_hash, role FROM auth_users WHERE username = :username");
    }

    query.bindValue(":username", username);

    if (!query.exec()) {
        QMessageBox::critical(this, "Database Error", "Failed to query the database: " + query.lastError().text());
        return;
    }

    // Check if a user was found
    if (!query.next()) {
        QMessageBox::warning(this, "Login", "Invalid username, ID, or password.");
        return;
    }

    // Extract stored password and role
    QString storedPassword = query.value("password_hash").toString(); // Plain-text password for now
    QString role = query.value("role").toString();

    // Verify the password
    if (userPassword != storedPassword) { // Direct comparison for testing
        QMessageBox::warning(this, "Login", "Invalid password.");
        return;
    }

    // Login successful
    QMessageBox::information(this, "Login", "Login successful!");

    // Navigate to the appropriate screen based on the role
    if (role == "Admin") {
        mainApplication *mainWindow = new mainApplication;
        mainWindow->show();
        this->close();
    } else if (role == "Student") {
        student *studentScreen = new student;
        studentScreen->show();
        this->close();
    } else {
        QMessageBox::critical(this, "Login", "Unknown role.");
    }
}

