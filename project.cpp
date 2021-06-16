#include "project.h"
#include "organizer.h"

Project::Project(QString name) {
    projectName = name;
}

QStringList Project::toString() {
    QStringList taskList;
    for (auto &&task : tasks)
        taskList.append(task.name());
    return taskList;
}

void Project::sortTasks() {
    QVector<Task> highList;
    QVector<Task> normalList;
    QVector<Task> lowList;

    for (auto &&task : tasks)
        switch (task.priority()) {
        case high:
            highList.append(task);
        case normal:
            normalList.append(task);
        case low:
            lowList.append(task);
        }
    tasks.clear();
    for (auto && task : highList)
        tasks.append(task);
    for (auto && task : normalList)
        tasks.append(task);
    for (auto &&task : lowList)
        tasks.append(task);
}
