#include "task.h"

Task::Task(QString name, bool isDone, Priority) {
    taskName = name;
    this->isDone = isDone;
    taskPriority = normal;
}
