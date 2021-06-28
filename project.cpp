#include "project.h"
#include "organizer.h"

Project::Project(QString name) {
    projectName = name;
}

void Project::prioritySort() {
    QList<Task> newList;
    for (int st = 0; st <= 1; st++)
        for (int i = 0; i <= 2; i++)
            for (auto &&task : this->tasks)
                if (task.priority() == i && task.status() == st)
                    newList.append(task);
    tasks = newList;
}
