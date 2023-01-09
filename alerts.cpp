#include "alerts.h"
#include "ui_alerts.h"
#include "dialog.h"


Alerts::Alerts(QWidget *parent, QStringList alerts) :
    QDialog(parent),
    ui(new Ui::Alerts)
{
    ui->setupUi(this);
    this->alrt = alerts;
    for (auto &&al : alrt)
        ui->listWidget->addItem(al);
}


Alerts::~Alerts()
{
    delete ui;
}


void Alerts::on_addButton_clicked()
{
    auto widget = new Dialog(this, "", "New alert");
    int ret = widget->exec();
    if (ret == QDialog::Rejected)
        return;
    if (ret) {
        QString alertName = widget->itemText;
        ui->listWidget->addItem(alertName);
    }
}


void Alerts::on_removeButton_clicked()
{
    ui->listWidget->takeItem(ui->listWidget->currentRow());
}


void Alerts::on_buttonBox_accepted()
{
    alrt.clear();
    for (int i=0; i<ui->listWidget->count(); i++)
        alrt.append(ui->listWidget->item(i)->text().replace("&&", "&"));
}
