#ifndef MODELDB_H
#define MODELDB_H



#include <QSqlTableModel>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <memory>



class modeldb
{
public:
    static modeldb& getInstance();
    void initDb();
    bool executeQuery(const QString& queryStr);
    QSqlDatabase getDatabase() const;

private:
    modeldb();
    modeldb(const modeldb&) = delete;
    modeldb& operator=(const modeldb&) = delete;

    QSqlDatabase db;

};

#endif // MODELDB_H
