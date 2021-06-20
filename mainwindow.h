#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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
    void on_addProBtn_clicked();
    void on_renameProBtn_clicked();
    void on_rmProBtn_clicked();
    void on_projectWidget_currentRowChanged();
    void on_addTaskBtn_clicked();
    void on_taskWidget_currentRowChanged(int currentRow);
    void on_renameTaskBtn_clicked();
    void on_highBtn_clicked();
    void on_normalBtn_clicked();
    void on_lowBtn_clicked();
    void on_doneBtn_clicked();
    void on_notDoneBtn_clicked();
    void on_rmTaskBtn_clicked();
    void on_infoButton_clicked();

private:
    Ui::MainWindow *ui;
    void readProjects();
    void saveProjects();
    void readPrefs();
};

#endif // MAINWINDOW_H
