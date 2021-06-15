#ifndef PROJECT_H
#define PROJECT_H

#include "task.h"

class Project {
public:
    explicit Project(QString name);
    QString name() const { return projectName; }
    void setName(QString name) { projectName = name; }
    static QList<Task> tasks;

private:
    QString projectName;
};

#endif // PROJECT_H
