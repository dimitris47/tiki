#ifndef ALERTS_H
#define ALERTS_H

#include <QDialog>


namespace Ui {
class Alerts;
}


class Alerts : public QDialog
{
    Q_OBJECT

public:
    explicit Alerts(QWidget *parent, QStringList alerts);
    ~Alerts();
    QStringList alrt;

private slots:
    void on_addButton_clicked();
    void on_removeButton_clicked();
    void on_buttonBox_accepted();

private:
    Ui::Alerts *ui;
};

#endif // ALERTS_H
