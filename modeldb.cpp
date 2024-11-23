#include "modeldb.h"



modeldb &modeldb::getInstance()
{
    static modeldb instance;
    return instance;
}

void modeldb::initDb()
{
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
    // QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL");
    // db.setHostName("localhost");
    // db.setPort(5432);
    // db.setDatabaseName("sarp"); // Ensure this matches the database you created
    // db.setUserName("sarp"); // Your PostgreSQL username
    // db.setPassword("password"); // Your PostgreSQL password

    if (QSqlDatabase::contains("qt_sql_default_connection")) {
        db = QSqlDatabase::database("qt_sql_default_connection");
    } else {
        db = QSqlDatabase::addDatabase("QPSQL");
        db.setHostName("localhost");
        db.setPort(5432);
        db.setDatabaseName("sarp"); // Ensure this matches your database name
        db.setUserName("sarp"); // PostgreSQL username
        db.setPassword("password"); // PostgreSQL password
    }
}
