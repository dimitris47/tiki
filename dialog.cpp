#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent, QString name, QString kind) : QDialog(parent), ui(new Ui::Dialog) {
    ui->setupUi(this);
    ui->lineEdit->setText(name);
    this->setWindowTitle(kind);
}

Dialog::~Dialog() { delete ui; }

void Dialog::on_buttonBox_accepted() {
    itemText = ui->lineEdit->text();
}
