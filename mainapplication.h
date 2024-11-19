#ifndef MAINAPPLICATION_H
#define MAINAPPLICATION_H

#include "modeldb.h"

#include <QWidget>




namespace Ui {
class mainApplication;
}

class mainApplication : public QWidget
{
    Q_OBJECT

public:
    explicit mainApplication(QWidget *parent = nullptr);
    ~mainApplication();
    void initMainAppTableView(modeldb& db);
    void checkBoxFilter();
    void groupBoxGroupSelect();

    void loadGroups();
    void loadStudentNames();
    void addStudent();
    void editStudent();
    void loadTeacherName();




private slots:


    void on_pushButtonLoadTable_clicked();

    void on_pushButton_StudentDelete_clicked();

    void pushButton_AddGroup();

    void on_pushButton_TESTadd_clicked();

    void on_pushButton_DeleteGroup_clicked();

    void on_pushButton_AddTeacher_clicked();

    void on_pushButton_DeleteTeacher_clicked();

    void on_pushButton_EditTeacher_clicked();

private:
    bool firstRun_ ;
    Ui::mainApplication *ui;
     QSqlDatabase db;
};

#endif // MAINAPPLICATION_H
