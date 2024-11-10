#include "mainapplication.h"
#include "ui_mainapplication.h"

#include <QSqlTableModel>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

mainApplication::mainApplication(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::mainApplication)
{
    ui->setupUi(this);


    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL");
    db.setHostName("localhost");
    db.setPort(5432);
    db.setDatabaseName("sarp"); // Ensure this matches the database you created
    db.setUserName("sarp"); // Your PostgreSQL username
    db.setPassword("password"); // Your PostgreSQL password

    if (!db.open()) {
        qDebug() << "Error: Unable to connect to the database!" << db.lastError().text();
    } else {
        qDebug() << "Database connection successful!";
    }

    // Modeli ayarla
    QSqlTableModel *model = new QSqlTableModel(this, db);
    model->setTable("people"); // Veritabanındaki tablo adını kullan
    model->select(); // Verileri modele çek

    // İsteğe bağlı: Sütun başlıklarını değiştirebilirsiniz
    model->setHeaderData(0, Qt::Horizontal, "Student id");
    model->setHeaderData(1, Qt::Horizontal, "Name");
    model->setHeaderData(2, Qt::Horizontal, "Surname");
    model->setHeaderData(3, Qt::Horizontal, "Patronymic name");
    model->setHeaderData(4, Qt::Horizontal, "Groupd Id");
    model->setHeaderData(5, Qt::Horizontal, "Type S/P");

    // QTableView ile modeli bağla

    ui->MainAppTableView->setModel(model);
    ui->MainAppTableView->resizeColumnsToContents();
    if (!model->select()) {
        qDebug() << "Error loading table data:" << model->lastError().text();
    }


}

mainApplication::~mainApplication()
{
    delete ui;
}

void mainApplication::on_pushButtonLoadTable_clicked()
{

}

