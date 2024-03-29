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


#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "alerts.h"
#include "dialog.h"
#include "global.h"
#include "organizer.h"
#include "project.h"
#include "task.h"

#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QFileDialog>
#include <QFontDialog>
#include <QMessageBox>
#include <QPrintDialog>
#include <QPrinter>
#include <QStandardPaths>
#include <QTextCodec>
#include <QTextDocument>

#define CURR_PRO Organizer::Projects[ui->projectWidget->currentRow()]
#define CURR_TASKS_ALL Organizer::Projects[ui->projectWidget->currentRow()].tasks
#define CURR_TASK Organizer::Projects[ui->projectWidget->currentRow()].tasks[ui->taskWidget->currentRow()]


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    readProjects();
    readPrefs();
}


MainWindow::~MainWindow()
{
    delete ui;
}


bool compareProjects(const Project &pro1, const Project &pro2)
{
    return pro1.name().toLower() < pro2.name().toLower();
}


bool compareTasks(const Task &task1, const Task &task2)
{
    return task1.name().toLower() < task2.name().toLower();
}


bool comparePriorities(const Task &task1, const Task &task2)
{
    return task1.priority() < task2.priority();
}


void MainWindow::sortProjects()
{
    ui->projectWidget->clear();
    std::sort(Organizer::Projects.begin(), Organizer::Projects.end(), compareProjects);
    for (int i=0; i<Organizer::Projects.count(); i++) {
        ui->projectWidget->addItem(Organizer::Projects.at(i).name());
        if (Organizer::Projects.at(i).tasks.isEmpty() || allDone(Organizer::Projects.at(i))) {
            ui->projectWidget->item(i)->setForeground(Qt::GlobalColor::gray);
        }
    }
}


void MainWindow::sortTasksByPriority()
{
    std::sort(CURR_TASKS_ALL.begin(), CURR_TASKS_ALL.end(), comparePriorities);

    for (int i=CURR_TASKS_ALL.count()-1; i>=0; i--) {
        Task task = CURR_TASKS_ALL.at(i);
        if (task.status()) {
            CURR_TASKS_ALL.takeAt(i);
            CURR_TASKS_ALL.append(task);
        }
    }

    ui->taskWidget->clear();
    for (auto &&task : CURR_TASKS_ALL) {
        ui->taskWidget->addItem(task.name());
    }
    for (int i=0; i<CURR_TASKS_ALL.count(); i++) {
        if (CURR_TASKS_ALL.at(i).status()) {
            ui->taskWidget->item(i)->setForeground(Qt::GlobalColor::gray);
        }
    }

    CURR_PRO.isModified = true;
    saveProjects();
}


bool MainWindow::allDone(const Project &project)
{
    auto pred = [](Task task) { return task.status() == 1; };
    return std::all_of(project.tasks.begin(), project.tasks.end(), pred);
}


void MainWindow::readProjects()
{
    QDir dataDir = QDir(QStandardPaths::standardLocations(QStandardPaths::AppDataLocation).at(0));
    QDirIterator it(dataDir.path(), QDir::Files, QDirIterator::Subdirectories);
    int i = 0;
    while (it.hasNext()) {
        QFile file(it.next());
        Project project(file.fileName().remove(dataDir.path() + '/').remove(".txt"));
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
                    task.setPriority(HIGH);
                    break;
                case 1:
                    task.setPriority(NORMAL);
                    break;
                case 2:
                    task.setPriority(LOW);
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
    sortProjects();
}


void MainWindow::showCounts()
{
    ui->proCountLbl->setText(QString::number(ui->projectWidget->count()) + " projects");

    if (ui->projectWidget->currentRow() >= 0) {
        int notDone = 0;
        int done = 0;
        for (auto &&task : CURR_TASKS_ALL) {
            if (!task.status()) {
                notDone++;
            } else {
                done++;
            }
        }
        ui->tasksCountLbl->setText(QString::number(CURR_TASKS_ALL.count()) + " tasks, " +
                                   QString::number(notDone) + " not done, " +
                                   QString::number(done) + " done");
    } else {
        ui->tasksCountLbl->clear();
    }
}


void MainWindow::on_sortTasksBtn_clicked()
{
    if (ui->projectWidget->currentItem() == NULL) {
        ui->statusbar->showMessage("No project selected", 1000);
        return;
    }
    for (auto &&task : CURR_TASKS_ALL) {
        task.setPriority(NORMAL);
        task.setStatus(0);
    }
    CURR_PRO.isModified = true;
    std::sort(CURR_TASKS_ALL.begin(), CURR_TASKS_ALL.end(), compareTasks);
    on_projectWidget_currentRowChanged();
    saveProjects();
}


void MainWindow::saveProjects()
{
    for (auto &&project : Organizer::Projects) {
        if (project.isModified) {
            QString projectData;
            for (auto &&task : project.tasks) {
                projectData.append(task.name() + "-->>" + (task.status() ? "1" : "0") + "-->>" +
                                   QString::number(task.priority()) + '\n');
            }
            QDir dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
            if (!dataDir.exists()) {
                dataDir.mkpath(".");
            }
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
}


void MainWindow::on_addProBtn_clicked()
{
    auto widget = new Dialog(this, "", "New Project");
    int ret = widget->exec();
    if (ret == QDialog::Rejected) {
        return;
    }
    if (ret) {
        static QRegularExpression re = QRegularExpression("[.|~|?|:|\\|/|%|*|\"|<|>|'|'|'\n']+");
        const QString projectTitle = widget->itemText.replace(re, "_");
        QStringList projectTitles;
        for (auto &&project : Organizer::Projects) {
            projectTitles.append(project.name().toLower());
        }
        if (!projectTitles.contains(projectTitle.toLower())) {
            ui->projectWidget->addItem(projectTitle);
            Organizer::Projects.append(Project(projectTitle));
            for (auto &&project : Organizer::Projects) {
                project.isModified = true;
            }
            showCounts();
            sortProjects();
            ui->projectWidget->setCurrentItem(ui->projectWidget->findItems(projectTitle, Qt::MatchFlag()).at(0));
            saveProjects();
        } else {
            ui->statusbar->showMessage("A project with this name already exists", 6000);
        }
    }
}


QString writableDir() {
    static QString dir;
    if (!dir.isEmpty()) {
        return dir;
    }
    QStringList locations = (QStringList()
                             << QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation)
                             << QStandardPaths::standardLocations(QStandardPaths::HomeLocation));
    for (auto &&loc : locations) {
        if (QFileInfo::exists(loc)) {
            dir = loc;
            return dir;
        }
    }
    return QString();
}


void MainWindow::on_addFromFileBtn_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open file", writableDir(), QString("All files (*.*)"));
    if (fileName.isEmpty()) {
        return;
    }

    QFile data(fileName);
    if (!data.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "error opening " << fileName;
        return;
    }

    QStringList path = fileName.split('/');
    QStringList name = path.at(path.length()-1).split('.');
    QString projectName = name.at(name.length()-2);

    static QRegularExpression re = QRegularExpression("[.|~|?|:|\\|/|%|*|\"|<|>|'|'|'\n']+");
    const QString projectTitle = projectName.replace(re, "_");
    QStringList projectTitles;
    for (auto &&project : Organizer::Projects) {
        projectTitles.append(project.name().toLower());
    }

    if (!projectTitles.contains(projectTitle.toLower())) {
        ui->projectWidget->addItem(projectTitle);
        Organizer::Projects.append(Project(projectTitle));
        for (auto &&project : Organizer::Projects) {
            project.isModified = true;
        }
        showCounts();
        sortProjects();
        ui->projectWidget->setCurrentItem(ui->projectWidget->findItems(projectTitle, Qt::MatchFlag()).at(0));
        saveProjects();

        QTextStream reader(&data);
        reader.setCodec(QTextCodec::codecForName("UTF-8"));
        while (!reader.atEnd()) {
            QString line = reader.readLine();
            if (line.contains("-->>")) {
                ui->statusbar->showMessage("'-->> is reserved and has been replaced by '-->'", 3000);
            }
            QString taskText = line.replace("-->>", "-->").replace('\n', '_');
            QStringList taskNames;
            for (auto &&task : CURR_TASKS_ALL) {
                taskNames.append(task.name());
            }
            if (!taskNames.contains(taskText)) {
                CURR_TASKS_ALL.append(Task(taskText));
                ui->taskWidget->clear();
                CURR_PRO.prioritySort();
                QStringList items;
                for (auto &&task : CURR_TASKS_ALL) {
                    items.append(task.name());
                }
                ui->taskWidget->addItems(items);
                for (int i=0; i<ui->taskWidget->count(); i++) {
                    if (CURR_TASKS_ALL.at(i).status()) {
                        ui->taskWidget->item(i)->setForeground(Qt::GlobalColor::gray);
                    }
                }
                CURR_PRO.isModified = true;
                saveProjects();
            } else {
                ui->statusbar->showMessage("A task with this name already exists", 3000);
            }
        }
        showCounts();
    } else {
        ui->statusbar->showMessage("A project with this name already exists", 6000);
    }
}


void MainWindow::on_renameProBtn_clicked()
{
    if (ui->projectWidget->currentItem() == NULL) {
        ui->statusbar->showMessage("No project selected", 1000);
        return;
    }

    QDir dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    if (!dataDir.exists()) {
        dataDir.mkpath(".");
    }
    const QString curr = ui->projectWidget->currentItem()->text();
    const QString currentName = dataDir.path() + '/' + curr + ".txt";

    auto widget = new Dialog(this, ui->projectWidget->currentItem()->text(), "Edit Project Title");
    int ret = widget->exec();
    if (ret == QDialog::Rejected) {
        return;
    }
    if (ret) {
        static QRegularExpression re = QRegularExpression("[.|~|?|:|\\|/|%|*|\"|<|>|'|'|'\n']+");
        const QString newTitle = widget->itemText.replace(re, "_");
        QStringList projectNames;
        for (auto &&project : Organizer::Projects) {
            projectNames.append(project.name().toLower());
        }
        if (!projectNames.contains(newTitle.toLower()) || curr.toLower() == newTitle.toLower()) {
            ui->projectWidget->currentItem()->setText(newTitle);
            CURR_PRO.setName(newTitle);
            QFile file(currentName);
            if (!file.open(QIODevice::WriteOnly | QFile::Text)) {
                qWarning() << tr("error opening %1").arg(currentName);
                return;
            }
            file.rename(currentName, dataDir.path() + '/' + widget->itemText + ".txt");
            if (file.exists()) {
                file.remove();
            }
            CURR_PRO.isModified = true;
            sortProjects();
            ui->projectWidget->setCurrentItem(ui->projectWidget->findItems(newTitle, Qt::MatchFlag()).at(0));
            saveProjects();
        } else {
            ui->statusbar->showMessage("A project with this name already exists", 6000);
        }
    }
}


void MainWindow::on_projectWidget_itemDoubleClicked()
{
    on_renameProBtn_clicked();
}


void MainWindow::on_rmProBtn_clicked()
{
    if (ui->projectWidget->currentItem() == NULL) {
        ui->statusbar->showMessage("No project selected", 1000);
        return;
    }

    QMessageBox box(QMessageBox::Warning, "Deleting project",
                    tr("Are you sure you want to delete this project?"),
                    QMessageBox::Yes | QMessageBox::Cancel,
                    this);
    if (box.exec() == QMessageBox::Yes) {
        QDir dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        if (!dataDir.exists()) {
            dataDir.mkpath(".");
        }
        QString currentName = dataDir.path() + '/' + ui->projectWidget->currentItem()->text() + ".txt";
        int row = ui->projectWidget->currentRow();
        ui->projectWidget->takeItem(row);
        Organizer::Projects.removeAt(row);
        QFile file(currentName);
        file.remove();
        CURR_PRO.isModified = true;
        saveProjects();
    }
    showCounts();
}


void MainWindow::on_projectWidget_currentRowChanged()
{
    ui->statusbar->clearMessage();
    ui->taskWidget->clear();
    if (ui->projectWidget->currentItem() != NULL) {
        for (int i=0; i<CURR_TASKS_ALL.count(); i++) {
            ui->taskWidget->addItem(CURR_TASKS_ALL.at(i).name());
            if (CURR_TASKS_ALL.at(i).status() == 1) {
                ui->taskWidget->item(i)->setForeground(Qt::GlobalColor::gray);
            }
            ui->taskWidget->item(i)->setToolTip(ui->taskWidget->item(i)->text());
        }
    }
    showCounts();
}


void MainWindow::on_addTaskBtn_clicked()
{
    if (ui->projectWidget->currentItem() == NULL) {
        ui->statusbar->showMessage("No project selected", 1000);
        return;
    }
    auto widget = new Dialog(this, "", "New Task");
    int ret = widget->exec();
    if (ret == QDialog::Rejected) {
        return;
    }
    if (ret) {
        if (widget->itemText.contains("-->>")) {
            ui->statusbar->showMessage("'-->> is reserved and has been replaced by '-->'", 3000);
        }
        QString taskText = widget->itemText.replace("-->>", "-->").replace('\n', '_');
        QStringList taskNames;
        for (auto &&task : CURR_TASKS_ALL) {
            taskNames.append(task.name());
        }
        if (!taskNames.contains(taskText)) {
            CURR_TASKS_ALL.append(Task(taskText));
            ui->taskWidget->clear();
            CURR_PRO.prioritySort();
            QStringList items;
            for (auto &&task : CURR_TASKS_ALL) {
                items.append(task.name());
            }
            ui->taskWidget->addItems(items);
            for (int i=0; i<ui->taskWidget->count(); i++) {
                if (CURR_TASKS_ALL.at(i).status()) {
                    ui->taskWidget->item(i)->setForeground(Qt::GlobalColor::gray);
                } else {
                    ui->projectWidget->currentItem()->setForeground(Qt::GlobalColor::darkCyan);
                }
            }
            CURR_PRO.isModified = true;
            saveProjects();
        } else {
            ui->statusbar->showMessage("A task with this name already exists", 3000);
        }
    }
    showCounts();
}


void MainWindow::on_taskWidget_currentRowChanged(int currentRow)
{
    if (ui->taskWidget->currentItem() != NULL) {
        ui->statusbar->showMessage(CURR_TASKS_ALL.at(currentRow).details());
    } else {
        ui->statusbar->clearMessage();
    }
}


void MainWindow::on_renameTaskBtn_clicked()
{
    if (ui->taskWidget->currentItem() == NULL) {
        ui->statusbar->showMessage("No task selected", 1000);
        return;
    }
    auto widget = new Dialog(this, ui->taskWidget->currentItem()->text(), "Edit Task Name");
    int ret = widget->exec();
    if (ret == QDialog::Rejected) {
        return;
    }
    if (ret) {
        if (widget->itemText.contains("-->>")) {
            ui->statusbar->showMessage("'-->> is reserved and has been replaced by '-->'", 3000);
        }
        QString newTask = widget->itemText.replace("-->>", "-->").replace('\n', '_');
        QStringList tasksNames;
        for (auto &&task : CURR_TASKS_ALL) {
            tasksNames.append(task.name());
        }
        if (!tasksNames.contains(newTask)) {
            ui->taskWidget->currentItem()->setText(widget->itemText);
            CURR_TASKS_ALL[ui->taskWidget->currentRow()].setName(ui->taskWidget->currentItem()->text());
        } else {
            ui->statusbar->showMessage("A task with this name already exists", 3000);
        }
    }
    CURR_PRO.isModified = true;
    saveProjects();
}


void MainWindow::on_taskWidget_itemDoubleClicked()
{
    on_renameTaskBtn_clicked();
}


void MainWindow::on_highBtn_clicked()
{
    if (ui->taskWidget->currentItem() == NULL) {
        ui->statusbar->showMessage("No task selected", 1000);
        return;
    }
    if (CURR_TASK.priority() != HIGH) {
        CURR_TASK.setPriority(HIGH);
        sortTasksByPriority();
    }
}


void MainWindow::on_normalBtn_clicked()
{
    if (ui->taskWidget->currentItem() == NULL) {
        ui->statusbar->showMessage("No task selected", 1000);
        return;
    }
    if (CURR_TASK.priority() != NORMAL) {
        CURR_TASK.setPriority(NORMAL);
        sortTasksByPriority();
    }
}


void MainWindow::on_lowBtn_clicked()
{
    if (ui->taskWidget->currentItem() == NULL) {
        ui->statusbar->showMessage("No task selected", 1000);
        return;
    }
    if (CURR_TASK.priority() != LOW) {
        CURR_TASK.setPriority(LOW);
        sortTasksByPriority();
    }
}


void MainWindow::on_doneBtn_clicked()
{
    if (ui->taskWidget->currentItem() == NULL) {
        ui->statusbar->showMessage("No task selected", 1000);
        return;
    }
    if (CURR_TASK.status() == 0) {
        QString taskName = CURR_TASK.name();
        CURR_TASK.setStatus(1);
        CURR_TASKS_ALL.move(ui->taskWidget->currentRow(), ui->taskWidget->count()-1);
        int row = ui->taskWidget->currentRow();
        ui->taskWidget->takeItem(row);
        ui->taskWidget->insertItem(ui->taskWidget->count(), taskName);
        ui->taskWidget->setCurrentRow(ui->taskWidget->count()-1);
        ui->taskWidget->currentItem()->setForeground(Qt::GlobalColor::gray);
        ui->taskWidget->setCurrentRow(row);
        ui->statusbar->showMessage(CURR_TASKS_ALL.at(ui->taskWidget->currentRow()).details());
        CURR_PRO.isModified = true;
        saveProjects();
    }
    if (allDone(CURR_PRO)) {
        ui->projectWidget->currentItem()->setForeground(Qt::GlobalColor::gray);
    }
    showCounts();
}


void MainWindow::on_notDoneBtn_clicked()
{
    if (ui->taskWidget->currentItem() == NULL) {
        ui->statusbar->showMessage("No task selected", 1000);
        return;
    }
    if (CURR_TASK.status() == 1) {
        CURR_TASK.setStatus(0);
    }
    sortTasksByPriority();
    ui->projectWidget->currentItem()->setForeground(Qt::GlobalColor::darkCyan);
    showCounts();
}


void MainWindow::on_rmTaskBtn_clicked()
{
    if (ui->taskWidget->currentItem() == NULL) {
        ui->statusbar->showMessage("No task selected", 1000);
        return;
    }
    int row = ui->taskWidget->currentRow();
    ui->taskWidget->takeItem(row);
    CURR_TASKS_ALL.removeAt(row);
    if (CURR_TASKS_ALL.isEmpty()) {
        ui->projectWidget->currentItem()->setForeground(Qt::GlobalColor::gray);
    }
    CURR_PRO.isModified = true;
    saveProjects();
    showCounts();
}


QString dirToWrite()
{
    static QString dir;
    if (!dir.isEmpty()) {
        return dir;
    }
    QStringList locations = (QStringList()
                             << QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation)
                             << QStandardPaths::standardLocations(QStandardPaths::HomeLocation)
                             << QStandardPaths::standardLocations(QStandardPaths::DesktopLocation));
    for (auto &&loc : locations) {
        if (QFileInfo::exists(loc)) {
            dir = loc;
            return dir;
        }
    }
    return QString();
}


const QString sp = "&#160;";
const QString unchecked = "&#9744;";
const QString checked = "&#9745;";


QString MainWindow::stringToPrint()
{
    QStringList taskList;
    for (auto &&task : CURR_TASKS_ALL) {
        if (!task.status()) {
            taskList.append("<span><div style='line-height:130%;'>" + unchecked + sp.repeated(3) + task.name().replace("<", "&#60;") + "</div></span>");
        } else {
            taskList.append("<s><div style='color:Qt::GlobalColor::gray;'>" + task.name().replace("<", "&#60;") + "</div></s>");
        }
    }
    QString toPrint;
    toPrint.append("<h2>" + CURR_PRO.name() + "</h2>" + br + taskList.join(""));
    return toPrint;
}


void MainWindow::on_pdfBtn_clicked()
{
    if (ui->projectWidget->currentItem() == NULL) {
        ui->statusbar->showMessage("No project selected", 1000);
        return;
    }
    QString fileName = QFileDialog::getSaveFileName(
                (QWidget *) 0, "Export to PDF", dirToWrite() + "/" + CURR_PRO.name(), "*.pdf");
    if (fileName.isEmpty()) {
        return;
    }
    if (QFileInfo(fileName).suffix().isEmpty()) {
        fileName.append(".pdf");
    }
    QPrinter printer(QPrinter::PrinterResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setPageSize(QPageSize(QPageSize::A4));
    printer.setOutputFileName(fileName);
    QTextDocument doc;
    doc.setHtml(stringToPrint());
    doc.print(&printer);
    ui->statusbar->showMessage(CURR_PRO.name() + " exported to PDF", 3000);
}


void MainWindow::on_printBtn_clicked()
{
    if (ui->projectWidget->currentItem() == NULL) {
        ui->statusbar->showMessage("No project selected", 1000);
        return;
    }
    QPrinter printer(QPrinter::HighResolution);
    QPrintDialog PrintDialog(&printer, this);
    if (PrintDialog.exec()) {
        printer.setFullPage(true);
        QTextDocument TextDocument;
        TextDocument.setHtml(stringToPrint());
        TextDocument.print(&printer);
        ui->statusbar->showMessage(CURR_PRO.name() + " sent to printer", 3000);
    }
}


void MainWindow::on_expTxtBtn_clicked()
{
    QStringList tasksToExport = {};
    for (int i=0; i<CURR_PRO.tasks.count(); i++) {
        tasksToExport.append(ui->taskWidget->item(i)->text());
    }
    QString fileName = QFileDialog::getSaveFileName(
        (QWidget *) 0, "Export to Text File", dirToWrite() + "/" + CURR_PRO.name(), "*.txt");
    if (fileName.isEmpty()) {
        return;
    }
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QFile::Text)) {
        qWarning() << tr("error opening %1").arg(fileName);
        return;
    }
    QTextStream data(&file);
    data.setCodec(QTextCodec::codecForName("UTF-8"));
    data.setIntegerBase(10);
    data << tasksToExport.join('\n');
    file.close();
}


void MainWindow::on_alertBtn_clicked()
{
    QSettings settings;
    QStringList alrt;
    QString alerts = settings.value("alerts", "").toString();
    if (alerts != "") {
        alrt = alerts.split("&&");
    }
    auto widget = new Alerts(this, alrt);
    int ret = widget->exec();
    if (ret == QDialog::Rejected) {
        return;
    }
    if (ret) {
        settings.setValue("alerts", widget->alrt.join("&&"));
        settings.sync();
    }
}


void MainWindow::on_fontBtn_clicked()
{
    QApplication::setFont(QFontDialog::getFont(0, QApplication::font()));
}


void MainWindow::on_infoButton_clicked()
{
    QMessageBox::about(this, tr("Program Info"),
                       (QApplication::applicationName() + " " + QApplication::applicationVersion() + br.repeated(2) +
                        APPINFO));
}


void MainWindow::readPrefs()
{
    QSettings settings;

    bool isMax = settings.value("isMaximized", false).toBool();
    if (isMax) {
        showMaximized();
    } else {
        const QByteArray geometry = settings.value("geometry", QByteArray()).toByteArray();
        restoreGeometry(geometry);
    }

    QString alerts = settings.value("alerts", "").toString();
    if (alerts != "") {
        alerts.replace("&&", "\n");
        QMessageBox::warning(this, tr("Alerts"), alerts);
    }

    const int selPro = settings.value("selectedProject", 0).toInt();
    ui->projectWidget->setCurrentRow(selPro);

    const QString f = settings.value("font", QFont()).toString();
    const int s = settings.value("size", 10).toInt();
    const QFont font(f, s);
    QApplication::setFont(font);
}


void MainWindow::savePrefs()
{
    QSettings settings;

    settings.setValue("isMaximized", isMaximized());
    if (!isMaximized()) {
        settings.setValue("geometry", saveGeometry());
    }

    settings.setValue("selectedProject", ui->projectWidget->currentRow());

    settings.setValue("font", QApplication::font().toString());
    settings.setValue("size", QApplication::font().pointSize());

    settings.sync();
}


void MainWindow::closeEvent(QCloseEvent *event)
{
    savePrefs();
    event->accept();
}
