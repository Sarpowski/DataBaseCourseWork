#ifndef MODELDB_H
#define MODELDB_H




#include <QSqlTableModel>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QFile>
#include <QSettings>
#include <memory>



class modeldb
{
public:
    static modeldb& getInstance();
    void initDb(const QString& configFilePath);
    bool executeQuery(const QString& queryStr);
    QSqlDatabase getDatabase();
private:
    modeldb();
    modeldb(const modeldb&) = delete;
    modeldb& operator=(const modeldb&) = delete;

    QSqlDatabase db;
};

#endif // MODELDB_H
