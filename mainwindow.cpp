#include "mainwindow.h"
#include "./ui_mainwindow.h"
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
    ui->groupBoxLogin->setMinimumSize(300, 300); // Adjust values as needed
    // Create a layout for the main window and set it to the central widget
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(ui->groupBoxLogin); // Use ui->groupBoxLogin to access the group box
    layout->setAlignment(ui->groupBoxLogin, Qt::AlignCenter);

    // Set the layout to the central widget
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
void MainWindow::on_pushButtonLogin_clicked()
{
    QString username = ui->lineEditUsername->text();
    QString userPassword = ui->lineEditPassword->text();

    if(username == "admin" && userPassword =="admin" ){
         QMessageBox::information(this, "Login","Username and password is correct");
    }
    else{
         QMessageBox::warning(this,"Login","Username or password in correct");
    }

}

