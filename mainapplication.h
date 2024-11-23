#ifndef MAINAPPLICATION_H
#define MAINAPPLICATION_H

#include "modeldb.h"

#include <QWidget>
#include <qcombobox.h>
#include <QCryptographicHash>




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
    void loadStudentNamesComboBox(QComboBox* comboBox);
    void addStudent();
    void editStudent();
    void loadTeacherName();
    void loadSubject();
    void loadGrades();
    void loadSubjectsWithTeachers();
    void exportToCsv();
    void exportToPdf();
    void exportData();




    // Utility function to hash a password using SHA-256
    QString hashPassword(const QString &plainPassword) {
        return QCryptographicHash::hash(plainPassword.toUtf8(), QCryptographicHash::Sha256).toHex();
    }
    void saveOrUpdateMark(int studentId, int subjectId, int teacherId, int mark);
    void loadComboBoxSubjects(QComboBox* comboBox, int studentId);
    void loadComboBoxMarks(QComboBox* comboBox);
    void loadComboBoxStudents(QComboBox* comboBox, int groupId);
    void loadComboBoxStudents(QComboBox* comboBox);
    void loadComboBoxGroupForMark(QComboBox* comboBox, const QString& placeholderText = "Select a Group");

    void loadComboBoxSubjectsForMark();
private slots:


    void on_pushButtonLoadTable_clicked();

    void on_pushButton_StudentDelete_clicked();

    void pushButton_AddGroup();

    void on_pushButton_TESTadd_clicked();

    void on_pushButton_DeleteGroup_clicked();

    void on_pushButton_AddTeacher_clicked();

    void on_pushButton_DeleteTeacher_clicked();

    void on_pushButton_EditTeacher_clicked();

    void on_pushButton_SubjectAdd_clicked();

    void on_pushButton_SubjectDelete_clicked();

    void on_pushButton_SubjectEdit_clicked();

    void on_pushButton_AssignTeacher_clicked();

    void on_pushButton_AssignGroup_clicked();


    void on_pushButton_ViewGrades_clicked();

    void on_pushButton_ExportData_clicked();

    void on_pushButton_MarkView_clicked();



    void on_pushButton_SaveMark_clicked();

    void on_tabWidget_2_currentChanged(int index);

    void on_comboBox_EditMarkSelectGroup_currentIndexChanged(int index);

    void on_comboBox_EditMarkSelectStudent_currentIndexChanged(int index);

    void on_comboBox_EditMarkChooseSubject_2_currentIndexChanged(int index);

    void on_MA_tabWidget_Main_tabBarClicked(int index);

    void on_pushButton_AddLogin_clicked();

private:
    bool firstRun_ ;
    Ui::mainApplication *ui;
     QSqlDatabase db;
};

#endif // MAINAPPLICATION_H
