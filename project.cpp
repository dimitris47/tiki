#include "project.h"
#include "organizer.h"

Project::Project(QString name) {
    projectName = name;
}

void Project::prioritySort() {
    QList<Task> newList;
    for (int i = 0; i <= 2; i++) {
        for (auto &&task : this->tasks)
            if (task.priority() == i && task.status() == 0)
                newList.append(task);
    }
    for (int i = 0; i <= 2; i++) {
        for (auto &&task : this->tasks)
            if (task.priority() == i && task.status() == 1)
                newList.append(task);
    }
    tasks = newList;
}
