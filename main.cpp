#include "mainapplication.h"
#include "mainwindow.h"

#include <QApplication>
#include "modeldb.h"


#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>


void connectToDatabase();


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    modeldb &dbInstance = modeldb::getInstance();
    dbInstance.initDb();

    //Login Screen
    MainWindow w;
    w.show();

    //Application Screen
   // mainApplication *mainWindow = new mainApplication;
   // mainWindow->show();

    //DataBase Test
  // connectToDatabase();
    return a.exec();
}
void connectToDatabase() {

    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL");
    db.setHostName("localhost");
    db.setPort(5432);
    db.setDatabaseName("sarp");
    db.setUserName("sarp");
    db.setPassword("password");

    if (!db.open()) {
        qDebug() << "Error: Unable to connect to the database!" << db.lastError().text();
    } else {
        qDebug() << "Database connection successful!";
    }

}
