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
 */

#include "torservice.h"


/** Returns true if services are supported. */
bool
TorService::isSupported()
{
  bool supported = false;
#if defined(Q_OS_WIN32)
  supported = (QSysInfo::WindowsVersion & QSysInfo::WV_NT_based);
#endif
  return supported;
}

/** Default ctor. */
TorService::TorService(const QString &torPath, const QString &torrc,
                       QObject* parent)
  : QObject(parent)
{
  // FIXME this is hardcoded for now
  _torrc = "\"C:\\Program Files\\Tor\\torrc\"";
  _torPath = "\"" + torPath + "\"";
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
#if defined(Q_OS_WIN32)
  if (_service) {
    CloseServiceHandle(_service);
    _service = NULL;
  }

  if (_manager) {
    CloseServiceHandle(_manager);
    _manager = NULL;
  }
#endif
}

/** Initializes the service and service manager. */
void
TorService::initialize()
{
  /* If services are supported, initialize the manager and service */
#if defined(Q_OS_WIN32)
  if (isSupported()) {
    /* Open service manager */
    if (_manager == NULL) {
    _manager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    }
    /* Open tor service */
    if (_service == NULL) {
    _service = OpenServiceA(_manager, TOR_SERVICE_NAME, TOR_SERVICE_ACCESS);
    }
  }
#endif
}

/** Returns true if the Tor service is installed. */
bool
TorService::isInstalled()
{
  return _service != NULL;
}

/** Returns true if the Tor service is running. */
bool
TorService::isRunning()
{
#if defined(Q_OS_WIN32)
  return (status() == SERVICE_RUNNING);
#else
  return false;
#endif
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
    if (!install()) {
      emit startFailed(tr("Unable to install the Tor service."));
      return;
    }
  }
#if defined(Q_OS_WIN32)
  /* Make sure we're initialized */
  initialize();

  /* Starting a service can take up to 30 seconds! */
  if (!isRunning()) StartService(_service, 0, NULL); 
  
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
#endif
}

/** Stops Tor service. */
void
TorService::stop()
{
#if defined(Q_OS_WIN32)
  if (isRunning()) {
    SERVICE_STATUS stat;
    stat.dwCurrentState = SERVICE_RUNNING;
    ControlService(_service, SERVICE_CONTROL_STOP, &stat);

    int tries = 0;
    while (true) {
      if (status() ==  SERVICE_STOPPED) break;
      tries++;
      Sleep(1000);
      if (tries > 5) break;
    }
  }

  if (!isRunning()) emit finished();
#endif
}

/** Installs the Tor service. */
bool
TorService::install()
{
#if defined(Q_OS_WIN32)
  if (!isSupported()) return false;

  /* Make sure we're initialized */
  initialize();

  if (!isInstalled()) {
    QString command = _torPath + " --nt-service -f " + _torrc;

    _service = CreateServiceA(_manager, TOR_SERVICE_NAME, TOR_SERVICE_DISP,
                              TOR_SERVICE_ACCESS, SERVICE_WIN32_OWN_PROCESS,
                              SERVICE_AUTO_START, SERVICE_ERROR_IGNORE,
                              command.toAscii().data(), NULL, NULL, NULL, NULL, "");

    SERVICE_DESCRIPTION desc;
    desc.lpDescription = TOR_SERVICE_DESC;
    ChangeServiceConfig2(_service, SERVICE_CONFIG_DESCRIPTION, &desc);
  }
  return isInstalled();
#else
  return false;
#endif
}

/** Removes the Tor service. */
bool
TorService::remove()
{
#if defined(Q_OS_WIN32)
  if (!isSupported()) return false;
  if (isInstalled()) {
    /* Stop the service */
    stop();

    /* Delete the service */
    if (!DeleteService(_service)) return false;
    
    /* Release references to manager and service */
    close();
  }

  return !isInstalled();
#else
  return false;
#endif
}

#if defined(Q_OS_WIN32)
/** Gets the status of the Tor service. */
DWORD
TorService::status()
{
  if (!(isSupported() && _manager && _service)) return DWORD(-1);

  SERVICE_STATUS s;
  DWORD stat = SERVICE_ERROR;

  if (QueryServiceStatus(_service, &s)) {
    stat = s.dwCurrentState;
  } 
  return stat;
#endif
}

