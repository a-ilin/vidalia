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
 * \file torevents.h
 * \version $Id$
 * \brief Parses and dispatches events from Tor
 */

#ifndef _TOREVENTS_H
#define _TOREVENTS_H

#include <QObject>
#include <QMultiHash>
#include <QList>

#include "eventtype.h"
#include "logevent.h"
#include "bandwidthevent.h"
#include "circuitevent.h"
#include "streamevent.h"
#include "orconnevent.h"
#include "newdescriptorevent.h"
#include "addressmapevent.h"
#include "statusevent.h"
#include "controlreply.h"


class TorEvents : public QObject
{
  Q_OBJECT

public:
  /** Asynchronous events sent from Tor to the controller */
  enum TorEvent {
    Unknown,
    Bandwidth,
    LogDebug, LogInfo, LogNotice, LogWarn, LogError,
    CircuitStatus,
    StreamStatus,
    OrConnStatus,
    NewDescriptor,
    AddressMap,
    GeneralStatus,
    ClientStatus,
    ServerStatus
  };
 
  /** Default Constructor */
  TorEvents();

  /** Adds an event and interested object to the event list */
  void add(TorEvent event, QObject *obj);
  /** Removes <b>obj</b> from the list of target objects for event 
   * <b>e</b>. */
  void remove(TorEvent event, QObject *obj);
  /** Returns true if an event has any registered handlers */
  bool contains(TorEvent event);
  /** Returns the list of events in which we're interested */
  QList<TorEvent> eventList();

  /** Parses an event message and emits the proper signal */
  void handleEvent(const ControlReply &reply);

  /** Dispatches a given event to all its handler targets. */
  void dispatch(TorEvent e, QEvent *event);
  
  /** Converts an Event to a string */
  static QString toString(TorEvents::TorEvent e);
  /** Converts a log severity to an event */
  static TorEvent toTorEvent(LogEvent::Severity severity);


private:
  /** Stores a mapping of Tor events to a list of the objects interested in
   * hearing about those events. */
  QMultiHash<TorEvent, QObject*> _eventList;
  
  /** Parses the event type from the event message */
  static TorEvent parseEventType(const ReplyLine &line);
  /** Converts a string to an Event */
  static TorEvent toTorEvent(const QString &event);
  
  /** Handle a bandwidth update event */
  void handleBandwidthUpdate(const ReplyLine &line);
  /** Handle a circuit status event */
  void handleCircuitStatus(const ReplyLine &line);
  /** Handle a stream status event */
  void handleStreamStatus(const ReplyLine &line);
  /** Handle a log message event */
  void handleLogMessage(const ReplyLine &line);
  /** Handle an OR connection status event. */
  void handleOrConnStatus(const ReplyLine &line);
  /** Handles a new list of descriptors event. */
  void handleNewDescriptor(const ReplyLine &line);
  /** Handles a new or updated address map event. */
  void handleAddressMap(const ReplyLine &line);
  /** Handles a Tor status event. */
  void handleStatusEvent(TorEvent type, const ReplyLine &line);
  
  /** Parses and posts a Tor client status event. */
  void dispatchClientStatusEvent(StatusEvent::Severity severity,
                                 const QString &action,
                                 const QHash<QString,QString> &args);
  /** Parses and posts a Tor server status event. */
  void dispatchServerStatusEvent(StatusEvent::Severity severity,
                                 const QString &action,
                                 const QHash<QString,QString> &args);
  /** Parses and posts a general Tor status event. */
  void dispatchGeneralStatusEvent(StatusEvent::Severity severity,
                                  const QString &action,
                                  const QHash<QString,QString> &args);

};

#endif

