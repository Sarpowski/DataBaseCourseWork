#include "mainapplication.h"
#include "ui_mainapplication.h"
#include <QTimer>


#include <QSqlTableModel>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>


#include <QComboBox>
#include <QCheckBox>
#include <qmessagebox.h>

#include "modeldb.h"

mainApplication::mainApplication(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::mainApplication)
{
    ui->setupUi(this);




    modeldb& db = modeldb::getInstance();
    db.initDb();

    initMainAppTableView(db);
    groupBoxGroupSelect();
    checkBoxFilter();
    editTab();

    connect(ui->pushButton_addStudent, &QPushButton::clicked, this, &mainApplication::on_pushButton_addStudent_clicked);
    //Connections
    connect(ui->MA_pushButton, &QPushButton::clicked, this, &mainApplication::on_pushButtonLoadTable_clicked);
    connect(ui->MA_checkBox_Student, &QCheckBox::stateChanged, this, &mainApplication::checkBoxFilter);
    connect(ui->MA_checkBox_Teacher, &QCheckBox::stateChanged, this, &mainApplication::checkBoxFilter);
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

    // İsteğe bağlı: Sütun başlıklarını değiştirebilirsiniz
    model->setHeaderData(0, Qt::Horizontal, "Student id");
    model->setHeaderData(1, Qt::Horizontal, "Name");
    model->setHeaderData(2, Qt::Horizontal, "Surname");
    model->setHeaderData(3, Qt::Horizontal, "Patronymic name");
    model->setHeaderData(4, Qt::Horizontal, "Groupd Id");
    model->setHeaderData(5, Qt::Horizontal, "Type S/P");

    // QTableView ile modeli bağla

    ui->MainAppTableView->setModel(model);
    ui->MainAppTableView->resizeColumnsToContents();
    if (!model->select()) {
        qDebug() << "Error loading table data:" << model->lastError().text();
    }
}

void mainApplication::on_pushButtonLoadTable_clicked()
{

    // Get the selected group name from the combo box
    QString selectedGroup = ui->MA_comboBox_group->currentText();

    if (selectedGroup == "Select a group") {
        // If placeholder text is selected, show all data
        QSqlTableModel *model = static_cast<QSqlTableModel *>(ui->MainAppTableView->model());
        if (model) {
            model->setFilter(""); // Show all data
            model->select();
        }
        return;
    }

    // Cast the model to QSqlTableModel
    QSqlTableModel *model = static_cast<QSqlTableModel *>(ui->MainAppTableView->model());

    if (model) {
        // Set the filter to only show rows matching the selected group
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
    // Get the model from the QTableView
    QSqlTableModel *model = static_cast<QSqlTableModel *>(ui->MainAppTableView->model());

    if (!model) {
        qDebug() << "Model is not properly initialized.";
        return;
    }

    // Construct the filter string based on checkbox states
    QStringList filterParts;

    // Filtering by group if a valid group is selected
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
        // If neither checkbox is checked, show no results
        filterParts << "1 = 0";  // Ensures that no rows are returned
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
    modeldb& db = modeldb::getInstance();
    ui->MA_comboBox_SelectGroup->clear();

    //Select Group ComboBox
    ui->MA_comboBox_SelectGroup->addItem("Select a group"); // Add placeholder item

    ui->MA_comboBox_group->addItem("Select a group"); // Add placeholder item
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

void mainApplication::editTab()
{
    editStudent();
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


void mainApplication::editStudent()
{
    ui->MA_lineEdit_EditName->setPlaceholderText("Name");
    ui->MA_lineEdit_EditSurname->setPlaceholderText("Surname");
    ui->MA_lineEdit_EditFatherName->setPlaceholderText("Father Name");
    ui->MA_comboBox_EditStudent->setPlaceholderText("choose student");
    ui->MA_comboBox_SelectGroup->setPlaceholderText("Select a group");
  //  ui->pushButtonStudentEdit->setText("Edit");
 //   ui->pushButton_StudentAdd->setText("Add");
 //   ui->pushButton_StudentDelete->setText("Delete");


}




void mainApplication::addStudent() {



    qDebug() << "addStudent called";  // Log entry for function call

    QString name = ui->MA_lineEdit_EditName->text();
    QString surname = ui->MA_lineEdit_EditSurname->text();
    QString fatherName = ui->MA_lineEdit_EditFatherName->text();
    QString groupName = ui->MA_comboBox_SelectGroup->currentText();

    if (groupName == "Select a group" || name.isEmpty() || surname.isEmpty() || fatherName.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Please fill all fields and select a valid group.");
        return;
    }

    modeldb& db = modeldb::getInstance();

    // Retrieve the group ID based on the group name
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





}
void mainApplication::on_pushButton_addStudent_clicked()
{
    addStudent();


}


void mainApplication::on_pushButton_StudentDelete_clicked()
{
    modeldb& db = modeldb::getInstance();

    QString firstName = ui->MA_lineEdit_EditName->text().trimmed();
    QString lastName = ui->MA_lineEdit_EditSurname->text().trimmed();
    QString fatherName = ui->MA_lineEdit_EditFatherName->text().trimmed();
    QString groupName = ui->MA_comboBox_SelectGroup->currentText().trimmed();

    // Check if all fields are filled and that a valid group is selected
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

    // Reset the input fields
    ui->MA_lineEdit_EditName->clear();
    ui->MA_lineEdit_EditSurname->clear();
    ui->MA_lineEdit_EditFatherName->clear();
    ui->MA_comboBox_SelectGroup->setCurrentIndex(-1);

    // Optionally, refresh the list of students to reflect the deletion
    initMainAppTableView(db);
}

