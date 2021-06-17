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
#define CURR_TASKS_ALL Organizer::Projects[ui->projectWidget->currentRow()].tasks
#define CURR_TASK Organizer::Projects[ui->projectWidget->currentRow()].tasks[ui->taskWidget->currentRow()]

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
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
                int priority = line.split("-->>").at(2).toInt();
                switch (priority) {
                case 0:
                    task.setPriority(high);
                    break;
                case 1:
                    task.setPriority(normal);
                    break;
                case 2:
                    task.setPriority(low);
                    break;
                }
                int status = line.split("-->>").at(1).toInt();
                task.setStatus(status);

                Organizer::Projects[i].tasks.append(task);
            }
        }
        i++;
        file.close();
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
                               (task.status() ? "1" : "0") + "-->>" +
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

    QDir dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    if (!dataDir.exists())
        dataDir.mkpath(".");
    QString currentName = dataDir.path() + '/' + ui->projectWidget->currentItem()->text() + ".txt";

    auto widget = new Dialog(this);
    int ret = widget->exec();
    if (ret == QDialog::Rejected)
        return;
    if (ret) {
        ui->projectWidget->currentItem()->setText(widget->itemText);
        CURR_PRO.setName(widget->itemText);
    }
    QFile file(currentName);
    if (!file.open(QIODevice::WriteOnly | QFile::Text)) {
        qWarning() << tr("error opening %1").arg(currentName);
        return;
    }
    file.rename(currentName, dataDir.path() + '/' + widget->itemText + ".txt");
    saveProjects();
}

void MainWindow::on_rmProBtn_clicked() {
    if (ui->projectWidget->currentItem() == NULL) {
        ui->statusbar->showMessage("No project selected", 1000);
        return;
    }

    QDir dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    if (!dataDir.exists())
        dataDir.mkpath(".");
    QString currentName = dataDir.path() + '/' + ui->projectWidget->currentItem()->text() + ".txt";

    int row = ui->projectWidget->currentRow();
    if (row != 0) {
        Organizer::Projects.removeAt(row);
        ui->projectWidget->takeItem(row);
    } else {
        ui->statusbar->showMessage("Can't remove the first project of the list -- bug to be solved", 3000);
    }

    QFile file(currentName);
    file.remove();
    saveProjects();
}

void MainWindow::on_projectWidget_currentRowChanged() {
    ui->statusbar->clearMessage();
    ui->taskWidget->clear();
    for (int i = 0; i < CURR_TASKS_ALL.count(); i++) {
        ui->taskWidget->addItem(CURR_TASKS_ALL.at(i).name());
        if (CURR_TASKS_ALL.at(i).status() == 1)
            ui->taskWidget->item(i)->setForeground(QColor(Qt::GlobalColor::gray));
    }
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
        CURR_TASKS_ALL.append(Task(widget->itemText));
        ui->taskWidget->clear();
        QStringList items;
        for (auto &&task : CURR_TASKS_ALL)
            items.append(task.name());
        ui->taskWidget->addItems(items);
    }
    saveProjects();
}

void MainWindow::on_taskWidget_currentRowChanged() {
    if (ui->taskWidget->currentItem() != NULL)
        ui->statusbar->showMessage(CURR_TASKS_ALL.at(ui->taskWidget->currentRow()).details());
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
        CURR_TASKS_ALL[ui->taskWidget->currentRow()].setName(ui->taskWidget->currentItem()->text());
    }
    saveProjects();
}

void MainWindow::on_highBtn_clicked() {
    if (CURR_TASK.priority() != high) {
        QString taskName = CURR_TASK.name();
        CURR_TASK.setPriority(high);
        CURR_TASKS_ALL.move(ui->taskWidget->currentRow(), 0);
        ui->taskWidget->takeItem(ui->taskWidget->currentRow());
        ui->taskWidget->insertItem(0, taskName);
        ui->taskWidget->setCurrentRow(0);
        saveProjects();
    }
}

void MainWindow::on_normalBtn_clicked() {
    if (CURR_TASK.priority() != normal) {
        QString taskName = CURR_TASK.name();
        int highs {0};
        for (auto &&task : CURR_TASKS_ALL)
            if (task.priority() == high)
                highs++;
        CURR_TASK.setPriority(normal);
        CURR_TASKS_ALL.move(ui->taskWidget->currentRow(), highs);
        ui->taskWidget->takeItem(ui->taskWidget->currentRow());
        ui->taskWidget->insertItem(highs, taskName);
        ui->taskWidget->setCurrentRow(highs);
        saveProjects();
    }
}

void MainWindow::on_lowBtn_clicked() {
    if (CURR_TASK.priority() != low) {
        QString taskName = CURR_TASK.name();
        int higher {-1};
        for (auto &&task : CURR_TASKS_ALL)
            if (task.priority() != low)
                higher++;
        CURR_TASK.setPriority(low);
        CURR_TASKS_ALL.move(ui->taskWidget->currentRow(), higher);
        ui->taskWidget->takeItem(ui->taskWidget->currentRow());
        ui->taskWidget->insertItem(higher, taskName);
        ui->taskWidget->setCurrentRow(higher);
        saveProjects();
    }
}

void MainWindow::on_doneBtn_clicked() {
    if (CURR_TASK.status() == 0) {
        QString taskName = CURR_TASK.name();
        CURR_TASK.setStatus(1);
        CURR_TASKS_ALL.move(ui->taskWidget->currentRow(), ui->taskWidget->count()-1);
        ui->taskWidget->takeItem(ui->taskWidget->currentRow());
        ui->taskWidget->insertItem(ui->taskWidget->count(), taskName);
        ui->taskWidget->setCurrentRow(ui->taskWidget->count()-1);
        ui->taskWidget->currentItem()->setForeground(QColor(Qt::GlobalColor::gray));
        ui->statusbar->showMessage(CURR_TASKS_ALL.at(ui->taskWidget->currentRow()).details());
        saveProjects();
    }
}

void MainWindow::on_notDoneBtn_clicked() {
    if (CURR_TASK.status() == 1) {
        CURR_TASK.setStatus(0);
        QString taskName = CURR_TASK.name();
        switch (CURR_TASK.priority()) {
        case 0:
            CURR_TASKS_ALL.move(ui->taskWidget->currentRow(), 0);
            ui->taskWidget->takeItem(ui->taskWidget->currentRow());
            ui->taskWidget->insertItem(0, taskName);
            ui->taskWidget->setCurrentRow(0);
            saveProjects();
            break;
        case 1:
            on_normalBtn_clicked();
            break;
        case 2:
            on_lowBtn_clicked();
            break;
        }
        ui->taskWidget->currentItem()->setForeground(QColor(Qt::GlobalColor::black));
        ui->statusbar->showMessage(CURR_TASKS_ALL.at(ui->taskWidget->currentRow()).details());
    }
}

void MainWindow::on_rmTaskBtn_clicked() {
    if (ui->taskWidget->currentItem() == NULL) {
        ui->statusbar->showMessage("No task selected", 1000);
        return;
    }
    int row = ui->taskWidget->currentRow();
    if (row != 0) {
        CURR_TASKS_ALL.removeAt(row);
        ui->taskWidget->takeItem(row);
        saveProjects();
    } else {
        ui->statusbar->showMessage("Can't remove the first task of the list -- bug to be solved", 3000);
    }
}

void MainWindow::readPrefs() {
    QSettings settings;
    bool isMax = settings.value("isMaximized", false).toBool();
    if (isMax) {
        showMaximized();
    } else {
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
