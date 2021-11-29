#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "project.h"
#include <QCloseEvent>
#include <QMainWindow>
#include <QSettings>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void savePrefs();
    void closeEvent(QCloseEvent *event) override;

private slots:
    bool allDone(const Project &project);
    void sortProjects();
    void sortTasksByPriority();
    void on_sortProBtn_clicked();
    void on_addProBtn_clicked();
    void on_renameProBtn_clicked();
    void on_projectWidget_itemDoubleClicked();
    void on_rmProBtn_clicked();
    void on_projectWidget_currentRowChanged();
    void on_addTaskBtn_clicked();
    void on_taskWidget_currentRowChanged(int currentRow);
    void on_renameTaskBtn_clicked();
    void on_taskWidget_itemDoubleClicked();
    void on_highBtn_clicked();
    void on_normalBtn_clicked();
    void on_lowBtn_clicked();
    void on_doneBtn_clicked();
    void on_notDoneBtn_clicked();
    void on_sortTasksBtn_clicked();
    void on_rmTaskBtn_clicked();
    void on_pdfBtn_clicked();
    void on_printBtn_clicked();
    void on_fontBtn_clicked();
    void on_infoButton_clicked();
    QString stringToPrint();

private:
    Ui::MainWindow *ui;
    void readProjects();
    void saveProjects();
    void readPrefs();
};

#endif // MAINWINDOW_H
