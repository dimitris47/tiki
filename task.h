#ifndef TASK_H
#define TASK_H

#include <QString>

enum Priority { HIGH, NORMAL, LOW };


class Task
{
public:
    explicit Task(QString name = "new task", bool isDone = false, Priority = NORMAL);
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
