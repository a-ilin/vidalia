/*
**  This file is part of Vidalia, and is subject to the license terms in the
**  LICENSE file, found in the top level directory of this distribution. If
**  you did not receive the LICENSE file with this file, you may obtain it
**  from the Vidalia source package distributed by the Vidalia Project at
**  http://www.torproject.org/projects/vidalia.html. No part of Vidalia,
**  including this file, may be copied, modified, propagated, or distributed
**  except according to the terms described in the LICENSE file.
*/

/*
** \file TorService.cpp
** \brief Starts, stops, installs, and uninstalls a Tor service (Win32).
*/

#include "TorService.h"
#include "TorService_p.h"
#include "tcglobal.h"

#include <QLibrary>

/** Returned by TorService::exitCode() when we are unable to determine the
 * actual exit code of the service (unless, of course, Tor returns -999999). */
#define UNKNOWN_EXIT_CODE     -999999

/** List of dynamically loaded NT service functions. */
ServiceFunctions TorServicePrivate::_service_fns =
  { false,
    NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL
  };


TorServicePrivate::TorServicePrivate(TorService* _q_ptr)
  : q_ptr(_q_ptr)
{
  _scm = openSCM();
}

TorServicePrivate::~TorServicePrivate()
{
  closeHandle(_scm);
}

/** Default ctor. */
TorService::TorService(QObject *parent)
  : QObject(parent)
  , d_ptr(new TorServicePrivate(this))
{
}

/** Default dtor. */
TorService::~TorService()
{
}

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
TorServicePrivate::loadServiceFunctions()
{
#define LOAD_SERVICE_FN(f) do {                                         \
  void *fn;                                                             \
  if (!((fn = QLibrary::resolve("advapi32", #f)))) {                    \
      return false;                                                     \
    } else {                                                            \
      _service_fns.f = (f ## _fn) fn;                                   \
    }                                                                   \
  } while (0)

  if (!TorService::isSupported()) {
    _service_fns.loaded = false;
  } else if (!_service_fns.loaded) {
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
  return _service_fns.loaded;
}

/** Opens a handle to the Tor service. Returns NULL on error. */
SC_HANDLE
TorServicePrivate::openService()
{
  if (!loadServiceFunctions())
    return NULL;
  if (!_scm)
    return NULL;
  return _service_fns.OpenServiceA(_scm,
                                   (LPCTSTR)TOR_SERVICE_NAME,
                                   TOR_SERVICE_ACCESS);
}

/** Opens a handle to the service control manager. Returns NULL on error. */
SC_HANDLE
TorServicePrivate::openSCM()
{
  if (!loadServiceFunctions())
    return NULL;
  return _service_fns.OpenSCManagerA(NULL, NULL, SC_MANAGER_ALL_ACCESS);
}

/** Closes the service <b>handle</b>. */
void
TorServicePrivate::closeHandle(SC_HANDLE handle)
{
  if (!loadServiceFunctions())
    return;
  _service_fns.CloseServiceHandle(handle);
}

/** Returns true if the Tor service is installed. */
bool
TorService::isInstalled()
{
  bool installed;
  SC_HANDLE service = d_ptr->openService();
  installed = (service != NULL);
  d_ptr->closeHandle(service);
  return installed;
}

/** Returns true if the Tor service is running. */
bool
TorService::isRunning()
{
  return (d_ptr->status() == SERVICE_RUNNING);
}

/** Starts Tor service. */
void
TorService::start()
{
  SC_HANDLE service = d_ptr->openService();

  if (!service) {
    tc::error("Bug: We tried to start the Tor service, but it is not installed.");
    emit startFailed(tr("The Tor service is not installed."));
    return;
  }

  /* Starting a service can take up to 30 seconds! */
  if (d_ptr->status() != SERVICE_RUNNING) {
    int tries = 0;
    tc::debug("Starting the Tor service.");
    d_ptr->_service_fns.StartServiceA(service, 0, NULL);

    while ((d_ptr->status() != SERVICE_RUNNING) && ++tries <= 5)
      Sleep(1000);
  }

  if (d_ptr->status() == SERVICE_RUNNING) {
    emit started();
  } else {
    tc::error("Unable to start the Tor service.");
    emit startFailed(tr("Unable to start the Tor service."));
  }
  d_ptr->closeHandle(service);
}

/** Stops Tor service. */
bool
TorService::stop()
{
  SC_HANDLE service = d_ptr->openService();

  if (!service)
    return false;

  if (d_ptr->status() != SERVICE_STOPPED) {
    SERVICE_STATUS stat;
    stat.dwCurrentState = SERVICE_RUNNING;
    tc::debug("Stopping the Tor service.");
    if (d_ptr->_service_fns.ControlService(service, SERVICE_CONTROL_STOP, &stat)) {
      /* XXX Five seconds isn't long enough to wait when we're stopping a Tor
       * that is running as a server, but we don't want to block for 30
       * seconds. It would be nice if we could get an async notification when
       * the service stops or fails to stop. */
      int tries = 0;
      while ((d_ptr->status() != SERVICE_STOPPED) && (++tries <= 5))
        Sleep(1000);
    }
  }
  d_ptr->closeHandle(service);

  /* Find out if the service really stopped and return the result */
  if (d_ptr->status() == SERVICE_STOPPED) {
    emit finished(exitCode(), exitStatus());
    return true;
  }
  /* XXX This needs an actual reason message. */
  tc::error("Unable to stop the Tor service.");
  return false;
}

/** Returns the exit code of the last Tor service that finished. */
int
TorService::exitCode()
{
  SC_HANDLE service;
  int exitCode = UNKNOWN_EXIT_CODE;

  service = d_ptr->openService();
  if (service) {
    SERVICE_STATUS s;
    if (d_ptr->_service_fns.QueryServiceStatus(service, &s)) {
      /* Services return one exit code, but it could be in one of two
       * variables. Fun. */
      exitCode = (int)(s.dwWin32ExitCode == ERROR_SERVICE_SPECIFIC_ERROR
                                            ? s.dwServiceSpecificExitCode
                                            : s.dwWin32ExitCode);
    }
    d_ptr->closeHandle(service);
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

/** Installs the Tor service. Returns true if the service was successfully
 * installed or already exists. */
bool
TorService::install(const QString &torPath, const QString &torrc,
                    quint16 controlPort)
{
  SC_HANDLE service;

  if (!d_ptr->_scm)
    return false;

  service = d_ptr->openService();
  if (!service) {
    QString command = QString("\"%1\" --nt-service -f \"%2\" ControlPort %3")
                                                 .arg(torPath)
                                                 .arg(torrc)
                                                 .arg(controlPort);

    tc::debug("Installing the Tor service using the command line '%1'")
                                                          .arg(command);
    service = d_ptr->_service_fns.CreateServiceA(d_ptr->_scm,
                              (LPCTSTR)TOR_SERVICE_NAME, (LPCTSTR)TOR_SERVICE_DISP,
                              TOR_SERVICE_ACCESS, SERVICE_WIN32_OWN_PROCESS,
                              SERVICE_AUTO_START, SERVICE_ERROR_IGNORE,
                              (LPCTSTR)command.toLatin1().data(), NULL, NULL, NULL,
                              NULL, NULL);
    if (!service) {
      /* XXX This needs an actual reason message. */
      tc::error("Failed to install the Tor service.");
      return false;
    }

    SERVICE_DESCRIPTION desc;
    desc.lpDescription = TOR_SERVICE_DESC;
    d_ptr->_service_fns.ChangeServiceConfig2A(service,
                                       SERVICE_CONFIG_DESCRIPTION, &desc);
    d_ptr->closeHandle(service);
  }
  return true;
}

/** Removes the Tor service. Returns true if the service was removed
 * successfully or does not exist. */
bool
TorService::remove()
{
  bool removed = true;
  SC_HANDLE service = d_ptr->openService();

  if (service) {
    stop();
    tc::debug("Removing the Tor service.");
    removed = d_ptr->_service_fns.DeleteService(service);
    d_ptr->closeHandle(service);
  }
  if (!removed) {
    /* XXX This needs an actual reason message. */
    tc::error("Failed to remove the Tor service.");
  }
  return removed;
}

/** Gets the status of the Tor service. */
DWORD
TorServicePrivate::status()
{
  SC_HANDLE service;
  SERVICE_STATUS s;
  DWORD stat = SERVICE_ERROR;

  service = openService();
  if (service && _service_fns.QueryServiceStatus(service, &s))
    stat = s.dwCurrentState;
  closeHandle(service);
  return stat;
}

