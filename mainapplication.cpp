#include "mainapplication.h"
#include "ui_mainapplication.h"


#include <QSqlTableModel>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>


#include <QComboBox>

#include "modeldb.h"

mainApplication::mainApplication(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::mainApplication)
{
    ui->setupUi(this);


    modeldb& db = modeldb::getInstance();
    db.initDb();

    initMainAppTableView(db);
    ui->MA_comboBox->addItem("Select a group"); // Add placeholder item

    QSqlQuery query(db.getDatabase());
    if (query.exec("SELECT name FROM groups")) {
        while (query.next()) {
            QString groupName = query.value(0).toString();
            ui->MA_comboBox->addItem(groupName);
        }
    } else {
        qDebug() << "Failed to retrieve group names:" << query.lastError().text();
    }

    connect(ui->MA_pushButton, &QPushButton::clicked, this, &mainApplication::on_pushButtonLoadTable_clicked);

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
    QString selectedGroup = ui->MA_comboBox->currentText();

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

