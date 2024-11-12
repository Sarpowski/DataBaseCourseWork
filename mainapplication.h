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

private slots:
    void on_pushButtonLoadTable_clicked();

private:
    Ui::mainApplication *ui;
};

#endif // MAINAPPLICATION_H
