#ifndef CUSTOMDIALOG_H
#define CUSTOMDIALOG_H

#include <QDialog>
#include <qboxlayout.h>
#include <qcombobox.h>
#include <qpushbutton.h>

class CustomDialog : public QDialog
{
public:
    CustomDialog(const QStringList& items);

    QComboBox* comboBox();

private:
    QComboBox* box;
};

#endif // CUSTOMDIALOG_H
