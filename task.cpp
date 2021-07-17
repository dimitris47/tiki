/**
 * Copyright 2021 Dimitris Psathas <dimitrisinbox@gmail.com>
 *
 * This file is part of Tiki.
 *
 * Tiki is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License  as  published by  the  Free Software
 * Foundation,  either version 3 of the License,  or (at your option)  any later
 * version.
 *
 * Tiki is distributed in the hope that it will be useful,  but  WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.  See the  GNU General Public License  for more details.
 *
 * You should have received a copy of the  GNU General Public License along with
 * Tiki. If not, see <http://www.gnu.org/licenses/>.
 */


#include "task.h"
#include <QDebug>

Task::Task(QString name, bool isDone, Priority) {
    taskName = name;
    this->isDone = isDone;
    taskPriority = normal;
}

QString Task::priorityString() const {
    switch (taskPriority) {
        case high: return "high"; break;
        case normal: return "normal"; break;
        case low: return "low"; break;
    }
    return "";
}

QString Task::details() const {
    QString nameF;
    if (this->name().count() <= 32) {
        nameF = this->name();
    } else {
        nameF = this->name().remove(32, this->name().count()) + "...";
    }
    return nameF + " || " + priorityString() + " priority, " + (isDone ? "done" : "not done");
}
