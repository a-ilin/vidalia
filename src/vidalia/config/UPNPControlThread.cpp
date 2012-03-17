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
** \file UPNPControlThread.cpp
** \brief Thread for configuring UPnP in the background
*/

#include "UPNPControlThread.h"
#include "UPNPControl.h"
#include "Vidalia.h"

#include <QWaitCondition>
#include <QMutex>
#include <QTime>
#include <QTextStream>
#include <QString>
#include <QMessageBox>

#define UPNPCONTROL_REINIT_MSEC 300000 // 5 minutes
#define UPNPCONTROL_MAX_WAIT_MSEC 60000 // 1 minute


/** Constructor. <b>control</b> will be used for retrieving the desired port
 * forwarding state. */
UPNPControlThread::UPNPControlThread(UPNPControl *control)
{
  _upnpInitialized = QTime();
  _keepRunning = true;
  _control = control;

  _dirPort = 0;
  _orPort = 0;

  _waitCondition = new QWaitCondition();
  _waitMutex = new QMutex();
}

/** Destructor. The UPnP control thread must be stopped prior to destroying
 * this object.
 * \sa stop()
 */
UPNPControlThread::~UPNPControlThread()
{
  delete _waitCondition;
  delete _waitMutex;
}

/** Thread entry point. The thread has a main loop that periodically wakes up
 * and updates the configured port mappings. Upon exiting, all port mappings
 * will be removed. */
void
UPNPControlThread::run()
{
  bool shouldExit = false;

  forever {
    /* TODO: Check for switching OR/Dir port */
    /* TODO: Check for router losing state */

    configurePorts();

    /* Wait for something to happen */
    _waitMutex->lock();
    if (_keepRunning) {
      /* We should continue */
      UPNPControl::instance()->setState(UPNPControl::IdleState);
      _waitCondition->wait(_waitMutex, UPNPCONTROL_MAX_WAIT_MSEC);

      /* Maybe we were asked to exit while waiting */
      shouldExit = !_keepRunning;
      _waitMutex->unlock();
      if (shouldExit)
        break;
    } else {
      /* We should exit */
      _waitMutex->unlock();
      break;
    }
  }

  /* Remove the existing port forwards */
  updatePort(_dirPort, 0);
  updatePort(_orPort, 0);
}

/** Sets up port forwarding according the previously-configured desired state.
 * The desired state is set using UPNPControl's setDesiredState() method.
 * \sa UPNPControl::setDesiredState
 */
void
UPNPControlThread::configurePorts()
{
  quint16 desiredDirPort, desiredOrPort;
  bool force_init = false;
  UPNPControl::UPNPError retval = UPNPControl::Success;

  /* Get desired state */
  _control->getDesiredState(&desiredDirPort, &desiredOrPort);

  /* If it's been a while since we initialized the router, or time has gone
     backward, then maybe the router has gone away or forgotten the forwards.
     Reset UPnP state, and re-do the port forwarding */
  if (_upnpInitialized.isNull() || // Is this the first time we have used UPNP?
      _upnpInitialized>QTime::currentTime() || // Has time gone backwards?
      _upnpInitialized.addMSecs(UPNPCONTROL_REINIT_MSEC)<QTime::currentTime() // Has it been REINIT_MSEC since initialization
      ) {
    _upnpInitialized = QTime();
    force_init = true;
  }

  if (!force_init) {
    /* Configure DirPort */
    if (desiredDirPort != _dirPort) {
      UPNPControl::instance()->setState(UPNPControl::UpdatingDirPortState);
      retval = updatePort(_dirPort, desiredDirPort);
      if (retval == UPNPControl::Success)
        _dirPort = desiredDirPort;
      else
        goto err;
    }

    /* Configure ORPort */
    if (desiredOrPort != _orPort) {
      UPNPControl::instance()->setState(UPNPControl::UpdatingORPortState);
      retval = updatePort(_orPort, desiredOrPort);
      if (retval == UPNPControl::Success)
        _orPort = desiredOrPort;
      else
        goto err;
    }
  } else {
    /* Add the mapping even if they exist already */
    UPNPControl::instance()->setState(UPNPControl::UpdatingDirPortState);
    retval = updatePort(0, desiredDirPort);
    if (retval == UPNPControl::Success)
      _dirPort = desiredDirPort;
    else
      goto err;

    UPNPControl::instance()->setState(UPNPControl::UpdatingORPortState);
    retval = updatePort(0, desiredOrPort);
    if (retval == UPNPControl::Success)
      _orPort = desiredOrPort;
    else goto err;
  }

  UPNPControl::instance()->setState(UPNPControl::ForwardingCompleteState);
  return;

err:
  UPNPControl::instance()->setError(retval);
  UPNPControl::instance()->setState(UPNPControl::ErrorState);
}

/** Terminates the UPnP control thread's run() loop.
 * \sa run()
 */
void
UPNPControlThread::stop()
{
  /* Lock access to _keepRunning */
  _waitMutex->lock();

  /* Ask the thread to stop */
  _keepRunning = false;

  /* Wake it up if needed */
  _waitCondition->wakeAll();

  /* Unlock shared state */
  _waitMutex->unlock();

  /* Wait for it to finish */
  wait();
}

/** Wakes up the UPnP control thread's run() loop.
 * \sa run()
 */
void
UPNPControlThread::wakeup()
{
  _waitMutex->lock();
  _waitCondition->wakeAll();
  _waitMutex->unlock();
}

/** Updates the port mapping for <b>oldPort</b>, changing it to
 * <b>newPort</b>. */
UPNPControl::UPNPError
UPNPControlThread::updatePort(quint16 oldPort, quint16 newPort)
{
  UPNPControl::UPNPError retval;

#ifdef Q_OS_WIN32
  // Workaround from http://trolltech.com/developer/knowledgebase/579
  WSAData wsadata;
  if (WSAStartup(MAKEWORD(2,0), &wsadata) != 0) {
    vWarn("WSAStartup failure while updating UPnP port forwarding");
    return UPNPControl::WSAStartupFailed;
  }
#endif

  if (_upnpInitialized.isNull() && (oldPort != 0 || newPort != 0)) {
    retval = initializeUPNP();
    if (retval == UPNPControl::Success)
      _upnpInitialized = QTime::currentTime();
    else
      _upnpInitialized = QTime();
  } else {
    retval = UPNPControl::Success;
  }

  if (retval == UPNPControl::Success && oldPort != 0)
    retval = disablePort(oldPort);

  if (retval == UPNPControl::Success && newPort != 0)
    retval = forwardPort(newPort);

#ifdef Q_OS_WIN32
  WSACleanup();
#endif

  return retval;
}

/** Discovers UPnP-enabled IGDs on the network. Based on
 * http://miniupnp.free.fr/files/download.php?file=xchat-upnp20061022.patch
 * This method will block for UPNPCONTROL_DISCOVER_TIMEOUT milliseconds. */
UPNPControl::UPNPError
UPNPControlThread::initializeUPNP()
{
  struct UPNPDev *devlist;
  int retval;

  memset(&urls, 0, sizeof(struct UPNPUrls));
  memset(&data, 0, sizeof(struct IGDdatas));

  UPNPControl::instance()->setState(UPNPControl::DiscoverState);

  devlist = upnpDiscover(UPNPCONTROL_DISCOVER_TIMEOUT, NULL, NULL, 0);
  if (NULL == devlist) {
    vWarn("upnpDiscover returned: NULL");
    return UPNPControl::NoUPNPDevicesFound;
  }

  retval = UPNP_GetValidIGD(devlist, &urls, &data, lanaddr, sizeof(lanaddr));

  vInfo("GetValidIGD returned: %1").arg(retval);

  freeUPNPDevlist(devlist);

  if (retval != 1 && retval != 2)
    return UPNPControl::NoValidIGDsFound;

  return UPNPControl::Success;
}

/** Adds a port forwarding mapping from external:<b>port</b> to
 * internal:<b>port</b>. Returns 0 on success, or non-zero on failure. */
UPNPControl::UPNPError
UPNPControlThread::forwardPort(quint16 port)
{
  QString sPort;
  int retval;

  char intClient[16];
  char intPort[6];

  // Convert the port number to a string
  sPort = QString::number(port);

  // Add the port mapping of external:port -> internal:port
  retval = UPNP_AddPortMapping(urls.controlURL, data.first.servicetype,
                               qPrintable(sPort), qPrintable(sPort), lanaddr,
                               "Tor relay", "TCP", NULL);
  if(UPNPCOMMAND_SUCCESS != retval) {
    vWarn("AddPortMapping(%1, %2, %3) failed with code %4")
            .arg(sPort).arg(sPort).arg(lanaddr).arg(retval);
    return UPNPControl::AddPortMappingFailed;
  }

  // Check if the port mapping was accepted
  retval = UPNP_GetSpecificPortMappingEntry(urls.controlURL, data.first.servicetype,
                                            qPrintable(sPort), "TCP",
                                            intClient, intPort);
  if(UPNPCOMMAND_SUCCESS != retval) {
    vWarn("GetSpecificPortMappingEntry() failed with code %1").arg(retval);
    return UPNPControl::GetPortMappingFailed;
  }

  if(! intClient[0]) {
    vWarn("GetSpecificPortMappingEntry failed.");
    return UPNPControl::GetPortMappingFailed;
  }

  // Output the mapping
  vInfo("(external):%1 -> %2:%3").arg(sPort).arg(intClient).arg(intPort);

  return UPNPControl::Success;
}

/** Removes the port mapping for <b>port</b>. Returns 0 on success or non-zero
 * on failure. */
UPNPControl::UPNPError
UPNPControlThread::disablePort(quint16 port)
{
  QString sPort = QString::number(port);

  // Remove the mapping
  int retval = UPNP_DeletePortMapping(urls.controlURL, data.first.servicetype,
                                      qPrintable(sPort), "TCP", NULL);
  if(UPNPCOMMAND_SUCCESS != retval) {
    vWarn("DeletePortMapping() failed with code %1").arg(retval);
    return UPNPControl::DeletePortMappingFailed;
  }

  // Output the cancelled mapping
  vInfo("(external):%1 -> <>").arg(sPort);

  return UPNPControl::Success;
}

