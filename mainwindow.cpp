#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include "mainapplication.h"
#include "student.h"

#include <QVBoxLayout>
#include <qmessagebox.h>
#include <QCryptographicHash> // Include for hashing

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //for myself
    QString admin= hashPassword("hashed_admin123");
    qDebug() << admin;

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


// void MainWindow::on_pushButtonLogin_clicked()
// {
//     QString username = ui->lineEditUsername->text();
//     QString userPassword = ui->lineEditPassword->text();

//     // Validate inputs
//     if (username.isEmpty() || userPassword.isEmpty()) {
//         QMessageBox::warning(this, "Login", "Username and password cannot be empty.");
//         return;
//     }

//     QSqlQuery query;

//     // Determine whether the input is numeric or alphanumeric
//     bool isNumeric = false;
//     username.toInt(&isNumeric);

//     // Prepare query based on input type
//     if (isNumeric) {
//         query.prepare("SELECT password_hash, role FROM auth_users WHERE student_id = :username OR professor_id = :username");
//         query.bindValue(":username", username.toInt());
//     } else {
//         query.prepare("SELECT password_hash, role FROM auth_users WHERE username = :username");
//         query.bindValue(":username", username);
//     }




//     // Execute query and handle errors
//     if (!query.exec() || !query.next()) {
//         QMessageBox::warning(this, "Login", "Invalid username or password.");
//         return;
//     }

//     // Extract stored password hash and role
//     QString storedHash = query.value("password_hash").toString();
//     QString role = query.value("role").toString();

//     // Remove \x prefix from stored hash (if necessary)
//     if (storedHash.startsWith("\\x")) {
//         storedHash = storedHash.mid(2);
//     }

//     // Hash the provided password
//     QString hashedInputPassword = hashPassword(userPassword);

//     // Verify the hashed password
//     if (hashedInputPassword != storedHash) {
//         QMessageBox::warning(this, "Login", "Invalid username or password.");
//         return;
//     }

//     // Login successful
//     QMessageBox::information(this, "Login", "Login successful!");

//     // Navigate to the appropriate screen
//     navigateToRoleScreen(role);
// }

// Centralized role navigation function
void MainWindow::navigateToRoleScreen(const QString &role)
{
    if (role == "Admin") {
        mainApplication *mainWindow = new mainApplication;
        mainWindow->show();
        this->close();
    } else if (role == "Student") {
        student *studentScreen = new student(this);
        studentScreen->show();
        this->close();
    }  else {
        QMessageBox::critical(this, "Login", "Unknown role.");
    }
}

void MainWindow::setUserId(int userId)
{
    userId_ = userId;
}

int MainWindow::getUserId()
{
    return userId_;
}



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

    // Determine whether the input is numeric
    bool isNumeric = false;
    int numericValue = username.toInt(&isNumeric); // Attempt conversion

    if (isNumeric) {
        // Numeric input, query for student_id or professor_id
        query.prepare("SELECT password_hash, role, student_id FROM auth_users "
                      "WHERE student_id = :numericValue OR professor_id = :numericValue");
        query.bindValue(":numericValue", numericValue);
    } else {
        // Alphanumeric input, query for username
        query.prepare("SELECT password_hash, role, student_id FROM auth_users WHERE username = :username");
        query.bindValue(":username", username);
    }

    // Execute query and handle errors
    if (!query.exec()) {
        QMessageBox::critical(this, "Database Error", "Failed to query the database: " + query.lastError().text());
        return;
    }

    // Check if a user was found
    if (!query.next()) {
        QMessageBox::warning(this, "Login", "Invalid username or password.");
        return;
    }

    // Extract stored hash and role
    QString storedHash = query.value("password_hash").toString();
    QString role = query.value("role").toString();
    int studentId = query.value("student_id").toInt();

    // Debugging
    qDebug() << "Student ID:" << studentId;

    // Hash the provided password
    QString hashedInputPassword = QCryptographicHash::hash(userPassword.toUtf8(), QCryptographicHash::Sha256).toHex();

    // Verify the hashed password
    if (hashedInputPassword != storedHash) {
        QMessageBox::warning(this, "Login", "Invalid username or password.");
        return;
    }

    // Navigate based on role
    if (role == "Admin") {
        mainApplication *mainWindow = new mainApplication;
        mainWindow->show();
        this->close();
    } else if (role == "Student") {
        loadMarks(studentId);
        setUserId(studentId);
        student *studentScreen = new student(this);
        qDebug()<<"student id"<<studentId; // Pass student_id to Student Dashboard

        studentScreen->show();
        this->close();
    } else {
        QMessageBox::critical(this, "Login", "Unknown role.");
    }
}

void MainWindow::loadMarks(int studentId)
{
   // ui->
}
