/*
**  This file is part of Vidalia, and is subject to the license terms in the
**  LICENSE file, found in the top level directory of this distribution. If you
**  did not receive the LICENSE file with this file, you may obtain it from the
**  Vidalia source package distributed by the Vidalia Project at
**  http://www.vidalia-project.net/. No part of Vidalia, including this file,
**  may be copied, modified, propagated, or distributed except according to the
**  terms described in the LICENSE file.
*/

/*
** \file file.h
** \version $Id$
** \brief Functions and definitions for common file I/O operations
*/

#ifndef _FILE_H
#define _FILE_H

#include <QString>


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
    
/** Recursively copy the contents of one directory to another. The
 * destination must already exist. Returns true on success, and false
 * otherwise. */
bool copy_dir(QString source, QString dest);

#endif

