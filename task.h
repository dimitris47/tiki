#ifndef TASK_H
#define TASK_H

#include <QString>

enum Priority {high, normal, low};

class Task {
public:
    explicit Task(QString name = "new task", Priority = normal);
    QString name() const { return taskName; }
    void setName(QString name) { taskName = name; }

private:
    QString taskName;
    bool isDone;
    Priority priority;
};

#endif // TASK_H
