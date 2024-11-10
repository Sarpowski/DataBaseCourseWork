#include "mainapplication.h"
#include "mainwindow.h"

#include <QApplication>



#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>


void connectToDatabase();


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //Login Screen
    //MainWindow w;
    //w.show();

    //Application Screen
    mainApplication *mainWindow = new mainApplication;
    mainWindow->show();

    //DataBase Test
    connectToDatabase();
    return a.exec();
}
void connectToDatabase() {
    // Create a database object

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

}
