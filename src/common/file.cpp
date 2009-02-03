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
** \file file.cpp
** \version $Id$
** \brief Functions and definitions for common file I/O operations
*/

#include <QDir>
#include <QFile>
#include "stringutil.h"
#include "file.h"

#if defined(Q_OS_WIN32)
#include "win32.h"
#endif


/** Create an empty file named <b>filename</b>. if <b>createdir</b> is true,
 * then the full path to <b>filename</b> will be created. Returns true on 
 * success, or false on error and <b>errmsg</b> will be set. */
bool
touch_file(QString filename, bool createdir, QString *errmsg)
{
  /* Expand the file's path if it starts with a shortcut, like "~/" or
   * "%APPDATA%" */
  filename = expand_filename(filename);
    
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

/** Recursively copy the contents of one directory to another. The
 * destination must already exist. Returns true on success, and false
 * otherwise. */
bool
copy_dir(QString source, QString dest)
{
  /* Source and destination as QDir's */
  QDir src(source);
  QDir dst(dest);
  
  /* Get contents of the directory */
  QFileInfoList contents = src.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);

  /* Copy each entry in src to dst */
  foreach (QFileInfo fileInfo, contents) {
    /* Get absolute path of source and destination */
    QString fileName = fileInfo.fileName();
    QString srcFilePath = src.absoluteFilePath(fileName);
    QString dstFilePath = dst.absoluteFilePath(fileName);

    if (fileInfo.isDir()) {
      /* This is a directory, make it and recurse */
      if (!dst.mkdir(fileName))
	return false;
      if (!copy_dir(srcFilePath, dstFilePath))
	return false;
    } else if (fileInfo.isFile()) {
      /* This is a file, copy it */
      if (!QFile::copy(srcFilePath, dstFilePath))
	return false;
    } 
    /* Ignore special files (e.g. symlinks, devices) */

  }
  return true;
}

/** Expands <b>filename</b> if it starts with "~/". On Windows, this will
 * expand "%APPDATA%" and "%PROGRAMFILES%". If <b>filename</b> does not
 * start with a shortcut, <b>filename</b> will be returned unmodified. */
QString
expand_filename(QString filename)
{
#if defined(Q_OS_WIN32)
  if (filename.startsWith("%APPDATA%\\") ||
      filename.startsWith("%APPDATA%/"))
    return filename.replace(0, 9, win32_app_data_folder());
    
  if (filename.startsWith("%PROGRAMFILES%\\") ||
      filename.startsWith("%PROGRAMFILES%/"))
    return filename.replace(0, 14, win32_program_files_folder());
#else
  if (filename.startsWith("~/"))
    return filename.replace(0, 1, QDir::homePath());
#endif
  return filename;
}

