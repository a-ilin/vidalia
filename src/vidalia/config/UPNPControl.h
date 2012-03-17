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
** \file UPNPControl.h
** \brief Singleton object for interacting with UPNP device
*/

#ifndef _UPNPCONTROL_H
#define _UPNPCONTROL_H

#include <QObject>
#include <QMutex>

/* Forward declaration to make it build */
class UPNPControlThread;


class UPNPControl : public QObject
{
  Q_OBJECT

public:
  /** UPnP-related error values. */
  enum UPNPError {
    Success,
    NoUPNPDevicesFound,
    NoValidIGDsFound,
    WSAStartupFailed,
    AddPortMappingFailed,
    GetPortMappingFailed,
    DeletePortMappingFailed,
    UnknownError
  };
  /** UPnP port forwarding state. */
  enum UPNPState {
    IdleState,
    ErrorState,
    DiscoverState,
    UpdatingORPortState,
    UpdatingDirPortState,
    ForwardingCompleteState
  };

  /** Returns a pointer to this object's singleton instance. */
  static UPNPControl* instance();
  /** Terminates the UPnP control thread and frees memory allocated to this
   * object's singleton instance. */
  static void cleanup();
  /** Sets <b>desiredDirPort</b> and <b>desiredOrPort</b> to the currently
   * forwarded DirPort and ORPort values. */
  void getDesiredState(quint16 *desiredDirPort, quint16 *desiredOrPort);
  /** Sets the desired DirPort and ORPort port mappings to
   * <b>desiredDirPort</b> and <b>desiredOrPort</b>, respectively. */
  void setDesiredState(quint16 desiredDirPort, quint16 desiredOrPort);

  /** Returns the type of error that occurred last. */
  UPNPError error() const;
  /** Returns a QString describing the type of error that occurred last. */
  QString errorString() const;

  /** Returns the number of milliseconds to wait for devices to respond
   * when attempting to discover UPnP-enabled IGDs. */
  int discoverTimeout() const;

signals:
  /** Emitted when the UPnP control thread status changes. */
  void stateChanged(UPNPControl::UPNPState state);

  /** Emitted when a UPnP error occurs. */
  void error(UPNPControl::UPNPError error);

protected:
  /** Constructor. Initializes and starts a thread in which all blocking UPnP
   * operations will be performed. */
  UPNPControl();
  /** Destructor. cleanup() should be called before the object is destroyed. */
  ~UPNPControl();

  /** Sets the most recent UPnP-related error to <b>error</b> and emits the
   * error() signal.
   * \sa error
   */
  void setError(UPNPError error);

  /** Sets the current UPnP state to <b>state</b> and emits the stateChanged()
   * signal.
   * \sa stateChanged
   */
  void setState(UPNPState state);

private:
  static UPNPControl* _instance; /**< UPNPControl singleton instance. */

  quint16 _forwardedORPort; /**< Currently forwarded ORPort. */
  quint16 _forwardedDirPort; /**< Currently forwarded DirPort. */
  QMutex* _mutex; /**< Mutex around variables shared with UPNPControlThread. */
  UPNPError _error; /**< Most recent UPNP error. */
  UPNPState _state; /**< Current UPNP status. */

  friend class UPNPControlThread;
  UPNPControlThread* _controlThread; /**< Thread used for UPnP operations. */
};

#endif

