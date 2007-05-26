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
 * \file file.h
 * \version $Id$
 * \brief Functions and definitions for common file I/O operations
 */

#ifndef _FILE_H
#define _FILE_H

#include <QString>
#include "string.h"


/**  Create an empty file named <b>filename</b>. if <b>createdir</b> is true,
 * then the full path to <b>filename</b> will be created. Returns true on 
 * success, or false on error and <b>errmsg</b> will be set. */
bool touch_file(QString filename, bool createdir = false, QString *errmsg = 0);

/** Creates all directories in <b>path</b>, if they do not exist. */
bool create_path(QString path);

/** Expands <b>filename</b> if it starts with "~/". On Windows, this will
 * expand "%APPDATA%" and "%PROGRAMFILES%". If <b>filename</b> does not
 * start with a shortcut, <b>filename</b> will be returned unmodified. */
QString expand_filename(QString filename);

#endif

