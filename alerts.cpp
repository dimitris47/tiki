/**
 * Copyright 2021-2023 Dimitris Psathas <dimitrisinbox@gmail.com>
 *
 * This file is part of Tiki.
 *
 * Tiki is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License  as  published by  the  Free Software
 * Foundation,  either version 3 of the License,  or (at your option)  any later
 * version.
 *
 * Tiki is distributed in the hope that it will be useful,  but  WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.  See the  GNU General Public License  for more details.
 *
 * You should have received a copy of the  GNU General Public License along with
 * Tiki. If not, see <http://www.gnu.org/licenses/>.
 */


#include "alerts.h"
#include "ui_alerts.h"
#include "dialog.h"


Alerts::Alerts(QWidget *parent, QStringList alerts) :
    QDialog(parent),
    ui(new Ui::Alerts)
{
    ui->setupUi(this);
    this->alrt = alerts;
    for (auto &&al : alrt) {
        ui->listWidget->addItem(al);
    }
}


Alerts::~Alerts()
{
    delete ui;
}


void Alerts::on_addButton_clicked()
{
    auto widget = new Dialog(this, "", "New alert");
    int ret = widget->exec();
    if (ret == QDialog::Rejected) {
        return;
    }
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
    for (int i=0; i<ui->listWidget->count(); i++) {
        alrt.append(ui->listWidget->item(i)->text().replace("&&", "&"));
    }
}
