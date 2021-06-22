#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent, QString name) : QDialog(parent), ui(new Ui::Dialog) {
    ui->setupUi(this);
    this->setWindowTitle("new project");
    ui->lineEdit->setText(name);
}

Dialog::~Dialog() { delete ui; }

void Dialog::on_buttonBox_accepted() {
    itemText = ui->lineEdit->text();
}
