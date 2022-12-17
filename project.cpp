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


#include "project.h"
#include "organizer.h"


Project::Project(QString name)
{
    projectName = name;
    isModified = false;
}


void Project::prioritySort()
{
    QList<Task> newList;
    for (int st = 0; st <= 1; st++)
        for (int i = 0; i <= 2; i++)
            for (auto &&task : this->tasks)
                if (task.priority() == i && task.status() == st)
                    newList.append(task);
    tasks = newList;
}
