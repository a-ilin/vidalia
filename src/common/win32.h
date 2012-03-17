/*
**  This file is part of Vidalia, and is subject to the license terms in the
**  LICENSE file, found in the top level directory of this distribution. If you
**  did not receive the LICENSE file with this file, you may obtain it from the
**  Vidalia source package distributed by the Vidalia Project at
**  http://www.torproject.org/projects/vidalia.html. No part of Vidalia,
**  including this file, may be copied, modified, propagated, or distributed
**  except according to the terms described in the LICENSE file.
*/

/*
** \file win32.h
** \brief Win32-specific functions
*/

#ifndef _WIN32_H
#define _WIN32_H

#include <QHash>
#include <QString>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

/** Retrieves the location of the user's %PROGRAMFILES% folder. */
QString win32_program_files_folder();

/** Retrieves the location of the user's %APPDATA% folder. */
QString win32_app_data_folder();

/** Returns value of keyName or empty QString if keyName doesn't exist */
QString win32_registry_get_key_value(QString keyLocation, QString keyName);

/** Creates and/or sets the key to the specified value */
void win32_registry_set_key_value(QString keyLocation, QString keyName, QString keyValue);

/** Removes the key from the registry if it exists */
void win32_registry_remove_key(QString keyLocation, QString keyName);

/** Returns a list of all currently active processes, including their pid
 * and exe filename. */
QHash<qint64, QString> win32_process_list();

/**
 * Close all processes started from the specified filename. Sends
 * WM_QUIT to all top-level windows. Filename should be given in
 * lowercase, and comparison is case insensitive. Note: the MSDN
 * documentation for WM_QUIT states that the message should not be
 * sent by PostMessage(). However, sending WM_CLOSE leaves Firefox
 * running, whereas WM_QUIT seems to work.
 */
void win32_end_process_by_filename(QString filename);

#endif

