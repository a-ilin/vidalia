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
** \file UPNPControl.cpp
** \brief Singleton object for interacting with UPNP device
*/

#include "UPNPControl.h"
#include "UPNPControlThread.h"

#include <QMutex>
#include <QMetaType>

#ifdef Q_OS_WIN32
#include <winsock2.h>
#endif


/** UPNPControl singleton instance. */
UPNPControl* UPNPControl::_instance = 0;

/** Returns a pointer to this object's singleton instance. */
UPNPControl* UPNPControl::instance()
{
  if (0 == _instance) {
    _instance = new UPNPControl();
    _instance->_controlThread->start();
  }
  return _instance;
}

/** Constructor. Initializes and starts a thread in which all blocking UPnP
 * operations will be performed. */
UPNPControl::UPNPControl()
{
  _forwardedORPort = 0;
  _forwardedDirPort = 0;
  _error = UnknownError;
  _state = IdleState;

  qRegisterMetaType<UPNPControl::UPNPError>("UPNPControl::UPNPError");
  qRegisterMetaType<UPNPControl::UPNPState>("UPNPControl::UPNPState");

  _mutex = new QMutex();
  _controlThread = new UPNPControlThread(this);
}

/** Destructor. cleanup() should be called before the object is destroyed.
 * \sa cleanup()
 */
UPNPControl::~UPNPControl()
{
  delete _mutex;
  delete _controlThread;
}

/** Terminates the UPnP control thread and frees memory allocated to this
 * object's singleton instance. */
void
UPNPControl::cleanup()
{
  _instance->_controlThread->stop();
  delete _instance;
  _instance = 0;
}

/** Sets <b>desiredDirPort</b> and <b>desiredOrPort</b> to the currently
 * forwarded DirPort and ORPort values. */
void
UPNPControl::getDesiredState(quint16 *desiredDirPort, quint16 *desiredOrPort)
{
  _mutex->lock();
  *desiredDirPort = _forwardedDirPort;
  *desiredOrPort = _forwardedORPort;
  _mutex->unlock();
}

/** Sets the desired DirPort and ORPort port mappings to <b>desiredDirPort</b>
 * and <b>desiredOrPort</b>, respectively. */
void
UPNPControl::setDesiredState(quint16 desiredDirPort, quint16 desiredOrPort)
{
  _mutex->lock();
  _forwardedDirPort = desiredDirPort;
  _forwardedORPort = desiredOrPort;
  _mutex->unlock();

  _controlThread->wakeup();
}

/** Sets the most recent UPnP-related error to <b>error</b> and emits the
 * error() signal. */
void
UPNPControl::setError(UPNPError upnpError)
{
  _mutex->lock();
  _error = upnpError;
  _mutex->unlock();

  emit error(upnpError);
}

/** Sets the current UPnP state to <b>state</b> and emits the stateChanged()
 * signal. */
void
UPNPControl::setState(UPNPState state)
{
  _mutex->lock();
  _state = state;
  _mutex->unlock();

  emit stateChanged(state);
}

/** Returns the type of error that occurred last. */
UPNPControl::UPNPError
UPNPControl::error() const
{
  QMutexLocker locker(_mutex);
  return _error;
}

/** Returns a QString describing the type of error that occurred last. */
QString
UPNPControl::errorString() const
{
  UPNPError error = this->error();

  switch (error) {
    case Success:
      return tr("Success");
    case NoUPNPDevicesFound:
      return tr("No UPnP-enabled devices found");
    case NoValidIGDsFound:
      return tr("No valid UPnP-enabled Internet gateway devices found");
    case WSAStartupFailed:
      return tr("WSAStartup failed");
    case AddPortMappingFailed:
      return tr("Failed to add a port mapping");
    case GetPortMappingFailed:
      return tr("Failed to retrieve a port mapping");
    case DeletePortMappingFailed:
      return tr("Failed to remove a port mapping");
    default:
      return tr("Unknown error");
  }
}

/** Returns the number of milliseconds to wait for devices to respond
 * when attempting to discover UPnP-enabled IGDs. */
int
UPNPControl::discoverTimeout() const
{
  return UPNPControlThread::UPNPCONTROL_DISCOVER_TIMEOUT;
}

