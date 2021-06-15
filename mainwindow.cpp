#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dialog.h"
#include "organizer.h"
#include "project.h"
#include "task.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) , ui(new Ui::MainWindow) {
    ui->setupUi(this);
    readPrefs();
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::debugProjects() {
    QStringList names;
    for (auto &&project : Organizer::Projects)
        names.append(project.name());
    qDebug() << "Projects:\n" << names;
}

void MainWindow::on_addProBtn_clicked() {
    auto widget = new Dialog(this);
    int ret = widget->exec();
    if (ret == QDialog::Rejected)
        return;
    if (ret) {
        ui->projectWidget->addItem(widget->itemText);
        Organizer::Projects.append(Project(widget->itemText));
    }
    debugProjects();
}

void MainWindow::on_renameProBtn_clicked() {
    if (ui->projectWidget->currentItem() == NULL) {
        ui->statusbar->showMessage("No project selected", 1000);
        return;
    }
    auto widget = new Dialog(this);
    int ret = widget->exec();
    if (ret == QDialog::Rejected)
        return;
    if (ret) {
        ui->projectWidget->currentItem()->setText(widget->itemText);
        Organizer::Projects[ui->projectWidget->currentRow()].setName(widget->itemText);
    }
    debugProjects();
}

void MainWindow::on_rmProBtn_clicked() {
    if (ui->projectWidget->currentItem() == NULL) {
        ui->statusbar->showMessage("No project selected", 1000);
        return;
    }
    int row = ui->projectWidget->currentRow();
    Organizer::Projects.removeAt(row);
    ui->projectWidget->takeItem(row);
}

void MainWindow::on_projectWidget_currentRowChanged(int currentRow) {
    ui->taskWidget->clear();
    QStringList items;
    for (auto &&task : Organizer::Projects.at(currentRow).tasks)
        items.append(task.getTaskName());
    ui->taskWidget->addItems(items);
}

void MainWindow::on_addTaskBtn_clicked() {
    if (ui->projectWidget->currentItem() == NULL) {
        ui->statusbar->showMessage("No project selected", 1000);
        return;
    }
    auto widget = new Dialog(this);
    int ret = widget->exec();
    if (ret == QDialog::Rejected)
        return;
    if (ret) {
        auto tasks = Organizer::Projects.at(ui->projectWidget->currentRow()).tasks;
        tasks.append(Task(widget->itemText));
        ui->taskWidget->clear();
        QStringList items;
        for (auto &&task : tasks)
            items.append(task.getTaskName());
        ui->taskWidget->addItems(items);
    }
}

void MainWindow::on_renameTaskBtn_clicked() {
}

void MainWindow::on_highBtn_clicked() {

}

void MainWindow::on_medBtn_clicked() {

}

void MainWindow::on_lowBtn_clicked() {

}

void MainWindow::on_doneBtn_clicked() {

}

void MainWindow::on_rmTaskBtn_clicked() {

}

void MainWindow::readPrefs() {
    QSettings settings;
    bool isMax = settings.value("isMaximized", false).toBool();
    if (isMax)
        showMaximized();
    else {
        const QByteArray geometry = settings.value("geometry", QByteArray()).toByteArray();
        restoreGeometry(geometry);
    }
}

void MainWindow::savePrefs() {
    QSettings settings;
    settings.setValue("isMaximized", isMaximized());
    if (!isMaximized())
        settings.setValue("geometry", saveGeometry());
    settings.sync();
}

void MainWindow::closeEvent(QCloseEvent *event) {
    savePrefs();
    event->accept();
}
