#include "mainapplication.h"
#include "mainwindow.h"

#include <QApplication>
#include "modeldb.h"


#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <qfiledialog.h>


void connectToDatabase();
QString init()

{
    QString targetFilePath = "C:\\Users\\Can\\Desktop\\dataBaseCourseWork\\CourseWorkDB\\build\\Desktop_Qt_6_8_0_MinGW_64_bit-Debug\\main.ini"; ;
    return targetFilePath;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QString configFilePath = init();


    // Initialize the database with the selected configuration file
    modeldb& dbInstance = modeldb::getInstance();
    dbInstance.initDb(configFilePath);



   // modeldb &dbInstance = modeldb::getInstance();


    //Login Screen
   // MainWindow w;
    //w.show();

    //Application Screen
    mainApplication *mainWindow = new mainApplication;
    mainWindow->show();

    //DataBase Test
    connectToDatabase();

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
