#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dialog.h"
#include "organizer.h"
#include "project.h"
#include "task.h"
#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QStandardPaths>
#include <QTextCodec>

#define CURR_PRO Organizer::Projects[ui->projectWidget->currentRow()]
#define CURR_PRO_TASKS Organizer::Projects[ui->projectWidget->currentRow()].tasks

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) , ui(new Ui::MainWindow) {
    ui->setupUi(this);
    readPrefs();
    readProjects();
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::readProjects() {
    QDir dataDir = QDir(QStandardPaths::standardLocations(QStandardPaths::AppDataLocation).at(0));
    QDirIterator it(dataDir.path(), QDir::Files, QDirIterator::Subdirectories);
    int i {0};
    while (it.hasNext()) {
        QFile file(it.next());
        Project project = Project(file.fileName().remove(dataDir.path() + '/').remove(".txt"));
        Organizer::Projects.append(project);
        file.open(QIODevice::ReadOnly);
        QTextStream reader(&file);
        reader.setCodec(QTextCodec::codecForName("UTF-8"));
        while (!reader.atEnd()) {
            QString line = reader.readLine();
            if (line.contains("-->>")) {
                QString taskName = line.split("-->>").at(0);
                Task task = Task(taskName);
                Organizer::Projects[i].tasks.append(task);
            }
        }
        i++;
    }
    for (auto &&pro : Organizer::Projects) {
        ui->projectWidget->addItem(pro.name());
        qDebug() << pro.toString();
    }
}

void MainWindow::saveProjects() {
    for (auto &&project : Organizer::Projects) {
        QString projectData;
        for (auto &&task : project.tasks)
            projectData.append(task.name() + "-->>" +
                               (task.status() ? "true" : "false") + "-->>" +
                               QString::number(task.priority()) + '\n');
        QDir dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        if (!dataDir.exists())
            dataDir.mkpath(".");
        QString fileName = dataDir.path() + '/' + project.name() + ".txt";
        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly | QFile::Text)) {
            qWarning() << tr("error opening %1").arg(fileName);
            return;
        }
        QTextStream data(&file);
        data.setCodec(QTextCodec::codecForName("UTF-8"));
        data.setIntegerBase(10);
        data << projectData;
        file.close();
    }
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
    saveProjects();
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
        CURR_PRO.setName(widget->itemText);
    }
    saveProjects();
}

void MainWindow::on_rmProBtn_clicked() {
    if (ui->projectWidget->currentItem() == NULL) {
        ui->statusbar->showMessage("No project selected", 1000);
        return;
    }
    int row = ui->projectWidget->currentRow();
    if (row != 0) {
        Organizer::Projects.removeAt(row);
        ui->projectWidget->takeItem(row);
    }
    else
        ui->statusbar->showMessage("Can't remove last remaining project -- bug to be solved", 3000);
    saveProjects();
}

void MainWindow::on_projectWidget_currentRowChanged() {
    ui->statusbar->clearMessage();
    ui->taskWidget->clear();
    QStringList items;
    for (auto &&task : CURR_PRO_TASKS)
        items.append(task.name());
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
        CURR_PRO_TASKS.append(Task(widget->itemText));
        ui->taskWidget->clear();
        QStringList items;
        for (auto &&task : CURR_PRO_TASKS)
            items.append(task.name());
        ui->taskWidget->addItems(items);
    }
    saveProjects();
}

void MainWindow::on_taskWidget_currentRowChanged(int currentRow) {
    if (ui->taskWidget->currentItem() != NULL)
        ui->statusbar->showMessage(CURR_PRO_TASKS.at(currentRow).details());
}

void MainWindow::on_renameTaskBtn_clicked() {
    if (ui->taskWidget->currentItem() == NULL) {
        ui->statusbar->showMessage("No task selected", 1000);
        return;
    }
    auto widget = new Dialog(this);
    int ret = widget->exec();
    if (ret == QDialog::Rejected)
        return;
    if (ret) {
        ui->taskWidget->currentItem()->setText(widget->itemText);
        CURR_PRO_TASKS[ui->taskWidget->currentRow()].setName(ui->taskWidget->currentItem()->text());
    }
    saveProjects();
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
    if (ui->taskWidget->currentItem() == NULL) {
        ui->statusbar->showMessage("No task selected", 1000);
        return;
    }
    int row = ui->taskWidget->currentRow();
    CURR_PRO_TASKS.removeAt(row);
    ui->taskWidget->takeItem(row);
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
