/**
 * Copyright 2021 Dimitris Psathas <dimitrisinbox@gmail.com>
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


#include "dialog.h"
#include "ui_dialog.h"


Dialog::Dialog(QWidget *parent, QString name, QString kind)
    : QDialog(parent)
    , ui(new Ui::Dialog)
{
    ui->setupUi(this);
    ui->lineEdit->setText(name);
    this->setWindowTitle(kind);
    if (kind == "New Task" || kind == "Edit Task Name") {
        ui->label->setText("note: line changes will be replaced with underscore ('_')");
    } else if (kind == "New alert") {
        ui->label->setVisible(false);
    }
}


Dialog::~Dialog()
{
    delete ui;
}


void Dialog::on_buttonBox_accepted()
{
    QString text = ui->lineEdit->text();
    if (text != "") {
        itemText = text;
    } else {
        itemText = "_";
    }
}
