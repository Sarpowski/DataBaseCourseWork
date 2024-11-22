#ifndef STUDENT_H
#define STUDENT_H

#include <QWidget>
#include <QComboBox>
#include "customdialog.h"

namespace Ui {
class student;
}

class student : public QWidget
{
    Q_OBJECT

public:
    explicit student(QWidget *parent = nullptr);

    ~student();

    void studentExportPdf(QString& text) const;
private slots:
    void on_pushButton_StudentExportMarks_clicked();

private:
    Ui::student *ui;
};

#endif // STUDENT_H
