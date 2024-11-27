#ifndef STUDENT_H
#define STUDENT_H

#include <QWidget>
#include <QComboBox>
#include "customdialog.h"
#include "mainwindow.h"
namespace Ui {
class student;
}

class student : public QWidget
{
    Q_OBJECT

public:
    explicit student(MainWindow* mainWindow, QWidget *parent = nullptr);

    ~student();
    void SloadMarks();
    int getUserId() const;
    void studentExportPdf();
    void studentExportCsv();
private slots:
    void on_pushButton_StudentExportMarks_clicked();

    void on_pushButtonStudentExit_clicked();

private:
    Ui::student *ui;
    MainWindow* mainWindow_;
};

#endif // STUDENT_H
