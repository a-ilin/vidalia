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
** \file UPNPControlThread.h
** \brief Thread for configuring UPnP in the background
*/

#ifndef _UPNPCONTROLTHREAD_H
#define _UPNPCONTROLTHREAD_H

#include "UPNPControl.h"

#define STATICLIB
#include <miniupnpc/miniwget.h>
#include <miniupnpc/miniupnpc.h>
#include <miniupnpc/upnpcommands.h>
#undef STATICLIB

#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QTime>


class UPNPControlThread : public QThread
{
  Q_OBJECT

public:
  /** Specifies the number of milliseconds to wait for devices to respond
   * when attempting to discover UPnP-enabled IGDs. */
  static const int UPNPCONTROL_DISCOVER_TIMEOUT = 2000;

  /** Constructor. <b>control</b> will be used for retrieving the desired port
   * forwarding state. */
  UPNPControlThread(UPNPControl *control);
  /** Destructor. The UPnP control thread must be stopped prior to destroying
   * this object. */
  ~UPNPControlThread();
  /** Terminates the UPnP control thread's run() loop. */
  void stop();
  /** Wakes up the UPnP control thread's run() loop. */
  void wakeup();

protected:
  /** Thread entry point. The thread has a main loop that periodically wakes
   * up  and updates the configured port mappings. Upon exiting, all port
   * mappings will be removed. */
  void run();

private:
  /** Sets up port forwarding according the previously-configured desired
   * state. The desired state is set using UPNPControl's setDesiredState()
   * method. */
  void configurePorts();
  /** Discovers UPnP-enabled IGDs on the network.  This method will block for
   * UPNPCONTROL_DISCOVER_TIMEOUT milliseconds. */
  UPNPControl::UPNPError initializeUPNP();
  /** Updates the port mapping for <b>oldPort</b>, changing it to
   * <b>newPort</b>. */
  UPNPControl::UPNPError updatePort(quint16 oldPort, quint16 newPort);
  /** Adds a port forwarding mapping from external:<b>port</b> to
   * internal:<b>port</b>. Returns 0 on success, or non-zero on failure. */
  UPNPControl::UPNPError forwardPort(quint16 port);
  /** Removes the port mapping for <b>port</b>. Returns 0 on success or
   * non-zero on failure. */
  UPNPControl::UPNPError disablePort(quint16 port);

  QTime _upnpInitialized; /**< Time at which the UPnP state was last set. */
  bool _keepRunning; /**< True if the control thread should keep running. */
  UPNPControl *_control; /**< Stores desired UPnP state. */
  QWaitCondition *_waitCondition; /**< Used to wake up the control thread. */
  QMutex *_waitMutex; /**< Mutex around shared variables. */
  quint16 _dirPort; /**< Desired DirPort. */
  quint16 _orPort; /**< Desired ORPort. */

  /* Used by miniupnpc library */
  struct UPNPUrls urls;
  struct IGDdatas data;
  char lanaddr[16];
};
#endif

