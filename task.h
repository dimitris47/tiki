#ifndef TASK_H
#define TASK_H

#include <QString>

enum Priority {high, normal, low};

class Task {
public:
    explicit Task(QString name = "new task", Priority = normal);
    QString getTaskName() const { return taskName; }

private:
    QString taskName;
    bool isDone;
    Priority priority;
};

#endif // TASK_H
