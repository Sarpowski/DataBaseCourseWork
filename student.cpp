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
    qDebug() << "Active database connections:";
    for (const QString &connName : QSqlDatabase::connectionNames()) {
        qDebug() << connName;
    }
    SloadMarks();
}

student::~student()
{
    delete ui;
}

void student::SloadMarks() {

    int userId = getUserId();//mainWindow_->getUserId();
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

int student::getUserId() const
{
    int userId = mainWindow_->getUserId();
    return userId;
}


void student::studentExportPdf()
{

    modeldb& db = modeldb::getInstance();
    QSqlDatabase database = db.getDatabase();



    if (!database.isOpen()) {
        QMessageBox::critical(this, "Database Error", "Database connection is not open.");
        return;
    }
    int studentId = getUserId();

    QSqlQuery query(db.getDatabase());
    query.prepare(
        "SELECT p.first_name || ' ' || p.last_name AS Name, "
        "p.father_name AS FatherName, "
        "COALESCE(g.name, 'N/A') AS GroupName, "
        "s.name AS Subject, "
        "m.value AS Mark "
        "FROM people p "
        "LEFT JOIN groups g ON p.group_id = g.id "
        "LEFT JOIN marks m ON m.student_id = p.id "
        "LEFT JOIN subjects s ON m.subject_id = s.id "
        "WHERE p.type = 'S' AND p.id = :studentId " // Filter for the specific student
        "ORDER BY s.name"
        );
    query.bindValue(":studentId", studentId);

    if (!query.exec()) {
        QMessageBox::warning(this, "Database Error", "Failed to fetch data: " + query.lastError().text());
        return;
    }

    QString fileName = QFileDialog::getSaveFileName(this, "Save PDF File", "", "PDF Files (*.pdf)");
    if (fileName.isEmpty()) {
        QMessageBox::information(this, "Cancelled", "Export cancelled.");
        return;
    }

    QPdfWriter pdfWriter(fileName);
    pdfWriter.setPageSize(QPageSize(QPageSize::A4));
    pdfWriter.setResolution(300);

    QPainter painter(&pdfWriter);
    QFont font;
    font.setPointSize(10);
    painter.setFont(font);

    int x = 200;
    int y = 200;
    int lineHeight = 100;

    // Step 4: Write headers
    painter.drawText(x, y, "Name");
    painter.drawText(x + 400, y, "Father Name");
    painter.drawText(x + 800, y, "Group");
    painter.drawText(x + 1200, y, "Subject");
    painter.drawText(x + 1600, y, "Mark");
    y += lineHeight;

    QString currentStudent;
    while (query.next()) {
        QString name = query.value("Name").toString();
        QString fatherName = query.value("FatherName").toString();
        QString group = query.value("GroupName").toString();
        QString subject = query.value("Subject").toString();
        QString mark = query.value("Mark").toString();

        if (name != currentStudent) {
            currentStudent = name;
            y += lineHeight; // Add spacing
            painter.drawText(x, y, name);
            painter.drawText(x + 400, y, fatherName);
            painter.drawText(x + 800, y, group);
        }

        // Write subject and mark
        painter.drawText(x + 1200, y, subject.isEmpty() ? "N/A" : subject);
        painter.drawText(x + 1600, y, mark.isEmpty() ? "N/A" : mark);
        y += lineHeight;

        // Add a new page if necessary
        if (y > pdfWriter.height() - 100) {
            pdfWriter.newPage();
            y = 200;
        }
    }

    painter.end();
    QMessageBox::information(this, "Success", "Data exported to PDF successfully.");

}

void student::studentExportCsv()
{

    modeldb& db = modeldb::getInstance();
    QSqlDatabase database = db.getDatabase();

    if (!database.isOpen()) {
        QMessageBox::critical(this, "Database Error", "Database connection is not open.");
        return;
    }
    int studentId = getUserId();

    QSqlQuery query(db.getDatabase());
    query.prepare(
        "SELECT p.first_name || ' ' || p.last_name AS Name, "
        "p.father_name AS FatherName, "
        "COALESCE(g.name, 'N/A') AS GroupName, "
        "s.name AS Subject, "
        "m.value AS Mark "
        "FROM people p "
        "LEFT JOIN groups g ON p.group_id = g.id "
        "LEFT JOIN marks m ON m.student_id = p.id "
        "LEFT JOIN subjects s ON m.subject_id = s.id "
        "WHERE p.type = 'S' AND p.id = :studentId "
        "ORDER BY s.name"
        );
    query.bindValue(":studentId", studentId);

    if (!query.exec()) {
        QMessageBox::warning(this, "Database Error", "Failed to fetch data: " + query.lastError().text());
        return;
    }

    //CSV export
    QString fileName = QFileDialog::getSaveFileName(this, "Save CSV File", "", "CSV Files (*.csv)");
    if (fileName.isEmpty()) {
        QMessageBox::information(this, "Cancelled", "Export cancelled.");
        return;
    }

    //CSV file
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "File Error", "Failed to create the CSV file.");
        return;
    }

    QTextStream out(&file);

    out << "Name,Father Name,Group,Subject,Mark\n";

    QString currentStudent;
    while (query.next()) {
        QString name = query.value("Name").toString();
        QString fatherName = query.value("FatherName").toString();
        QString group = query.value("GroupName").toString();
        QString subject = query.value("Subject").toString();
        QString mark = query.value("Mark").toString();

        if (name != currentStudent) {
            currentStudent = name;
            out << "\"" << name << "\","
                << "\"" << fatherName << "\","
                << "\"" << group << "\",";
        } else {
            out << ",,,";
        }

        out << "\"" << (subject.isEmpty() ? "N/A" : subject) << "\","
            << "\"" << (mark.isEmpty() ? "N/A" : mark) << "\"\n";
    }

    file.close();
    QMessageBox::information(this, "Success", "Data exported to CSV successfully.");

}



void student::on_pushButton_StudentExportMarks_clicked()
{
    QStringList itemList({"pdf", "csv"});
    CustomDialog dialog(itemList);
    if (dialog.exec() == QDialog::Accepted)
    {
        qDebug() << dialog.comboBox()->currentText();
    }
    QString text = dialog.comboBox()->currentText();
    if(text == "pdf"){
        studentExportPdf();

    }
    else if(text == "csv"){
        studentExportCsv();
    }
}


void student::on_pushButtonStudentExit_clicked()
{
    this->close();

    MainWindow* loginScreen = new MainWindow();
    loginScreen->show();
}

