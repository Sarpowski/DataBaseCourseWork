#include "student.h"
#include "ui_student.h"
#include "modeldb.h"
#include "mainwindow.h"


#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QRegularExpression>
#include <QComboBox>
#include <QCheckBox>
#include <qmessagebox.h>
#include <QPdfWriter>
#include <QFile>
#include <QPainter>
#include <QStyledItemDelegate>
#include <qstandarditemmodel.h>



student::student(MainWindow *mainWindow, QWidget *parent)
    : QWidget(parent)
    , mainWindow_(mainWindow)
    , ui(new Ui::student)
{
    ui->setupUi(this);
    SloadMarks();
}

student::~student()
{
    delete ui;
}

void student::SloadMarks() {

    int userId = mainWindow_->getUserId();
    qDebug() << "User ID at StudentHCPP: " << userId;
    QSqlQuery query;
    query.prepare(
        "SELECT s.name AS subject, m.value AS grade "
        "FROM marks m "
        "JOIN subjects s ON m.subject_id = s.id "
        "WHERE m.student_id = :userId"
        );
    query.bindValue(":userId", userId);

    if (!query.exec()) {
        qDebug() << "Database query failed:" << query.lastError().text();
        return;
    }

    QStandardItemModel *model = new QStandardItemModel(this);
    model->setHorizontalHeaderLabels({"Subject", "Grade"});

    while (query.next()) {
        QString subject = query.value("subject").toString();
        int grade = query.value("grade").toInt();

        QList<QStandardItem *> row;
        row.append(new QStandardItem(subject));
        row.append(new QStandardItem(QString::number(grade)));
        model->appendRow(row);
    }

    ui->tableViewStudent->setModel(model);
    ui->tableViewStudent->resizeColumnsToContents();
}


void student::studentExportPdf(QString& studentId) const
{
//TODO
/*
    1. Retrieve the `studentId` from the AUTH database, which stores user authentication details.
    2. Use this `studentId` to query the second database (DB_HEAVEN) where the marks, subjects,
       groups, and other related data are stored.
    3. Fetch the student's marks and subjects from the DB_HEAVEN database.
    4. Format the fetched data for display in a PDF document, including:
       - Student name, group, and subject names.
       - Marks for each subject, possibly formatted as a table.
    5. Use QPdfWriter to create a PDF file and write the formatted data to the document.

*/
}

void student::on_pushButton_StudentExportMarks_clicked()
{
    QStringList itemList({"pdf", "format2", "format3"});
    CustomDialog dialog(itemList);
    if (dialog.exec() == QDialog::Accepted)
    {
        // take proper action here
        qDebug() << dialog.comboBox()->currentText();
    }
    QString text = dialog.comboBox()->currentText();
    studentExportPdf(text);
}

