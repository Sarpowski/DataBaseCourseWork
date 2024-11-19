#include "mainapplication.h"
#include "ui_mainapplication.h"
#include <QTimer>


#include <QSqlTableModel>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

#include <QInputDialog>
#include <QMessageBox>
#include <QRegularExpression>
#include <QComboBox>
#include <QCheckBox>
#include <qmessagebox.h>

#include "modeldb.h"

mainApplication::mainApplication(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::mainApplication)

{
    ui->setupUi(this);


    firstRun_ = true;

    modeldb& db = modeldb::getInstance();
    db.initDb();

    initMainAppTableView(db);
    groupBoxGroupSelect();
    checkBoxFilter();
    loadStudentNames();


    //Connections
    connect(ui->pushButton_AddGroup,&QPushButton::clicked,this,&mainApplication::pushButton_AddGroup);
    connect(ui->MA_pushButton, &QPushButton::clicked, this, &mainApplication::on_pushButtonLoadTable_clicked);
    connect(ui->MA_checkBox_Student, &QCheckBox::checkStateChanged, this, &mainApplication::checkBoxFilter);
    connect(ui->MA_checkBox_Teacher, &QCheckBox::checkStateChanged, this, &mainApplication::checkBoxFilter);
    connect(ui->pushButtonStudentEdit, &QPushButton::clicked, this, &mainApplication::editStudent);
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

    // Construct the filter string based on checkbox states
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

    // Apply the filter to the model
    model->setFilter(finalFilter);
    if (!model->select()) {
        qDebug() << "Error applying filter:" << model->lastError().text();
    }
}

void mainApplication::groupBoxGroupSelect()
{
    // modeldb& db = modeldb::getInstance();
    // ui->MA_comboBox_SelectGroup->clear();

    // ui->MA_comboBox_SelectGroup->addItem("Select a group");

    // ui->MA_comboBox_group->addItem("Select a group");
    // QSqlQuery queryGroup(db.getDatabase());


    // if (queryGroup.exec("SELECT name FROM groups")) {
    //     while (queryGroup.next()) {
    //         QString groupName = queryGroup.value(0).toString();
    //         ui->MA_comboBox_group->addItem(groupName);
    //         ui->MA_comboBox_SelectGroup->addItem(groupName);
    //     }

    // } else {
    //     qDebug() << "Failed to retrieve group names:" << queryGroup.lastError().text();
    // }

    modeldb& db = modeldb::getInstance();

    // Clear existing items
    ui->MA_comboBox_SelectGroup->clear();
    ui->MA_comboBox_group->clear();

    // Add default item
    ui->MA_comboBox_SelectGroup->addItem("Select a group");
    ui->MA_comboBox_group->addItem("Select a group");

    QSqlQuery queryGroup(db.getDatabase());

    // Fetch group names from the database
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

// //TODO
// void mainApplication::editStudent()
// {
//     ui->MA_lineEdit_EditName->setPlaceholderText("Name");
//     ui->MA_lineEdit_EditSurname->setPlaceholderText("Surname");
//     ui->MA_lineEdit_EditFatherName->setPlaceholderText("Father Name");
//     ui->MA_comboBox_EditStudent->setPlaceholderText("choose student");
//     ui->MA_comboBox_SelectGroup->setPlaceholderText("Select a group");
//   //  ui->pushButtonStudentEdit->setText("Edit");
//  //   ui->pushButton_StudentAdd->setText("Add");
//  //   ui->pushButton_StudentDelete->setText("Delete");


// }



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
        //   QMessageBox::warning(this, "Error", "Failed to retrieve group ID. Please check the database.");
        return;
    }

    int groupId = groupQuery.value(0).toInt();
    qDebug() << "Group ID for selected group:" << groupId;

    // Proceed with the insertion if no duplicate is found
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

    // Delete the student
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


//DOPE
void mainApplication::on_pushButton_TESTadd_clicked()
{
    addStudent();

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
        // qDebug() << "Failed to delete group:" << deleteQuery.lastError().text();
        // QMessageBox::warning(this, "Database Error", "Failed to delete group.");
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

void mainApplication::loadStudentNames() {
    modeldb& db = modeldb::getInstance();

    // Clear the combo box before populating
    ui->MA_comboBox_EditStudent->clear();

    // Add a default "Select a student" item
    ui->MA_comboBox_EditStudent->addItem("Select a student");

    // Query to fetch student names and IDs
    QSqlQuery query(db.getDatabase());
    query.prepare("SELECT id, first_name, last_name FROM people WHERE type = 'S' ORDER BY first_name");

    if (query.exec()) {
        while (query.next()) {
            QString studentId = query.value(0).toString();
            QString studentName = query.value(1).toString() + " " + query.value(2).toString();
            // Add student ID and name to the combo box
            ui->MA_comboBox_EditStudent->addItem(studentName, studentId);
        }
    } else {
        QMessageBox::warning(this, "Database Error", "Failed to load student names: " + query.lastError().text());
    }
}


void mainApplication::editStudent() {
    modeldb& db = modeldb::getInstance();

    // Get selected student ID from the combo box
    QString studentId = ui->MA_comboBox_EditStudent->currentData().toString();
    if (studentId.isEmpty() || studentId == "Select a student"  ) {
        QMessageBox::warning(this, "Input Error", "Please select a valid student EDIT part.");
        return;
    }

    // Fetch the selected group name and get the corresponding group ID
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

    // Fetch the edited details
    QString firstName = ui->MA_lineEdit_EditName->text().trimmed();
    QString lastName = ui->MA_lineEdit_EditSurname->text().trimmed();
    QString fatherName = ui->MA_lineEdit_EditFatherName->text().trimmed();

    // Validate inputs
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

        // Refresh the combo box to reflect the updated student details
        loadStudentNames();

        // Reselect the updated student in the combo box
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

