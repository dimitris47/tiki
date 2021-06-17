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
