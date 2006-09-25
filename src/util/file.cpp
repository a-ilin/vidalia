/****************************************************************
 *  Vidalia is distributed under the following license:
 *
 *  Copyright (C) 2006,  Matt Edman, Justin Hipple
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, 
 *  Boston, MA  02110-1301, USA.
 ****************************************************************/

/** 
 * \file file.cpp
 * \version $Id$
 * \brief Functions and definitions for common file I/O operations
 */

#include <QDir>
#include <QFile>
#include "file.h"


/** Create an empty file named <b>filename</b>. if <b>createdir</b> is true,
 * then the full path to <b>filename</b> will be created. Returns true on 
 * success, or false on error and <b>errmsg</b> will be set. */
bool
touch_file(QString filename, bool createdir, QString *errmsg)
{
  /* If the file's path doesn't exist and we're supposed to create it, do that
   * now. */
  if (createdir && !create_path(QFileInfo(filename).absolutePath())) {
    return false;
  }
 
  /* Touch the file */
  QFile file(filename);
  if (!QFileInfo(filename).exists()) {
    if (!file.open(QIODevice::WriteOnly)) {
      return err(errmsg, file.errorString());
    } 
  }
  return true;
}

/** Creates all directories in <b>path</b>, if they do not exist. */
bool
create_path(QString path)
{
  QDir dir(path);
  if (!dir.exists()) {
    path = dir.absolutePath();
    if (!dir.mkpath(path)) {
      return false;
    }
  }
  return true;
}

