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
** \file win32.cpp
** \version $Id$ 
** \brief Win32-specific functions
*/

#include "win32.h"
#include <tlhelp32.h>
#include <shlobj.h>
#include <QDir>
#include <QLibrary>
#include <QtDebug>

#if defined(UNICODE)
/* Force the ascii verisons of these functions, so we can run on Win98. We
 * don't pass any Unicode strings to these functions anyway. */
#undef PROCESSENTRY32
#undef LPPROCESSENTRY32
#undef Process32First
#undef Process32Next
#endif

/* Load the tool help functions dynamically, since they don't exist on
 * Windows NT 4.0 */
typedef HANDLE (WINAPI *CreateToolhelp32Snapshot_fn)(DWORD, DWORD);
typedef BOOL (WINAPI *Process32First_fn)(HANDLE, LPPROCESSENTRY32);
typedef BOOL (WINAPI *Process32Next_fn)(HANDLE, LPPROCESSENTRY32);


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

/**
 * Callback for EnumThreadWindows which sends the WM_QUIT message
 */
BOOL CALLBACK quitWindowCallback(HWND hwnd, LPARAM targetPID)
{
  DWORD hwndPID = 0;

  /* If the process ID for hwnd matches the target PID, post
     WM_QUIT to the window */
  GetWindowThreadProcessId(hwnd, &hwndPID);
  if (hwndPID == (DWORD)targetPID)
    PostMessage(hwnd, WM_QUIT, 0, (LPARAM)NULL);
}

/**
 * Close process with the specified PID. Sends WM_QUIT to all
 * top-level windows.
 */
void
win32_end_process_by_pid(DWORD pid)
{
  /* Send WM_QUIT to all windows */
  EnumWindows(&quitWindowCallback, (LPARAM)pid);
  /* At this point we could kill the main thread, but how do we find
     the ID of the main thread? We can find the ID of all threads
     but killing them all seems to cause a problem for Firefox */
  //PostThreadMessage(thread.th32ThreadID, WM_CLOSE, 0, (LPARAM)NULL);
}

/**
 * Close all processes started from the specified filename. Sends
 * WM_QUIT to all top-level windows. Filename should be given in
 * lowercase, and comparison is case insensitive. Note: the MSDN
 * documentation for WM_QUIT states that the message should not be
 * sent by PostMessage(). However, sending WM_CLOSE leaves Firefox
 * running, whereas WM_QUIT seems to work.
 */
void
win32_end_process_by_filename(QString filename)
{
  /* Get list of running processes */
  QHash<qint64, QString> procList = win32_process_list();

  /* On old versions of Windows win32_process_list() will return
     an empty list. In this case, just keep Vidalia open */
  if (procList.isEmpty()) {
    return;
  }

  /* Loop over all processes */
  QHashIterator<qint64, QString> i(procList);
  while (i.hasNext()) {
    i.next();
    if (i.value().toLower() == filename) {
      /* Kill this process */
      win32_end_process_by_pid((DWORD)i.key());
    }
  }
}

/** Returns a list of all currently active processes, including their pid
 * and exe filename. */
QHash<qint64, QString>
win32_process_list()
{
  QHash<qint64, QString> procList;
  CreateToolhelp32Snapshot_fn pCreateToolhelp32Snapshot;
  Process32First_fn pProcess32First;
  Process32Next_fn pProcess32Next;
  HANDLE hSnapshot;
  PROCESSENTRY32 proc;
  QString exeFile;
  qint64 pid;

  /* Load the tool help functions */
  pCreateToolhelp32Snapshot =
    (CreateToolhelp32Snapshot_fn)QLibrary::resolve("kernel32", "CreateToolhelp32Snapshot");
  pProcess32First = (Process32First_fn)QLibrary::resolve("kernel32", "Process32First");
  pProcess32Next = (Process32Next_fn)QLibrary::resolve("kernel32", "Process32Next");
 
  if (!pCreateToolhelp32Snapshot || !pProcess32First || !pProcess32Next) {
    qWarning("Unable to load tool help functions. Running process information "
             "will be unavailable.");
    return QHash<qint64, QString>();
  }

  /* Create a snapshot of all active processes */
  hSnapshot = pCreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  if (hSnapshot != INVALID_HANDLE_VALUE) {
    proc.dwSize = sizeof(PROCESSENTRY32);
    
    /* Iterate through all the processes in the snapshot */
    if (pProcess32First(hSnapshot, &proc)) {
      do {
        /* Extract the PID and exe filename from the process record */
        pid = (qint64)proc.th32ProcessID;
        exeFile = QString::fromAscii((const char *)proc.szExeFile);
        
        /* Add this process to our list */
        procList.insert(pid, exeFile);
      } while (pProcess32Next(hSnapshot, &proc));
    }
    CloseHandle(hSnapshot);
  }
  return procList;
}

