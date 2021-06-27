#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dialog.h"
#include "global.h"
#include "organizer.h"
#include "project.h"
#include "task.h"
#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QFileDialog>
#include <QMessageBox>
#include <QPrinter>
#include <QStandardPaths>
#include <QTextCodec>
#include <QTextDocument>

#define CURR_PRO Organizer::Projects[ui->projectWidget->currentRow()]
#define CURR_TASKS_ALL Organizer::Projects[ui->projectWidget->currentRow()].tasks
#define CURR_TASK Organizer::Projects[ui->projectWidget->currentRow()].tasks[ui->taskWidget->currentRow()]

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    readPrefs();
    readProjects();
    ui->projectWidget->setFont(QFont("Sans-serif", 11));
    ui->taskWidget->setFont(QFont("Sans-serif", 11));
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
    for (int i = 0; i < Organizer::Projects.count(); i++) {
        ui->projectWidget->addItem(Organizer::Projects.at(i).name());
        if (Organizer::Projects.at(i).tasks.isEmpty())
            ui->projectWidget->item(i)->setForeground(QColor(Qt::GlobalColor::gray));
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
        ui->projectWidget->addItem(widget->itemText.replace(QRegularExpression("[?|:|\\|/|%|*|\"|<|>|'|']+"), "_"));
        Organizer::Projects.append(Project(widget->itemText.replace(QRegularExpression("[?|:|\\|/|%|*|\"|<|>|'|']+"), "_")));
        ui->projectWidget->setCurrentRow(ui->projectWidget->count()-1);
        ui->projectWidget->currentItem()->setForeground(QColor(Qt::GlobalColor::gray));
        saveProjects();
    }
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

    auto widget = new Dialog(this, ui->projectWidget->currentItem()->text());
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
    ui->projectWidget->takeItem(row);
    Organizer::Projects.removeAt(row);
    QFile file(currentName);
    file.remove();
    saveProjects();
}

void MainWindow::on_projectWidget_currentRowChanged() {
    ui->statusbar->clearMessage();
    ui->taskWidget->clear();
    if (ui->projectWidget->currentItem() != NULL)
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
        CURR_TASKS_ALL.append(Task(widget->itemText.replace("-->>", "-->")));
        ui->taskWidget->clear();
        CURR_PRO.prioritySort();
        QStringList items;
        for (auto &&task : CURR_TASKS_ALL)
            items.append(task.name());
        ui->taskWidget->addItems(items);
        for (int i = 0; i < ui->taskWidget->count(); i++)
            if (CURR_TASKS_ALL.at(i).status())
                ui->taskWidget->item(i)->setForeground(QColor(Qt::GlobalColor::gray));
    }
    if (ui->projectWidget->currentItem()->foreground() == QColor(Qt::GlobalColor::gray))
        ui->projectWidget->currentItem()->setForeground(QColor(Qt::GlobalColor::black));
    saveProjects();
}

void MainWindow::on_taskWidget_currentRowChanged(int currentRow) {
    if (ui->taskWidget->currentItem() != NULL)
        ui->statusbar->showMessage(CURR_TASKS_ALL.at(currentRow).details());
}

void MainWindow::on_renameTaskBtn_clicked() {
    if (ui->taskWidget->currentItem() == NULL) {
        ui->statusbar->showMessage("No task selected", 1000);
        return;
    }
    auto widget = new Dialog(this, ui->taskWidget->currentItem()->text());
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
    if (CURR_TASK.priority() != high)
        if (!CURR_TASK.status()) {
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
    QString taskName = CURR_TASK.name();
    int highs {0};
    for (auto &&task : CURR_TASKS_ALL)
        if (task.priority() == high)
            if (!task.status())
                highs++;
    CURR_TASK.setPriority(normal);
    CURR_TASKS_ALL.move(ui->taskWidget->currentRow(), highs);
    ui->taskWidget->takeItem(ui->taskWidget->currentRow());
    ui->taskWidget->insertItem(highs, taskName);
    ui->taskWidget->setCurrentRow(highs);
    saveProjects();
}

void MainWindow::on_lowBtn_clicked() {
    if (CURR_TASK.priority() != low) {
        QString taskName = CURR_TASK.name();
        int higher {-1};
        for (auto &&task : CURR_TASKS_ALL)
            if (task.priority() != low)
                if (!task.status())
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
    saveProjects();
}

void MainWindow::on_rmTaskBtn_clicked() {
    if (ui->taskWidget->currentItem() == NULL) {
        ui->statusbar->showMessage("No task selected", 1000);
        return;
    }
    int row = ui->taskWidget->currentRow();
    ui->taskWidget->takeItem(row);
    CURR_TASKS_ALL.removeAt(row);
    if (CURR_TASKS_ALL.isEmpty())
        ui->projectWidget->currentItem()->setForeground(QColor(Qt::GlobalColor::gray));
    saveProjects();
}

QString dirToWrite() {
    static QString dir;
    if (!dir.isEmpty())
        return dir;
    QStringList locations = (QStringList()
                             << QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation)
                             << QStandardPaths::standardLocations(QStandardPaths::HomeLocation));
    for (auto &&loc : locations)
        if (QFileInfo::exists(loc)) {
            dir = loc;
            return dir;
        }
    return QString();
}

void MainWindow::on_pdfBtn_clicked() {
    QString fileName = QFileDialog::getSaveFileName((QWidget* )0, "Export to PDF", dirToWrite(), "*.pdf");
    if (QFileInfo(fileName).suffix().isEmpty())
        fileName.append(".pdf");
    QPrinter printer(QPrinter::PrinterResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setPageSize(QPageSize(QPageSize::A4));
    printer.setOutputFileName(fileName);
    QStringList taskList;
    for (auto &&task : CURR_TASKS_ALL)
        taskList.append("<span>&#8226; " + task.name().replace("<", "&#60;") + "</span>");
    QTextDocument doc;
    doc.setHtml("<b>" + CURR_PRO.name() + "</b>" + "<br/><br/><br/>" + taskList.join("<br/><br/>"));
    doc.print(&printer);
    ui->statusbar->showMessage(CURR_PRO.name() + " exported to PDF", 3000);
}

void MainWindow::on_printBtn_clicked() {

}

void MainWindow::on_infoButton_clicked() {
    QMessageBox::about(this, tr("Program Info"),
                       (QApplication::applicationName() + " " + QApplication::applicationVersion() + "<br/><br/>" +
                        APPINFO));
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
