#include "mainapplication.h"
#include "ui_mainapplication.h"
#include <QTimer>


#include <QSqlTableModel>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QStandardItemModel>

#include<QFileDialog>
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
#include "modeldb.h"

#include <QCryptographicHash>



mainApplication::mainApplication(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::mainApplication)

{
    ui->setupUi(this);


    firstRun_ = true;

    modeldb& db = modeldb::getInstance();


    initMainAppTableView(db);
    loadSubjectsWithTeachers();
    groupBoxGroupSelect();
    checkBoxFilter();

    loadStudentNamesComboBox(ui->MA_comboBox_EditStudent);
    loadTeacherName();
    loadSubject();
    ui->comboBox_Subject->setMinimumSize(200, 30);


    loadComboBoxGroupForMark(ui->comboBox_MarkSelectGroup);
    loadComboBoxSubjectsForMark();

    //Connections
    connect(ui->pushButton_AddGroup,&QPushButton::clicked,this,&mainApplication::pushButton_AddGroup);
    connect(ui->MA_pushButton, &QPushButton::clicked, this, &mainApplication::on_pushButtonLoadTable_clicked);
    connect(ui->MA_checkBox_Student, &QCheckBox::checkStateChanged, this, &mainApplication::checkBoxFilter);
    connect(ui->MA_checkBox_Teacher, &QCheckBox::checkStateChanged, this, &mainApplication::checkBoxFilter);
    connect(ui->pushButtonStudentEdit, &QPushButton::clicked, this, &mainApplication::editStudent);

    // In mainApplication constructor or setupUi
    connect(ui->pushButton_2, &QPushButton::clicked,
            this, &mainApplication::on_pushButtonLoadTable_clicked);
}



mainApplication::~mainApplication()
{
    delete ui;
}

void mainApplication::initMainAppTableView(modeldb& db)
{
    QSqlTableModel *model = new QSqlTableModel(this, db.getDatabase());
    model->setTable("people");
    model->select();

    model->setHeaderData(0, Qt::Horizontal, "Student id");
    model->setHeaderData(1, Qt::Horizontal, "Name");
    model->setHeaderData(2, Qt::Horizontal, "Surname");
    model->setHeaderData(3, Qt::Horizontal, "Patronymic name");
    model->setHeaderData(4, Qt::Horizontal, "Groupd Id");
    model->setHeaderData(5, Qt::Horizontal, "Type S/P");


    ui->MainAppTableView->setModel(model);
    ui->MainAppTableView->resizeColumnsToContents();
    if (!model->select()) {
        qDebug() << "Error loading table data:" << model->lastError().text();
    }
}

void mainApplication::on_pushButtonLoadTable_clicked()
{

    QString selectedGroup = ui->MA_comboBox_group->currentText();

    if (selectedGroup == "Select a group") {
        QSqlTableModel *model = static_cast<QSqlTableModel *>(ui->MainAppTableView->model());
        if (model) {
            model->setFilter(""); // Show all data
            model->select();
        }
        return;
    }

    QSqlTableModel *model = static_cast<QSqlTableModel *>(ui->MainAppTableView->model());

    if (model) {
        model->setFilter(QString("group_id = (SELECT id FROM groups WHERE name = '%1')").arg(selectedGroup));

        if (!model->select()) {
            qDebug() << "Error filtering table data:" << model->lastError().text();
        }
    } else {
        qDebug() << "Model is not properly initialized.";
    }



}

void mainApplication::checkBoxFilter()
{
    QSqlTableModel *model = static_cast<QSqlTableModel *>(ui->MainAppTableView->model());

    if (!model) {
        qDebug() << "Model is not properly initialized.";
        return;
    }

    QStringList filterParts;

    QString selectedGroup = ui->MA_comboBox_group->currentText();
    if (selectedGroup != "Select a group") {
        filterParts << QString("group_id = (SELECT id FROM groups WHERE name = '%1')").arg(selectedGroup);
    }

    // Checkbox filtering by type (Student or Teacher)
    bool isStudentChecked = ui->MA_checkBox_Student->isChecked();
    bool isTeacherChecked = ui->MA_checkBox_Teacher->isChecked();


    if (isStudentChecked && !isTeacherChecked) {
        filterParts << "type = 'S'";
    } else if (!isStudentChecked && isTeacherChecked) {
        filterParts << "type = 'P'";
    } else if (!isStudentChecked && !isTeacherChecked) {
        filterParts << "1 = 0";
    }

    // Combine all filter parts
    QString finalFilter = filterParts.join(" AND ");

    // Apply the filter
    model->setFilter(finalFilter);
    if (!model->select()) {
        qDebug() << "Error applying filter:" << model->lastError().text();
    }
}

void mainApplication::groupBoxGroupSelect()
{

    modeldb& db = modeldb::getInstance();

    ui->MA_comboBox_SelectGroup->clear();
    ui->MA_comboBox_group->clear();

    ui->MA_comboBox_SelectGroup->addItem("Select a group");
    ui->MA_comboBox_group->addItem("Select a group");

    QSqlQuery queryGroup(db.getDatabase());

    if (queryGroup.exec("SELECT name FROM groups")) {
        while (queryGroup.next()) {
            QString groupName = queryGroup.value(0).toString();
            ui->MA_comboBox_group->addItem(groupName);
            ui->MA_comboBox_SelectGroup->addItem(groupName);
        }
    } else {
        qDebug() << "Failed to retrieve group names:" << queryGroup.lastError().text();
    }
}



void mainApplication::loadGroups()
{
    QSqlQuery query(db);
    ui->MA_comboBox_EditStudent->clear();
    if (query.exec("SELECT name FROM groups")) {
        while (query.next()) {
            ui->MA_comboBox_EditStudent->addItem(query.value(0).toString());
        }
    } else {
        qDebug() << "Failed to load groups:" << query.lastError().text();
    }
}









//DOPE
void mainApplication::on_pushButton_TESTadd_clicked()
{
    addStudent();

}

//fix this shit
void mainApplication::addStudent() {



    qDebug() << "addStudent called";
    QString name = ui->MA_lineEdit_EditName->text();
    QString surname = ui->MA_lineEdit_EditSurname->text();
    QString fatherName = ui->MA_lineEdit_EditFatherName->text();
    QString groupName = ui->MA_comboBox_SelectGroup->currentText();

    if (groupName == "Select a group" || name.isEmpty() || surname.isEmpty() || fatherName.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Please fill all fields and select a valid group.");
        return;
    }

    modeldb& db = modeldb::getInstance();

    QSqlQuery groupQuery(db.getDatabase());
    groupQuery.prepare("SELECT id FROM groups WHERE name = :group_name");
    groupQuery.bindValue(":group_name", groupName);

    if (!groupQuery.exec() || !groupQuery.next()) {
        qDebug() << "Error retrieving group ID:" << groupQuery.lastError().text();
        return;
    }

    int groupId = groupQuery.value(0).toInt();
    qDebug() << "Group ID for selected group:" << groupId;

    QSqlQuery query(db.getDatabase());
    query.prepare("INSERT INTO people (first_name, last_name, father_name, group_id, type) "
                  "VALUES (:name, :surname, :father_name, :group_id, 'S')");
    query.bindValue(":name", name);
    query.bindValue(":surname", surname);
    query.bindValue(":father_name", fatherName);
    query.bindValue(":group_id", groupId);

    if (!query.exec()) {
        QMessageBox::information(this, "Error", "Error adding student.");
        qDebug() << "Error adding student:" << query.lastError().text();
    } else {
        QMessageBox::information(this, "Success", "Student added successfully.");
    }

    groupBoxGroupSelect();

}



void mainApplication::editStudent() {
    modeldb& db = modeldb::getInstance();

    QString studentId = ui->MA_comboBox_EditStudent->currentData().toString();
    if (studentId.isEmpty() || studentId == "Select a student"  ) {
        QMessageBox::warning(this, "Input Error", "Please select a valid student EDIT part.");
        return;
    }

    QString groupName = ui->MA_comboBox_SelectGroup->currentText();
    int groupId = 0;

    QSqlQuery groupQuery(db.getDatabase());
    groupQuery.prepare("SELECT id FROM groups WHERE name = :group_name");
    groupQuery.bindValue(":group_name", groupName);

    if (groupQuery.exec() && groupQuery.next()) {
        groupId = groupQuery.value(0).toInt();
    } else {
        QMessageBox::warning(this, "Database Error", "Failed to find the selected group: " + groupQuery.lastError().text());
        return;
    }

    QString firstName = ui->MA_lineEdit_EditName->text().trimmed();
    QString lastName = ui->MA_lineEdit_EditSurname->text().trimmed();
    QString fatherName = ui->MA_lineEdit_EditFatherName->text().trimmed();

    if (firstName.isEmpty() || lastName.isEmpty() || fatherName.isEmpty() || groupId == 0) {
        QMessageBox::warning(this, "Input Error", "All fields must be filled out.");
        return;
    }

    // Update the database
    QSqlQuery query(db.getDatabase());
    query.prepare("UPDATE people SET first_name = :first_name, last_name = :last_name, "
                  "father_name = :father_name, group_id = :group_id WHERE id = :student_id");
    query.bindValue(":first_name", firstName);
    query.bindValue(":last_name", lastName);
    query.bindValue(":father_name", fatherName);
    query.bindValue(":group_id", groupId);
    query.bindValue(":student_id", studentId);

    if (query.exec()) {
        QMessageBox::information(this, "Success", "Student details updated successfully.");

        loadStudentNamesComboBox(ui->MA_comboBox_EditStudent);

        for (int i = 0; i < ui->MA_comboBox_EditStudent->count(); ++i) {
            if (ui->MA_comboBox_EditStudent->itemData(i).toString() == studentId) {
                ui->MA_comboBox_EditStudent->setCurrentIndex(i);
                break;
            }
        }
    } else {
        QMessageBox::warning(this, "Database Error", "Failed to update student details: " + query.lastError().text());
    }

    // Clear the input fields
    ui->MA_lineEdit_EditName->clear();
    ui->MA_lineEdit_EditSurname->clear();
    ui->MA_lineEdit_EditFatherName->clear();
    ui->MA_comboBox_SelectGroup->setCurrentText("Select a group");
    firstRun_=false;
}

void mainApplication::loadTeacherName()
{
    modeldb& db = modeldb::getInstance();

    ui->comboBox_TeacherSelect->clear();

    ui->comboBox_TeacherSelect->addItem("Select a Teacher");

    QSqlQuery query(db.getDatabase());
    query.prepare("SELECT id, first_name, last_name FROM people WHERE type = 'P' ORDER BY first_name");

    if (query.exec()) {
        while (query.next()) {
            QString studentId = query.value(0).toString();
            QString studentName = query.value(1).toString() + " " + query.value(2).toString();
            // Add student ID and name to the combo box
            ui->comboBox_TeacherSelect->addItem(studentName, studentId);
        }
    } else {
        QMessageBox::warning(this, "Database Error", "Failed to load student names: " + query.lastError().text());
    }
}

void mainApplication::loadSubject()
{
    modeldb& db = modeldb::getInstance();

    ui->comboBox_Subject->clear();

    ui->comboBox_Subject->addItem("Select a Subject");

    QSqlQuery query(db.getDatabase());
    query.prepare("SELECT id, name FROM subjects;");

    if (query.exec()) {
        while (query.next()) {
            QString subjectId = query.value(0).toString();  // ID of the subject
            QString subjectName = query.value(1).toString(); // Name of the subject
            ui->comboBox_Subject->addItem(subjectName, subjectId);
        }
    } else {
        QMessageBox::warning(this, "Database Error", "Failed to load subjects: " + query.lastError().text());
    }
}
void mainApplication::loadGrades()
{
    //bug fix maintance happens in here
    modeldb& db = modeldb::getInstance();

    QSqlQueryModel *model = new QSqlQueryModel(this);

    QSqlQuery query(db.getDatabase());
    query.prepare("SELECT m.id AS Grade_ID, "
                  "       p.first_name || ' ' || p.last_name AS Student, "
                  "       g.name AS Group_Name, "
                  "       s.name AS Subject, "
                  "       COALESCE(m.value::TEXT, 'No Grade') AS Grade, "
                  "       TO_CHAR(m.created_at, 'YYYY-MM-DD HH24:MI') AS Assigned_At "
                  "FROM marks m "
                  "LEFT JOIN people p ON m.student_id = p.id "
                  "LEFT JOIN groups g ON p.group_id = g.id "
                  "LEFT JOIN subjects s ON m.subject_id = s.id "
                  "ORDER BY m.created_at DESC");

    // Execute the query
    if (!query.exec()) {
        QMessageBox::warning(this, "Database Error", "Failed to load grades: " + query.lastError().text());
        return;
    }

    //model->setQuery(std::move(query));
    // Use std::move to transfer ownership of the query
    model->setQuery(std::move(query));

    // Check for query execution errors after setQuery
    if (model->lastError().isValid()) {
        qDebug() << "Query execution error:" << model->lastError().text();
        return;
    }
    model->setHeaderData(0, Qt::Horizontal, "Grade ID");
    model->setHeaderData(1, Qt::Horizontal, "Student");
    model->setHeaderData(2, Qt::Horizontal, "Group Name");
    model->setHeaderData(3, Qt::Horizontal, "Subject");
    model->setHeaderData(4, Qt::Horizontal, "Grade");
    model->setHeaderData(5, Qt::Horizontal, "Assigned At");

    ui->tableViewSubject->setModel(model);

    ui->tableViewSubject->resizeColumnsToContents();

    ui->tableViewSubject->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    if (query.lastError().isValid()) {
        qDebug() << "Query execution error:" << query.lastError().text();
    }

}





void mainApplication::on_pushButton_StudentDelete_clicked()
{
    modeldb& db = modeldb::getInstance();

    QString firstName = ui->MA_lineEdit_EditName->text().trimmed();
    QString lastName = ui->MA_lineEdit_EditSurname->text().trimmed();
    QString fatherName = ui->MA_lineEdit_EditFatherName->text().trimmed();
    QString groupName = ui->MA_comboBox_SelectGroup->currentText().trimmed();

    if (firstName.isEmpty() || lastName.isEmpty() || fatherName.isEmpty() || groupName.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "All fields must be filled, and a valid group must be selected to delete a student.");
        return;
    }

    QSqlQuery groupQuery(db.getDatabase());
    groupQuery.prepare("SELECT id FROM groups WHERE name = :groupName");
    groupQuery.bindValue(":groupName", groupName);

    int groupId = 0;
    if (groupQuery.exec() && groupQuery.next()) {
        groupId = groupQuery.value(0).toInt();
    } else {
        qDebug() << "Failed to retrieve group id:" << groupQuery.lastError().text();
        QMessageBox::warning(this, "Error", "Invalid group selected.");
        return;
    }

    QSqlQuery checkQuery(db.getDatabase());
    checkQuery.prepare("SELECT 1 FROM people WHERE first_name = :firstName AND last_name = :lastName AND father_name = :fatherName AND group_id = :groupId");
    checkQuery.bindValue(":firstName", firstName);
    checkQuery.bindValue(":lastName", lastName);
    checkQuery.bindValue(":fatherName", fatherName);
    checkQuery.bindValue(":groupId", groupId);

    if (!checkQuery.exec() || !checkQuery.next()) {
        QMessageBox::warning(this, "Error", "Student not found.");
        return;
    }

    // Confirm deletion
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Delete Confirmation", "Are you sure you want to delete this student?",
                                  QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::No) {
        return;
    }

    QSqlQuery deleteQuery(db.getDatabase());
    deleteQuery.prepare("DELETE FROM people WHERE first_name = :firstName AND last_name = :lastName AND father_name = :fatherName AND group_id = :groupId");
    deleteQuery.bindValue(":firstName", firstName);
    deleteQuery.bindValue(":lastName", lastName);
    deleteQuery.bindValue(":fatherName", fatherName);
    deleteQuery.bindValue(":groupId", groupId);

    if (!deleteQuery.exec()) {
        qDebug() << "Failed to delete student:" << deleteQuery.lastError().text();
        QMessageBox::warning(this, "Database Error", "Failed to delete student.");
        return;
    }

    QMessageBox::information(this, "Success", "Student deleted successfully.");

    ui->MA_lineEdit_EditName->clear();
    ui->MA_lineEdit_EditSurname->clear();
    ui->MA_lineEdit_EditFatherName->clear();
    ui->MA_comboBox_SelectGroup->setCurrentIndex(-1);

    initMainAppTableView(db);
    groupBoxGroupSelect();

}


void mainApplication::on_pushButton_DeleteGroup_clicked()
{
    modeldb& db = modeldb::getInstance();


    // Prompt user to enter the group name
    bool ok;
    QString groupName = QInputDialog::getText(this, "Delete Group",
                                              "Enter the group name to delete:",
                                              QLineEdit::Normal, "", &ok);

    if (groupName.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "All fields must be filled, and a valid group must be selected to delete a student.");
        return;
    }

    // Retrieve group ID
    QSqlQuery groupQuery(db.getDatabase());
    groupQuery.prepare("SELECT id FROM groups WHERE name = :groupName");
    groupQuery.bindValue(":groupName", groupName);

    int groupId = 0;
    if (groupQuery.exec() && groupQuery.next()) {
        groupId = groupQuery.value(0).toInt();
    } else {
        qDebug() << "Failed to retrieve group id:" << groupQuery.lastError().text();
        QMessageBox::warning(this, "Error", "Invalid group selected.");
        return;
    }

    // Check if the student exists
    QSqlQuery checkQuery(db.getDatabase());
    checkQuery.prepare("SELECT delete_group_and_students(:group_name);");
    // Confirm deletion
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Delete Confirmation", "Are you sure you want to delete this group?",
                                  QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::No) {
        return;
    }
    checkQuery.bindValue(":group_name", groupName);

    if (!checkQuery.exec() || !checkQuery.next()) {
        QMessageBox::warning(this, "Error", "group not found.");
        return;
    }
    groupBoxGroupSelect();



    // Delete the student
    QSqlQuery deleteQuery(db.getDatabase());
    deleteQuery.prepare("SELECT delete_group_and_students(:group_name);");
    deleteQuery.bindValue(":group_name", groupName);
    if (!deleteQuery.exec()) {

        return;
    }


    QMessageBox::information(this, "Success", "Group deleted successfully.");
}




void mainApplication::pushButton_AddGroup()
{
    bool ok;
    QString groupName = QInputDialog::getText(this, tr("Add Group"),
                                              tr("Enter Group Name (Format: 3084/1_2024):"), QLineEdit::Normal,
                                              "", &ok);

    if (ok && !groupName.isEmpty()) {
        // Check if the group name matches the required format using regex
        QRegularExpression regex("^[0-9]{4}/[0-9]_[0-9]{4}$");
        if (!regex.match(groupName).hasMatch()) {
            QMessageBox::warning(this, tr("Invalid Format"), tr("Please enter the group name in the correct format (e.g., 3084/1_2024)."));
            return;
        }

        // Insert the group into the database using a prepared statement
        QSqlQuery query;
        query.prepare("INSERT INTO groups (name) VALUES (:groupName)");
        query.bindValue(":groupName", groupName);

        if (query.exec()) {
            QMessageBox::information(this, tr("Success"), tr("Group added successfully."));
        } else {
            qDebug() << "Failed to add group:" << query.lastError().text();
            QMessageBox::warning(this, tr("Error"), tr("Failed to add group: %1").arg(query.lastError().text()));
        }
    }
    groupBoxGroupSelect();

}

void mainApplication::loadStudentNamesComboBox(QComboBox* comboBox) {
    modeldb& db = modeldb::getInstance();


   comboBox->clear();

    // Add a default "Select a student" item
    comboBox->addItem("Select a student");

    // Query to fetch student names and IDs
    QSqlQuery query(db.getDatabase());
    query.prepare("SELECT id, first_name, last_name FROM people WHERE type = 'S' ORDER BY first_name");

    if (query.exec()) {
        while (query.next()) {
            QString studentId = query.value(0).toString();
            QString studentName = query.value(1).toString() + " " + query.value(2).toString();
            // Add student ID and name to the combo box
            comboBox->addItem(studentName, studentId);
        }
    } else {
        QMessageBox::warning(this, "Database Error", "Failed to load student names: " + query.lastError().text());
    }
}


void mainApplication::on_pushButton_AddTeacher_clicked()
{

    qDebug() << "addTeacher called";
    QString name = ui->lineEdit_Tname->text();
    QString surname = ui->lineEdit_Tsurname->text();
    QString fatherName = ui->lineEdit_TfatherName->text();


    if (name.isEmpty() || surname.isEmpty() || fatherName.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Please fill all fields.");
        return;
    }

    modeldb& db = modeldb::getInstance();
    QSqlQuery groupQuery(db.getDatabase());
    QSqlQuery query(db.getDatabase());

    query.prepare("INSERT INTO people (first_name, last_name, father_name, group_id, type) "
                  "VALUES (:name, :surname, :father_name, :group_id, 'P')");
    query.bindValue(":name", name);
    query.bindValue(":surname", surname);
    query.bindValue(":father_name", fatherName);
    query.bindValue(":type", 'P');

    if (!query.exec()) {
        QMessageBox::information(this, "Error", "Error adding teacher.");
        qDebug() << "Error adding student:" << query.lastError().text();
    } else {
        QMessageBox::information(this, "Success", "Teacher added successfully.");
    }


}


void mainApplication::on_pushButton_DeleteTeacher_clicked()
{
    modeldb& db = modeldb::getInstance();

    QString firstName = ui->lineEdit_Tname->text().trimmed();
    QString lastName = ui->lineEdit_Tsurname->text().trimmed();
    QString fatherName = ui->lineEdit_TfatherName->text().trimmed();

    if (firstName.isEmpty() || lastName.isEmpty() || fatherName.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "All fields must be filled.");
        return;
    }


    QSqlQuery checkQuery(db.getDatabase());
    checkQuery.prepare("SELECT 1 FROM people WHERE first_name = :firstName AND last_name = :lastName AND father_name = :fatherName");
    checkQuery.bindValue(":firstName", firstName);
    checkQuery.bindValue(":lastName", lastName);
    checkQuery.bindValue(":fatherName", fatherName);


    if (!checkQuery.exec() || !checkQuery.next()) {
        QMessageBox::warning(this, "Error", "Student not found.");
        return;
    }

    // Confirm deletion
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Delete Confirmation", "Are you sure you want to delete this teacher?",
                                  QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::No) {
        return;
    }

    QSqlQuery deleteQuery(db.getDatabase());
    deleteQuery.prepare("DELETE FROM people WHERE first_name = :firstName AND last_name = :lastName AND father_name = :fatherName");
    deleteQuery.bindValue(":firstName", firstName);
    deleteQuery.bindValue(":lastName", lastName);
    deleteQuery.bindValue(":fatherName", fatherName);

    if (!deleteQuery.exec()) {
        qDebug() << "Failed to delete teacher:" << deleteQuery.lastError().text();
        QMessageBox::warning(this, "Database Error", "Failed to delete teacher.");
        return;
    }

    QMessageBox::information(this, "Success", "teacher deleted successfully.");

    ui->lineEdit_Tname->clear();
    ui->lineEdit_Tsurname->clear();
    ui->lineEdit_TfatherName->clear();

    initMainAppTableView(db);
    //groupBoxGroupSelect();

}

void mainApplication::on_pushButton_EditTeacher_clicked()
{
    modeldb& db = modeldb::getInstance();

    QString teacherId = ui->comboBox_TeacherSelect->currentData().toString();
    if (teacherId.isEmpty() || teacherId == "Select a Teacher") {
        QMessageBox::warning(this, "Input Error", "Please select a valid teacher to edit.");
        return;
    }

    QString firstName = ui->lineEdit_Tname->text().trimmed();
    QString lastName = ui->lineEdit_Tsurname->text().trimmed();
    QString fatherName = ui->lineEdit_TfatherName->text().trimmed();

    if (firstName.isEmpty() || lastName.isEmpty() || fatherName.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "All fields must be filled out.");
        return;
    }

    QSqlQuery query(db.getDatabase());
    query.prepare("UPDATE people SET first_name = :first_name, last_name = :last_name, "
                  "father_name = :father_name WHERE id = :teacher_id AND type = 'P'");
    query.bindValue(":first_name", firstName);
    query.bindValue(":last_name", lastName);
    query.bindValue(":father_name", fatherName);
    query.bindValue(":teacher_id", teacherId);

    if (query.exec()) {
        QMessageBox::information(this, "Success", "Teacher details updated successfully.");

        loadTeacherName();

        for (int i = 0; i < ui->comboBox_TeacherSelect->count(); ++i) {
            if (ui->comboBox_TeacherSelect->itemData(i).toString() == teacherId) {
                ui->comboBox_TeacherSelect->setCurrentIndex(i);
                break;
            }
        }
    } else {
        QMessageBox::warning(this, "Database Error", "Failed to update teacher details: " + query.lastError().text());
    }

    // Clear the input fields
    ui->lineEdit_Tname->clear();
    ui->lineEdit_Tsurname->clear();
    ui->lineEdit_TfatherName->clear();
    ui->comboBox_TeacherSelect->setCurrentText("Select a Teacher");
}

void mainApplication::on_pushButton_SubjectAdd_clicked()
{
    bool ok;
    QString subject = QInputDialog::getText(this, "Add Subject",
                                              "Enter the subject name to enter:",
                                              QLineEdit::Normal, "", &ok);

    if (subject.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "All fields must be filled.");
        return;
    }

    modeldb& db = modeldb::getInstance();

    QSqlQuery query(db.getDatabase());
    query.prepare("INSERT INTO subjects (name) VALUES (:subjectName)");
    query.bindValue(":subjectName", subject);

    if (query.exec()) {
        QMessageBox::information(this, "Success", "Subject added successfully.");

        loadSubject();
        loadSubjectsWithTeachers();

    } else {
        QMessageBox::warning(this, "Database Error", "Failed to update Subject: " + query.lastError().text());
    }

}

void mainApplication::on_pushButton_SubjectDelete_clicked()
{
    int currentIndex = ui->comboBox_Subject->currentIndex();
    if (currentIndex <= 0) { // Ensure a valid subject is selected
        QMessageBox::warning(this, "Input Error", "Please select a valid subject to delete.");
        return;
    }

    QVariant subjectId = ui->comboBox_Subject->currentData();
    if (!subjectId.isValid()) {
        QMessageBox::warning(this, "Selection Error", "Could not retrieve the selected subject.");
        return;
    }

    // Confirm deletion
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Delete Confirmation",
                                  "Are you sure you want to delete this subject? All associated data will be removed.",
                                  QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::No) {
        return;
    }

    modeldb& db = modeldb::getInstance();
    QSqlQuery query(db.getDatabase());

    // Step 1: Remove marks associated with the subject (if ON DELETE CASCADE is not set)
    query.prepare("DELETE FROM marks WHERE subject_id = :subjectId");
    query.bindValue(":subjectId", subjectId);
    if (!query.exec()) {
        QMessageBox::warning(this, "Database Error", "Failed to delete associated marks: " + query.lastError().text());
        return;
    }

    // Step 2: Nullify the teacher_id reference in the subjects table
    query.prepare("UPDATE subjects SET teacher_id = NULL WHERE id = :subjectId");
    query.bindValue(":subjectId", subjectId);
    if (!query.exec()) {
        QMessageBox::warning(this, "Database Error", "Failed to nullify teacher reference: " + query.lastError().text());
        return;
    }

    // Step 3: Delete the subject
    query.prepare("DELETE FROM subjects WHERE id = :subjectId");
    query.bindValue(":subjectId", subjectId);
    if (query.exec()) {
        QMessageBox::information(this, "Success", "Subject deleted successfully.");
        loadSubject();
        loadSubjectsWithTeachers(); // Refresh the UI
    } else {
        QMessageBox::warning(this, "Database Error", "Failed to delete the subject: " + query.lastError().text());
    }
}



void mainApplication::on_pushButton_SubjectEdit_clicked()
{
    int currentIndex = ui->comboBox_Subject->currentIndex();
    if (currentIndex <= 0) {
        QMessageBox::warning(this, "Input Error", "Please select a valid subject to delete.");
        return;
    }

    QVariant subjectId = ui->comboBox_Subject->currentData();
    if (!subjectId.isValid()) {
        QMessageBox::warning(this, "Selection Error", "Could not retrieve the selected subject.");
        return;
    }

    modeldb& db = modeldb::getInstance();
    QSqlQuery query(db.getDatabase());


    bool ok;
    QString newSubjectName = QInputDialog::getText(this, "Delete Group",
                                            "Enter the subject name to enter:",
                                            QLineEdit::Normal, "", &ok);

    if (newSubjectName.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "All fields must be filled.");
        return;
    }


    query.prepare("UPDATE subjects SET name = :newName WHERE id = :subjectId");
    query.bindValue(":newName", newSubjectName.trimmed());
    query.bindValue(":subjectId", subjectId);

    if (query.exec()) {
        QMessageBox::information(this, "Success", "Subject added successfully.");


        loadSubject();
        loadSubjectsWithTeachers(); // Refresh the view

    } else {
        QMessageBox::warning(this, "Database Error", "Failed to update Subject: " + query.lastError().text());
    }
}


void mainApplication::on_pushButton_AssignTeacher_clicked()
{
    // Get the selected subject ID from the combo box
    int subjectIndex = ui->comboBox_Subject->currentIndex();
    if (subjectIndex <= 0) { // First item is "Select a Subject"
        QMessageBox::warning(this, "Input Error", "Please select a valid subject.");
        return;
    }

    QVariant subjectId = ui->comboBox_Subject->currentData();
    if (!subjectId.isValid()) {
        QMessageBox::warning(this, "Selection Error", "Could not retrieve the selected subject.");
        return;
    }

    modeldb& db = modeldb::getInstance();
    QSqlQuery query(db.getDatabase());
    query.prepare("SELECT id, first_name || ' ' || last_name AS full_name FROM people WHERE type = 'P'");

    QStringList teacherList;
    QList<QVariant> teacherIds;

    if (query.exec()) {
        while (query.next()) {
            teacherIds.append(query.value(0));               // Store teacher ID
            teacherList.append(query.value(1).toString());  // Store teacher name
        }
    } else {
        QMessageBox::warning(this, "Database Error", "Failed to fetch teachers: " + query.lastError().text());
        return;
    }

    // Ensure there are teachers available
    if (teacherList.isEmpty()) {
        QMessageBox::information(this, "No Teachers", "There are no teachers available to assign.");
        return;
    }

    // Display a pop-up combo box (QInputDialog)
    bool ok;
    QString selectedTeacher = QInputDialog::getItem(this, "Select Teacher",
                                                    "Choose a teacher to assign to the subject:",
                                                    teacherList, 0, false, &ok);

    if (!ok || selectedTeacher.isEmpty()) {
        QMessageBox::information(this, "Cancelled", "No teacher was selected.");
        return;
    }

    // Find the selected teacher's ID
    int selectedIndex = teacherList.indexOf(selectedTeacher);
    QVariant teacherId = teacherIds[selectedIndex];

    // Confirm assignment
    QString subjectName = ui->comboBox_Subject->currentText();
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Confirm Assignment",
                                  QString("Are you sure you want to assign '%1' to '%2'?")
                                      .arg(selectedTeacher, subjectName),
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::No) {
        return;
    }

    QSqlQuery updateQuery(db.getDatabase());
    updateQuery.prepare("UPDATE subjects SET teacher_id = :teacherId WHERE id = :subjectId");
    updateQuery.bindValue(":teacherId", teacherId);
    updateQuery.bindValue(":subjectId", subjectId);

    if (updateQuery.exec()) {
        QMessageBox::information(this, "Success", "Teacher successfully assigned to the subject.");
        loadSubject(); // Refresh the subjects
        loadSubjectsWithTeachers(); // Refresh the view
    } else {
        QMessageBox::warning(this, "Database Error", "Failed to assign teacher to subject: " + updateQuery.lastError().text());
    }
}

void mainApplication::loadSubjectsWithTeachers()
{
    modeldb& db = modeldb::getInstance();

    QSqlQueryModel *model = new QSqlQueryModel(this);

    QSqlQuery query(db.getDatabase());
    query.prepare("SELECT s.id AS Subject_ID, "
                  "       s.name AS Subject, "
                  "       COALESCE(p.first_name || ' ' || p.last_name, 'No Teacher') AS Teacher "
                  "FROM subjects s "
                  "LEFT JOIN people p ON s.teacher_id = p.id");

    if (!query.exec()) {
        QMessageBox::warning(this, "Database Error", "Failed to load subjects and teachers: " + query.lastError().text());
        return;
    }

    model->setQuery(std::move(query));

    model->setHeaderData(0, Qt::Horizontal, "Subject ID");
    model->setHeaderData(1, Qt::Horizontal, "Subject");
    model->setHeaderData(2, Qt::Horizontal, "Teacher");

    ui->tableViewSubject->setModel(nullptr); // Detach current model
    ui->tableViewSubject->setModel(model);   // Attach refreshed model

    ui->tableViewSubject->resizeColumnsToContents();

    ui->tableViewSubject->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    if (query.lastError().isValid()) {
        qDebug() << "Query execution error:" << query.lastError().text();
    }
}

void mainApplication::exportToCsv(QString type)
{
    modeldb& db = modeldb::getInstance();

    // // Get the selected user type from the combo box
    // QString userType = ui->comboBox_ExportData->currentText();
    // if (userType.isEmpty()) {
    //     QMessageBox::warning(this, "Input Error", "Please select a user type (Students or Staff).");
    //     return;
    // }

    // QString type = (userType == "students") ? "S" : "P";

    QSqlQuery query(db.getDatabase());
    query.prepare("SELECT p.first_name || ' ' || p.last_name AS Name, "
                  "p.father_name AS FatherName, "
                  "COALESCE(g.name, 'N/A') AS GroupName "
                  "FROM people p "
                  "LEFT JOIN groups g ON p.group_id = g.id "
                  "WHERE p.type = :type " // Use dynamic type selection
                  "ORDER BY g.name, p.first_name, p.last_name"); // Sort by group and name
    query.bindValue(":type", type);

    if (!query.exec()) {
        QMessageBox::warning(this, "Database Error", "Failed to fetch data: " + query.lastError().text());
        return;
    }

    QString fileName = QFileDialog::getSaveFileName(this, "Save CSV File", "", "CSV Files (*.csv)");
    if (fileName.isEmpty()) {
        QMessageBox::information(this, "Cancelled", "Export cancelled.");
        return;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "File Error", "Failed to create the CSV file.");
        return;
    }

    QTextStream out(&file);

    // Write headers
    out << "Name,Father Name,Group\n";

    // Write data
    while (query.next()) {
        QString name = query.value("Name").toString();
        QString fatherName = query.value("FatherName").toString();
        QString group = query.value("GroupName").toString();

        // Write to the CSV file in comma-separated format
        out << "\"" << name << "\","
            << "\"" << fatherName << "\","
            << "\"" << group << "\"\n";
    }

    file.close();

    QMessageBox::information(this, "Success", "Data exported to CSV successfully.");
}

void mainApplication::exportToPdf(QString type)
{
    modeldb& db = modeldb::getInstance();

    // // Get the selected user type from the combo box
    // QString userType = ui->comboBox_ExportData->currentText();
    // if (userType.isEmpty()) {
    //     QMessageBox::warning(this, "Input Error", "Please select a user type (Students or Staff).");
    //     return;
    // }

    // QString type = (userType == "students") ? "S" : "P";




    QSqlQuery query(db.getDatabase());
    query.prepare("SELECT p.first_name || ' ' || p.last_name AS Name, "
                  "p.father_name AS FatherName, "
                  "COALESCE(g.name, 'N/A') AS GroupName "
                  "FROM people p "
                  "LEFT JOIN groups g ON p.group_id = g.id "
                  "WHERE p.type = :type " // Use dynamic type selection
                  "ORDER BY g.name, p.first_name, p.last_name"); // Sort by group and name
    query.bindValue(":type", type);

    if (!query.exec()) {
        QMessageBox::warning(this, "Database Error", "Failed to fetch data: " + query.lastError().text());
        return;
    }

    QString fileName = QFileDialog::getSaveFileName(this, "Save PDF File", "", "PDF Files (*.pdf)");
    if (fileName.isEmpty()) {
        QMessageBox::information(this, "Cancelled", "Export cancelled.");
        return;
    }

    // Create the PDF
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

    // Write headers
    painter.drawText(x, y, "Name");
    painter.drawText(x + 400, y, "Father Name");
    painter.drawText(x + 800, y, "Group");
    y += lineHeight;

    // Write data
    while (query.next()) {
        QString name = query.value("Name").toString();
        QString fatherName = query.value("FatherName").toString();
        QString group = query.value("GroupName").toString();

        painter.drawText(x, y, name);
        painter.drawText(x + 400, y, fatherName);
        painter.drawText(x + 800, y, group);
        y += lineHeight;

        // Add new page if necessary
        if (y > pdfWriter.height() - 100) {
            pdfWriter.newPage();
            y = 200; // Reset y to the starting point
            // Redraw headers for the new page
            painter.drawText(x, y, "Name");
            painter.drawText(x + 400, y, "Father Name");
            painter.drawText(x + 800, y, "Group");
            y += lineHeight;
        }
    }

    painter.end();
    QMessageBox::information(this, "Success", "Data exported to PDF successfully.");
}

QString mainApplication::hashPassword(const QString &plainPassword)
{
    return QCryptographicHash::hash(plainPassword.toUtf8(), QCryptographicHash::Sha256).toHex();
}



void mainApplication::loadComboBoxSubjects(QComboBox *comboBox, int studentId)
{
    if (!comboBox) return;

    modeldb& db = modeldb::getInstance();
    comboBox->clear();
    comboBox->addItem("Select a Subject", QVariant());

    if (studentId == 0) return; // No valid student selected

    QSqlQuery query(db.getDatabase());
    query.prepare("SELECT id, name FROM subjects "
                  "WHERE id IN (SELECT subject_id FROM marks WHERE student_id = :studentId) "
                  "ORDER BY name");
    query.bindValue(":studentId", studentId);

    if (query.exec()) {
        while (query.next()) {
            int subjectId = query.value(0).toInt();
            QString subjectName = query.value(1).toString();
            comboBox->addItem(subjectName, subjectId);
        }
    } else {
        QMessageBox::warning(this, "Database Error", "Failed to load subjects: " + query.lastError().text());
    }
}

void mainApplication::loadComboBoxMarks(QComboBox *comboBox)
{
    if (!comboBox) return;

    comboBox->clear();
    for (int i = 2; i <= 5; ++i) {
        comboBox->addItem(QString::number(i), i);
    }
}

void mainApplication::saveOrUpdateMark(int studentId, int subjectId, int teacherId, int mark)
{
    if (studentId == 0 || subjectId == 0 || mark == 0) {
        QMessageBox::warning(this, "Input Error", "Please select valid inputs.");
        return;
    }

    modeldb& db = modeldb::getInstance();
    QSqlQuery query(db.getDatabase());
    query.prepare(
        "INSERT INTO marks (student_id, subject_id, teacher_id, value) "
        "VALUES (:studentId, :subjectId, :teacherId, :mark) "
        "ON CONFLICT (student_id, subject_id) "
        "DO UPDATE SET value = EXCLUDED.value, created_at = CURRENT_TIMESTAMP"
        );

    query.bindValue(":studentId", studentId);
    query.bindValue(":subjectId", subjectId);
    query.bindValue(":teacherId", teacherId);
    query.bindValue(":mark", mark);

    if (query.exec()) {
        QMessageBox::information(this, "Success", "Mark saved successfully.");
    } else {
        QMessageBox::warning(this, "Database Error", "Failed to save mark: " + query.lastError().text());
    }
}
void mainApplication::loadComboBoxStudents(QComboBox *comboBox, int groupId)
{
    if (!comboBox) return;

    modeldb& db = modeldb::getInstance();
    comboBox->clear();
    comboBox->addItem("Select a Student", QVariant());

    if (groupId == 0) return; // No valid group selected

    QSqlQuery query(db.getDatabase());
    query.prepare("SELECT id, first_name, last_name FROM people WHERE type = 'S' AND group_id = :groupId ORDER BY first_name");
    query.bindValue(":groupId", groupId);

    if (query.exec()) {
        while (query.next()) {
            int studentId = query.value(0).toInt();
            QString studentName = query.value(1).toString() + " " + query.value(2).toString();
            comboBox->addItem(studentName, studentId);
        }
    } else {
        QMessageBox::warning(this, "Database Error", "Failed to load students: " + query.lastError().text());
    }
}

void mainApplication::loadComboBoxStudents(QComboBox *comboBox)
{
    if (!comboBox) return;

    modeldb& db = modeldb::getInstance();
    comboBox->clear();
    comboBox->addItem("Select a Student", QVariant());



    QSqlQuery query(db.getDatabase());
    query.prepare("SELECT id, first_name, last_name FROM people WHERE type = 'S' ORDER BY first_name");


    if (query.exec()) {
        while (query.next()) {
            int studentId = query.value(0).toInt();
            QString studentName = query.value(1).toString() + " " + query.value(2).toString();
            comboBox->addItem(studentName, studentId);
        }
    } else {
        QMessageBox::warning(this, "Database Error", "Failed to load students: " + query.lastError().text());
    }
}






void mainApplication::loadComboBoxGroupForMark(QComboBox* comboBox, const QString& placeholderText)
{
    if (!comboBox) {
        qWarning() << "Null pointer passed for comboBox.";
        return;
    }

    modeldb& db = modeldb::getInstance();

    comboBox->clear(); // Clear any existing items
    comboBox->addItem(placeholderText, QVariant()); // Add placeholder

    QSqlQuery query(db.getDatabase());
    query.prepare("SELECT id, name FROM groups ORDER BY name");

    if (query.exec()) {
        while (query.next()) {
            int groupId = query.value(0).toInt();
            QString groupName = query.value(1).toString();
            comboBox->addItem(groupName, groupId);
        }
    } else {
        QMessageBox::warning(this, "Database Error", "Failed to load groups: " + query.lastError().text());
    }
}

void mainApplication::loadComboBoxSubjectsForMark()
{
    modeldb& db = modeldb::getInstance();

    ui->comboBox_MarkSelectSubject->clear();
    ui->comboBox_MarkSelectSubject->addItem("Select a Subject", QVariant()); // Placeholder item

    QSqlQuery query(db.getDatabase());
    query.prepare("SELECT id, name FROM subjects ORDER BY name");

    if (query.exec()) {
        while (query.next()) {
            int subjectId = query.value(0).toInt();
            QString subjectName = query.value(1).toString();
            ui->comboBox_MarkSelectSubject->addItem(subjectName, subjectId);
        }
    } else {
        QMessageBox::warning(this, "Database Error", "Failed to load subjects: " + query.lastError().text());
    }
}



void mainApplication::on_pushButton_AssignGroup_clicked()
{
    modeldb& db = modeldb::getInstance();

    QSqlQuery groupQuery(db.getDatabase());
    groupQuery.prepare("SELECT id, name FROM groups");

    QStringList groupList;
    QList<QVariant> groupIds;

    if (groupQuery.exec()) {
        while (groupQuery.next()) {
            groupIds.append(groupQuery.value(0));               // Store group ID
            groupList.append(groupQuery.value(1).toString());  // Store group name
        }
    } else {
        QMessageBox::warning(this, "Database Error", "Failed to fetch groups: " + groupQuery.lastError().text());
        return;
    }

    if (groupList.isEmpty()) {
        QMessageBox::information(this, "No Groups", "There are no groups available.");
        return;
    }

    bool ok;
    QString selectedGroup = QInputDialog::getItem(this, "Select Group",
                                                  "Choose a group to assign a subject:",
                                                  groupList, 0, false, &ok);

    if (!ok || selectedGroup.isEmpty()) {
        QMessageBox::information(this, "Cancelled", "No group was selected.");
        return;
    }

    int selectedGroupIndex = groupList.indexOf(selectedGroup);
    QVariant groupId = groupIds[selectedGroupIndex];

    QSqlQuery subjectQuery(db.getDatabase());
    subjectQuery.prepare("SELECT id, name FROM subjects");

    QStringList subjectList;
    QList<QVariant> subjectIds;

    if (subjectQuery.exec()) {
        while (subjectQuery.next()) {
            subjectIds.append(subjectQuery.value(0));               // Store subject ID
            subjectList.append(subjectQuery.value(1).toString());  // Store subject name
        }
    } else {
        QMessageBox::warning(this, "Database Error", "Failed to fetch subjects: " + subjectQuery.lastError().text());
        return;
    }

    if (subjectList.isEmpty()) {
        QMessageBox::information(this, "No Subjects", "There are no subjects available.");
        return;
    }

    QString selectedSubject = QInputDialog::getItem(this, "Select Subject",
                                                    "Choose a subject to assign to the group:",
                                                    subjectList, 0, false, &ok);

    if (!ok || selectedSubject.isEmpty()) {
        QMessageBox::information(this, "Cancelled", "No subject was selected.");
        return;
    }

    int selectedSubjectIndex = subjectList.indexOf(selectedSubject);
    QVariant subjectId = subjectIds[selectedSubjectIndex];

    QSqlQuery studentQuery(db.getDatabase());
    studentQuery.prepare("SELECT id FROM people WHERE group_id = :groupId AND type = 'S'");
    studentQuery.bindValue(":groupId", groupId);

    if (!studentQuery.exec()) {
        QMessageBox::warning(this, "Database Error", "Failed to fetch students: " + studentQuery.lastError().text());
        return;
    }

    QSqlQuery assignQuery(db.getDatabase());
    int assignedCount = 0;

    while (studentQuery.next()) {
        QVariant studentId = studentQuery.value(0); // Get student ID

        // Check if this subject is already assigned to the student
        QSqlQuery checkQuery(db.getDatabase());
        checkQuery.prepare("SELECT 1 FROM marks WHERE student_id = :studentId AND subject_id = :subjectId");
        checkQuery.bindValue(":studentId", studentId);
        checkQuery.bindValue(":subjectId", subjectId);

        if (checkQuery.exec() && checkQuery.next()) {
            // Skip if already assigned
            continue;
        }

        // Assign the subject to the student
        assignQuery.prepare("INSERT INTO marks (student_id, subject_id) VALUES (:studentId, :subjectId)");
        assignQuery.bindValue(":studentId", studentId);
        assignQuery.bindValue(":subjectId", subjectId);

        if (assignQuery.exec()) {
            assignedCount++;
        } else {
            qDebug() << "Failed to assign subject to student ID:" << studentId << "Error:" << assignQuery.lastError().text();
        }
    }

    QMessageBox::information(this, "Assignment Complete",
                             QString("Subject '%1' successfully assigned to %2 students in group '%3'.")
                                 .arg(selectedSubject, QString::number(assignedCount), selectedGroup));

   //loadGrades(); // Refresh the grades view
}



void mainApplication::on_pushButton_ViewGrades_clicked()
{
    loadGrades();
}


void mainApplication::on_pushButton_ExportData_clicked()
{

    modeldb& db = modeldb::getInstance();

    // Get the selected user type from the combo box
    QString userType = ui->comboBox_ExportData->currentText();
    if (userType.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Please select a user type (Students or Staff).");
        return;
    }

    QString type = (userType == "students") ? "S" : "P";



    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Choose Export Format");
    msgBox.setText("How would you like to export the data?");
    QPushButton* pdfButton = msgBox.addButton("Export as PDF", QMessageBox::AcceptRole);
    QPushButton* csvButton = msgBox.addButton("Export as CSV", QMessageBox::AcceptRole);
    QPushButton* cancelButton = msgBox.addButton("Cancel", QMessageBox::RejectRole);

    msgBox.exec();

    if (msgBox.clickedButton() == cancelButton) {
        QMessageBox::information(this, "Cancelled", "Export cancelled.");
        return;
    }

    if (msgBox.clickedButton() == pdfButton) {
        exportToPdf(type);
    } else if (msgBox.clickedButton() == csvButton) {
        exportToCsv(type);
    }

}


void mainApplication::on_pushButton_MarkView_clicked()
{

    modeldb& db = modeldb::getInstance();
    // Create a QSqlQueryModel to display and edit the results in the QTableView
    QSqlQueryModel *model = new QSqlQueryModel(this);

    QSqlQuery query(db.getDatabase());
    query.prepare(R"(
        SELECT m.id AS Mark_ID,
               p.first_name || ' ' || p.last_name AS Student_Name,
               s.name AS Subject,
               m.value AS Mark
        FROM marks m
        JOIN people p ON m.student_id = p.id
        JOIN subjects s ON m.subject_id = s.id
        WHERE p.group_id = :groupId
        ORDER BY p.first_name
    )");

    QVariant groupId = ui->comboBox_MarkSelectGroup->currentData();
    if (!groupId.isValid()) {
        QMessageBox::warning(this, "Input Error", "Please select a valid group.");
        return;
    }
    query.bindValue(":groupId", groupId.toInt());

    if (!query.exec()) {
        QMessageBox::warning(this, "Database Error", query.lastError().text());
        return;
    }

    model->setQuery(std::move(query));

    model->setHeaderData(0, Qt::Horizontal, "Mark ID");
    model->setHeaderData(1, Qt::Horizontal, "Student Name");
    model->setHeaderData(2, Qt::Horizontal, "Subject");
    model->setHeaderData(3, Qt::Horizontal, "Mark");

    ui->tableViewMark->setModel(model);

    ui->tableViewMark->resizeColumnsToContents();

    ui->tableViewMark->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    ui->tableViewMark->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void mainApplication::on_pushButton_SaveMark_clicked()
{
    int studentId = ui->comboBox_EditMarkSelectStudent->currentData().toInt();
    int subjectId = ui->comboBox_EditMarkChooseSubject_2->currentData().toInt();
    int mark = ui->comboBox_EditMark_Mark->currentData().toInt();
    saveOrUpdateMark(studentId, subjectId,35, mark);//админ can be able to change but who knows
//                                                   i dont have any idea for a ideal way to handle logically i mean
}


void mainApplication::on_tabWidget_2_currentChanged(int index)
{
    loadComboBoxGroupForMark(ui->comboBox_EditMarkSelectGroup);

}


void mainApplication::on_comboBox_EditMarkSelectGroup_currentIndexChanged(int index)
{
    loadComboBoxStudents(ui->comboBox_EditMarkSelectStudent, index);
}


void mainApplication::on_comboBox_EditMarkSelectStudent_currentIndexChanged(int index)
{
    int studentId = ui->comboBox_EditMarkSelectStudent->itemData(index).toInt();
    loadComboBoxSubjects(ui->comboBox_EditMarkChooseSubject_2, studentId);
    ui->comboBox_EditMark_Mark->clear(); // Clear marks until a subject is selected
}


void mainApplication::on_comboBox_EditMarkChooseSubject_2_currentIndexChanged(int index)
{
      loadComboBoxMarks(ui->comboBox_EditMark_Mark);
}


void mainApplication::on_MA_tabWidget_Main_tabBarClicked(int index)
{
    loadComboBoxStudents(ui->comboBox_PL_StudentChoose);
    ui->lineEdit_PL_password->setPlaceholderText("username");
    ui->lineEdit_PL_username->setPlaceholderText("password");

}


void mainApplication::on_pushButton_AddLogin_clicked()
{
    QString studentName = ui->comboBox_PL_StudentChoose->currentText();
    QString username = ui->lineEdit_PL_username->text();
    QString plainPassword = ui->lineEdit_PL_password->text();

    // Validate inputs
    if (studentName.isEmpty() || username.isEmpty() || plainPassword.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Please fill in all fields.");
        return;
    }

    QSqlQuery query;

    // Retrieve the student_id based on the selected student name
    query.prepare("SELECT id FROM people WHERE CONCAT(first_name, ' ', last_name) = :studentName AND type = 'S'");
    query.bindValue(":studentName", studentName);

    if (!query.exec() || !query.next()) {
        QMessageBox::critical(this, "Database Error", "Failed to retrieve student ID: " + query.lastError().text());
        return;
    }

    int studentId = query.value("id").toInt();

    // Check if the user already exists in the auth_users table
    query.prepare("SELECT COUNT(*) FROM auth_users WHERE student_id = :student_id");
    query.bindValue(":student_id", studentId);

    if (!query.exec() || !query.next()) {
        QMessageBox::critical(this, "Database Error", "Failed to check existing user: " + query.lastError().text());
        return;
    }

    bool userExists = query.value(0).toInt() > 0;

    // Hash the password using the utility function
    QString passwordHash = hashPassword(plainPassword);

    if (userExists) {
        // Update existing user
        query.prepare("UPDATE auth_users SET username = :username, password_hash = :password_hash WHERE student_id = :student_id");
    } else {
        // Insert new user
        query.prepare("INSERT INTO auth_users (student_id, username, password_hash, role) "
                      "VALUES (:student_id, :username, :password_hash, 'Student')");
    }

    query.bindValue(":student_id", studentId);
    query.bindValue(":username", username);
    query.bindValue(":password_hash", passwordHash);

    if (!query.exec()) {
        QMessageBox::critical(this, "Database Error", "Failed to add or update the student's login credentials: " + query.lastError().text());
        return;
    }

    // Success feedback
    QMessageBox::information(this, "Success", userExists
                                                  ? "Login credentials updated successfully!"
                                                  : "Login credentials added successfully!");

    // Clear the input fields
    ui->lineEdit_PL_username->clear();
    ui->lineEdit_PL_password->clear();
}

void mainApplication::on_pushButton_AddConfig_clicked()
{
    QString configFilePath = QFileDialog::getOpenFileName(
        nullptr,
        "Select Database Configuration File",
        QCoreApplication::applicationDirPath(),
        "INI Files (*.ini);;All Files (*)"
        );

    // Check if the user canceled the dialog
    if (configFilePath.isEmpty()) {
        qDebug() << "No configuration file selected. Exiting...";
        return;
    }

    QString targetFilePath = QCoreApplication::applicationDirPath() + "/main.ini";

    QFile sourceFile(configFilePath);
    QFile targetFile(targetFilePath);

    // Open the source file for reading
    if (!sourceFile.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open source file for reading:" << configFilePath;
        return;
    }

    // Open the target file for writing
    if (!targetFile.open(QIODevice::WriteOnly)) {
        qDebug() << "Failed to open target file for writing:" << targetFilePath;
        return;
    }

    // Copy the contents of the source file to the target file
    QByteArray fileContents = sourceFile.readAll();
    targetFile.write(fileContents);

    // Close both files
    sourceFile.close();
    targetFile.close();

    qDebug() << "Configuration file copied successfully to:" << targetFilePath;
}

