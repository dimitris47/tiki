#include "archive.h"
#include "ui_archive.h"

Archive::Archive(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Archive)
{
    ui->setupUi(this);
}

Archive::~Archive()
{
    delete ui;
}

void Archive::on_buttonBox_accepted()
{

}

