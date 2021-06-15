#ifndef PROJECT_H
#define PROJECT_H

#include "task.h"

class Project {
public:
    explicit Project(QString name);
    static QList<Task> tasks;
    void setName(QString name) {
        projectName = name;
    }
    QString name() const {
        return projectName;
    }

private:
    QString projectName;
};

#endif // PROJECT_H
