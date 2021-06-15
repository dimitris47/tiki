#include "project.h"
#include "organizer.h"
#include <QList>

Project::Project(QString name) {
    projectName = name;
}

QList<Task> Project::tasks = {};
