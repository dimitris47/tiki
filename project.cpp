#include "project.h"
#include "organizer.h"

Project::Project(QString name) {
    projectName = name;
}

void Project::prioritySort() {
    QList<Task> newList;
    for (auto &&task : this->tasks)
        if (task.priority() == 0)
            newList.append(task);
    for (auto &&task : this->tasks)
        if (task.priority() == 1)
            newList.append(task);
    for (auto &&task : this->tasks)
        if (task.priority() == 2)
            newList.append(task);
    tasks = newList;
}
