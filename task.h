#ifndef TASK_H
#define TASK_H

#include <QString>

enum Priority {high, normal, low};

class Task {
public:
    explicit Task(QString name = "new task", bool isDone = false, Priority = normal);
    QString name() const { return taskName; }
    void setName(QString name) { taskName = name; }
    Priority priority() const { return taskPriority; };
    void setPriority(Priority priority) { taskPriority = priority; };
    bool status() const { return isDone; };
    void setStatus(bool status) { isDone = status; };
    QString priorityString() const;
    QString details() const;

private:
    QString taskName;
    bool isDone;
    Priority taskPriority;
};

#endif // TASK_H
