#include "customdialog.h"


CustomDialog::CustomDialog(const QStringList &items)
{
    setLayout(new QHBoxLayout());

    box = new QComboBox;

    box->addItems(items);
    layout()->addWidget(box);

    QPushButton* ok = new QPushButton("ok");
    layout()->addWidget(ok);
    connect(ok, &QPushButton::clicked, this, [this]()
            {
                accept();
            });

}

QComboBox *CustomDialog::comboBox()
{
    return box;
}
