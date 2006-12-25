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
 * \file win32.cpp
 * \version $Id: string.h 700 2006-04-15 17:11:09Z edmanm $ 
 * \brief Win32-specific functions
 */

#include "win32.h"
#include <tlhelp32.h>
#include <shlobj.h>
#include <QDir>


/** Finds the location of the "special" Windows folder using the given CSIDL
 * value. If the folder cannot be found, the given default path is used. */
QString
win32_get_folder_location(int folder, QString defaultPath)
{
  TCHAR path[MAX_PATH+1];
  LPITEMIDLIST idl;
  IMalloc *m;
  HRESULT result;

  /* Find the location of %PROGRAMFILES% */
  if (SUCCEEDED(SHGetSpecialFolderLocation(NULL, folder, &idl))) {
    /* Get the path from the IDL */
    result = SHGetPathFromIDList(idl, path);
    SHGetMalloc(&m);
    if (m) {
      m->Release();
    }
    if (SUCCEEDED(result)) {
      QT_WA(return QString::fromUtf16((const ushort *)path);,
            return QString::fromLocal8Bit((char *)path);)
    }
  }
  return defaultPath;
}

/** Gets the location of the user's %PROGRAMFILES% folder. */
QString
win32_program_files_folder()
{
  return win32_get_folder_location(
     CSIDL_PROGRAM_FILES, QDir::rootPath() + "\\Program Files");
}

/** Gets the location of the user's %APPDATA% folder. */
QString
win32_app_data_folder()
{
  return win32_get_folder_location(
      CSIDL_APPDATA, QDir::homePath() + "\\Application Data");
}

/** Returns the value in keyName at keyLocation. 
 *  Returns an empty QString if the keyName doesn't exist */
QString
win32_registry_get_key_value(QString keyLocation, QString keyName)
{
  HKEY key;
  char data[255] = {0};
  DWORD size = sizeof(data);

  /* Open the key for reading (opens new key if it doesn't exist) */
  if (RegOpenKeyExA(HKEY_CURRENT_USER,
                    qPrintable(keyLocation), 
                    0L, KEY_READ, &key) == ERROR_SUCCESS) {
    
    /* Key exists, so read the value into data */
    RegQueryValueExA(key, qPrintable(keyName), 
                    NULL, NULL, (LPBYTE)data, &size);
  }

  /* Close anything that was opened */
  RegCloseKey(key);

  return QString(data);
}

/** Creates and/or sets the key to the specified value */
void
win32_registry_set_key_value(QString keyLocation, QString keyName, QString keyValue)
{
  HKEY key;
  
  /* Open the key for writing (opens new key if it doesn't exist */
  if (RegOpenKeyExA(HKEY_CURRENT_USER,
                   qPrintable(keyLocation),
                   0, KEY_WRITE, &key) != ERROR_SUCCESS) {

    /* Key didn't exist, so write the newly opened key */
    RegCreateKeyExA(HKEY_CURRENT_USER,
                   qPrintable(keyLocation),
                   0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL,
                   &key, NULL);
  }

  /* Save the value in the key */
  RegSetValueExA(key, qPrintable(keyName), 0, REG_SZ, 
                (BYTE *)qPrintable(keyValue),
                (DWORD)keyValue.length() + 1); // include null terminator

  /* Close the key */
  RegCloseKey(key);
}

/** Removes the key from the registry if it exists */
void
win32_registry_remove_key(QString keyLocation, QString keyName)
{
  HKEY key;
  
  /* Open the key for writing (opens new key if it doesn't exist */
  if (RegOpenKeyExA(HKEY_CURRENT_USER,
                   qPrintable(keyLocation),
                   0, KEY_SET_VALUE, &key) == ERROR_SUCCESS) {
  
    /* Key exists so delete it */
    RegDeleteValueA(key, qPrintable(keyName));
  }

  /* Close anything that was opened */
  RegCloseKey(key);
}

/** Returns a list of all currently active processes, including their pid
 * and exe filename. */
QHash<qint64, QString>
win32_process_list()
{
#if defined(UNICODE)
/* Force the ascii verisons of these functions, so we can run on Win98. We
 * don't pass any Unicode strings to these functions anyway. */
#undef PROCESSENTRY32
#undef Process32First
#undef Process32Next
#endif
  QHash<qint64, QString> procList;
  HANDLE hSnapshot;
  PROCESSENTRY32 proc;
  QString exeFile;
  qint64 pid;

  /* Create a snapshot of all active processes */
  hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  if (hSnapshot != INVALID_HANDLE_VALUE) {
    proc.dwSize = sizeof(PROCESSENTRY32);
    
    /* Iterate through all the processes in the snapshot */
    if (Process32First(hSnapshot, &proc)) {
      do {
        /* Extract the PID and exe filename from the process record */
        pid = (qint64)proc.th32ProcessID;
        exeFile = QString::fromAscii((const char *)proc.szExeFile);
        
        /* Add this process to our list */
        procList.insert(pid, exeFile);
      } while (Process32Next(hSnapshot, &proc));
    }
    CloseHandle(hSnapshot);
  }
  return procList;
}

