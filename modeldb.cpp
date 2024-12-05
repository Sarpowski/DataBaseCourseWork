#include "modeldb.h"




modeldb &modeldb::getInstance()
{
    static modeldb instance;
    return instance;
}





void modeldb::initDb(const QString& configFilePath)
{

    QString targetFilePath = "C:\\Users\\Can\\Desktop\\dataBaseCourseWork\\CourseWorkDB\\build\\Desktop_Qt_6_8_0_MinGW_64_bit-Debug\\main.ini"; ;

    QFile configFile(targetFilePath);
    if (!configFile.exists()) {
        qDebug() << "Error: Configuration file not found:" << configFilePath;
        return;
    }


    QSettings settings(configFilePath, QSettings::IniFormat);
    QString host = settings.value("database/host", "localhost").toString();
    int port = settings.value("database/port", 5432).toInt();
    QString dbName = settings.value("database/name", "default_db").toString();
    QString username = settings.value("database/username", "user").toString();
    QString password = settings.value("database/password", "password").toString();

    qDebug() << "Loaded database configuration:";
    qDebug() << "Host:" << host;
    qDebug() << "Port:" << port;
    qDebug() << "Database:" << dbName;
    qDebug() << "Username:" << username;

    // Set up the database connection
    if (QSqlDatabase::contains("qt_sql_default_connection")) {
        db = QSqlDatabase::database("qt_sql_default_connection");
    } else {
        db = QSqlDatabase::addDatabase("QPSQL"); // PostgreSQL driver
    }

    db.setHostName(host);
    db.setPort(port);
    db.setDatabaseName(dbName);
    db.setUserName(username);
    db.setPassword(password);



    // Open the database
    if (!db.open()) {
        qDebug() << "Error: Unable to connect to the database!" << db.lastError().text();
    } else {
        qDebug() << "Database connection successful!";
    }
}

bool modeldb::executeQuery(const QString &queryStr)
{
    QSqlQuery query(db);
    if (!query.exec(queryStr)) {
        qDebug() << "SQL Error:" << query.lastError().text();
        return false;
    }
    return true;
}

QSqlDatabase modeldb::getDatabase()
{
    return db;
}

modeldb::modeldb()
{
    // Constructor does nothing; initDb must be called explicitly
}

