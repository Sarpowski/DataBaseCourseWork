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


    mainWindowInit();
}

MainWindow::~MainWindow()
{
    delete ui;
}

QString MainWindow::hashPassword(const QString &plainPassword)
{
    return QCryptographicHash::hash(plainPassword.toUtf8(), QCryptographicHash::Sha256).toHex();
}



void MainWindow::mainWindowInit(){
    ui->groupBoxLogin->setMinimumSize(300, 300);
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(ui->groupBoxLogin);
    layout->setAlignment(ui->groupBoxLogin, Qt::AlignCenter);

    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);
    ui->lineEditPassword->setPlaceholderText("Enter your password");
    ui->lineEditUsername->setPlaceholderText("Enter your username");
}



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

    if (username.isEmpty() || userPassword.isEmpty()) {
        QMessageBox::warning(this, "Login", "Username and password cannot be empty.");
        return;
    }

    QSqlQuery query;

    bool isNumeric = false;
    int numericValue = username.toInt(&isNumeric); // Attempt conversion

    if (isNumeric) {
        // Numeric input, query for student_id or professor_id
        query.prepare("SELECT password_hash, role, student_id FROM auth_users "
                      "WHERE student_id = :numericValue OR professor_id = :numericValue");
        query.bindValue(":numericValue", numericValue);
    } else {
        // Alphanumeric input
        query.prepare("SELECT password_hash, role, student_id FROM auth_users WHERE username = :username");
        query.bindValue(":username", username);
    }

    if (!query.exec()) {
        QMessageBox::critical(this, "Database Error", "Failed to query the database: " + query.lastError().text());
        return;
    }


    if (!query.next()) {
        QMessageBox::warning(this, "Login", "Invalid username or password.");
        return;
    }

    // hash and role
    QString storedHash = query.value("password_hash").toString();
    QString role = query.value("role").toString();
    int studentId = query.value("student_id").toInt();

    // Debugging
    qDebug() << "Student ID:" << studentId;

    //provided password
    QString hashedInputPassword = QCryptographicHash::hash(userPassword.toUtf8(), QCryptographicHash::Sha256).toHex();

    // Verify
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
