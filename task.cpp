#include "task.h"
#include <QDebug>

Task::Task(QString name, bool isDone, Priority) {
    taskName = name;
    this->isDone = isDone;
    taskPriority = normal;
}

QString Task::priorityString() const {
    switch (taskPriority) {
        case high: return "high"; break;
        case normal: return "normal"; break;
        case low: return "low"; break;
    }
    return "";
}

QString Task::details() const {
    QString nameF;
    if (this->name().count() <= 16) {
        nameF = this->name();
    } else {
        nameF = this->name().remove(16, this->name().count()) + "...";
    }
    return nameF + " || " + priorityString() + " priority, " + (isDone ? "done" : "not done");
}
