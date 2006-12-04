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
 * \file torservice.cpp
 * \version $Id$
 * \brief Starts, stops, installs, and uninstalls a Tor service (Win32).
 */

#include <QLibrary>

#include "torservice.h"

/** Returned by TorService::exitCode() when we are unable to determine the
 * actual exit code of the service (unless, of course, Tor returns -999999). */
#define UNKNOWN_EXIT_CODE     -999999

/** List of dynamically loaded NT service functions. */
ServiceFunctions TorService::_service_fns = 
  { false,
    NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL };


/** Returns true if services are supported. */
bool
TorService::isSupported()
{
  return (QSysInfo::WindowsVersion & QSysInfo::WV_NT_based);
}

/** Dyanmically loads NT service related functions from advapi32.dll. This
 * function is adapted from Tor's nt_service_load_library() function. See
 * LICENSE for details on Tor's license. */
bool
TorService::loadServiceFunctions()
{
#define LOAD_SERVICE_FN(f) do {                                         \
  void *fn;                                                             \
  if (!((fn = QLibrary::resolve("advapi32", #f)))) {                    \
      return false;                                                     \
    } else {                                                            \
      _service_fns.f = (f ## _fn) fn;                                   \
    }                                                                   \
  } while (0)

  if (!_service_fns.loaded) {
    LOAD_SERVICE_FN(ChangeServiceConfig2A);
    LOAD_SERVICE_FN(CloseServiceHandle);
    LOAD_SERVICE_FN(ControlService);
    LOAD_SERVICE_FN(CreateServiceA);
    LOAD_SERVICE_FN(DeleteService);
    LOAD_SERVICE_FN(OpenSCManagerA);
    LOAD_SERVICE_FN(OpenServiceA);
    LOAD_SERVICE_FN(QueryServiceStatus);
    LOAD_SERVICE_FN(SetServiceStatus);
    LOAD_SERVICE_FN(StartServiceA);
    _service_fns.loaded = true;
  }
  return true;
}

/** Default ctor. */
TorService::TorService(QObject* parent)
  : QObject(parent)
{
  _manager = NULL;
  _service = NULL;

  initialize();
}

/** Default dtor. */
TorService::~TorService()
{
  close();
}

/** Closes the service and the service manager. */
void
TorService::close()
{
  if (loadServiceFunctions()) {
    if (_service) {
      _service_fns.CloseServiceHandle(_service);
      _service = NULL;
    }
    if (_manager) {
      _service_fns.CloseServiceHandle(_manager);
      _manager = NULL;
    }
  }
}

/** Initializes the service and service manager. */
bool
TorService::initialize()
{
  /* If services are supported, initialize the manager and service */
  if (isSupported()) {
    /* Load NT service related functions */
    if (!loadServiceFunctions()) {
      return false;
    }
    /* Open service manager */
    if (_manager == NULL) {
      _manager = _service_fns.OpenSCManagerA(NULL, NULL, 
                                             SC_MANAGER_ALL_ACCESS);
    }
    /* Open tor service */
    if (_service == NULL) {
      _service = _service_fns.OpenServiceA(_manager, 
                                           (LPCTSTR)TOR_SERVICE_NAME,
                                           TOR_SERVICE_ACCESS);
    }
  }
  return true;
}

/** Returns true if the Tor service is installed. */
bool
TorService::isInstalled()
{
  initialize();
  return (_service != NULL);
}

/** Returns true if the Tor service is running. */
bool
TorService::isRunning()
{
  initialize();
  return (status() == SERVICE_RUNNING);
}

/** Starts Tor service. */
void
TorService::start()
{
  if (!isSupported()) {
    emit startFailed(tr("Services not supported on this platform."));
    return;
  }
  if (!isInstalled()) {
    emit startFailed(tr("The Tor service is not installed."));
    return;
  }
  if (!initialize()) {
    return;
  }

  /* Starting a service can take up to 30 seconds! */
  if (!isRunning()) {
    _service_fns.StartServiceA(_service, 0, NULL);
  }
  
  int tries = 0;
  while (true) {
    if (isRunning()) break;
    tries++;
    Sleep(1000);
    if (tries > 5) break;
  }

  if (isRunning()) {
    emit started();
  } else {
    emit startFailed(tr("Unable to start Tor service."));
  }
}

/** Stops Tor service. */
void
TorService::stop()
{
  if (!loadServiceFunctions()) {
    return;
  }
  if (isRunning()) {
    SERVICE_STATUS stat;
    stat.dwCurrentState = SERVICE_RUNNING;
    _service_fns.ControlService(_service, SERVICE_CONTROL_STOP, &stat);

    int tries = 0;
    while (true) {
      if (status() ==  SERVICE_STOPPED) break;
      tries++;
      Sleep(1000);
      if (tries > 5) break;
    }
  }

  if (!isRunning()) {
    /* Emit the signal that we stopped and the service's exit code and status. */
    emit finished(exitCode(), exitStatus());
  }
}

/** Returns the exit code of the last Tor service that finished. */
int
TorService::exitCode()
{
  int exitCode = UNKNOWN_EXIT_CODE;
  
  if (loadServiceFunctions()) {
    if (isSupported() && _manager && _service) {
      SERVICE_STATUS s;
      if (_service_fns.QueryServiceStatus(_service, &s)) {
        /* Services return one exit code, but it could be in one of two
         * variables. Fun. */
        exitCode = (int)(s.dwWin32ExitCode == ERROR_SERVICE_SPECIFIC_ERROR
                                              ? s.dwServiceSpecificExitCode
                                              : s.dwWin32ExitCode);
      }
    }
  }
  return exitCode;
}

/** Returns the exit status of the last Tor service that finished. */
QProcess::ExitStatus
TorService::exitStatus()
{
  /* NT services don't really have an equivalent to QProcess::CrashExit, so 
   * this just returns QProcess::NormalExit. Tor _could_ set
   * dwServiceSpecificExitCode to some magic value when it starts and then
   * set it to the real exit code when Tor exits. Then we would know if the
   * service crashed when dwServiceSpecificExitCode is still the magic value.
   * However, I don't care and it doesn't really matter anyway. */
  return QProcess::NormalExit;
}

/** Installs the Tor service. */
bool
TorService::install(const QString &torPath, const QString &torrc,
                    quint16 controlPort)
{
  if (!isSupported()) return false;

  if (!isInstalled()) {
    QString command = QString("\"%1\" --nt-service -f \"%2\" ControlPort %3")
                                                 .arg(torPath)
                                                 .arg(torrc)
                                                 .arg(controlPort);

    _service = _service_fns.CreateServiceA(_manager, 
                              (LPCTSTR)TOR_SERVICE_NAME, (LPCTSTR)TOR_SERVICE_DISP,
                              TOR_SERVICE_ACCESS, SERVICE_WIN32_OWN_PROCESS,
                              SERVICE_AUTO_START, SERVICE_ERROR_IGNORE,
                              (LPCTSTR)command.toAscii().data(), NULL, NULL, NULL, 
                              NULL, NULL);

    if (_service != NULL) {
      SERVICE_DESCRIPTION desc;
      desc.lpDescription = TOR_SERVICE_DESC;
      _service_fns.ChangeServiceConfig2A(_service, 
                                         SERVICE_CONFIG_DESCRIPTION, &desc);
    }
  }
  return isInstalled();
}

/** Removes the Tor service. */
bool
TorService::remove()
{
  if (!loadServiceFunctions()) {
    return false;
  }

  if (!isSupported()) return false;
  if (isInstalled()) {
    /* Stop the service */
    stop();

    /* Delete the service */
    if (!_service_fns.DeleteService(_service)) return false;
    
    /* Release references to manager and service */
    close();
  }
  return !isInstalled();
}


/** Gets the status of the Tor service. */
DWORD
TorService::status()
{
  if (!(isSupported() && _manager && _service)) return DWORD(-1);

  SERVICE_STATUS s;
  DWORD stat = SERVICE_ERROR;

  if (_service_fns.QueryServiceStatus(_service, &s)) {
    stat = s.dwCurrentState;
  } 
  return stat;
}

