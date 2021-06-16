#include "task.h"

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
    return this->name() + ": " + priorityString() + " priority, " + (isDone ? "done" : "not done");
}
