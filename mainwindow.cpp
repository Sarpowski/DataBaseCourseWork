#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QVBoxLayout>


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

}
