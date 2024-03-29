#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>

namespace Ui {
class Dialog;
}


class Dialog : public QDialog {
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent=nullptr, QString name="", QString kind="new");
    ~Dialog();
    QString itemText;

private slots:
    void on_buttonBox_accepted();

private:
    Ui::Dialog *ui;
};

#endif // DIALOG_H
