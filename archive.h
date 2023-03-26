#ifndef ARCHIVE_H
#define ARCHIVE_H

#include <QDialog>

namespace Ui {
class Archive;
}

class Archive : public QDialog
{
    Q_OBJECT

public:
    explicit Archive(QWidget *parent = nullptr);
    ~Archive();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::Archive *ui;
};

#endif // ARCHIVE_H
