/****************************************************************
*  $Id$
* 
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

#ifndef _TOREVENTS_H
#define _TOREVENTS_H

#include <QObject>
#include <QMultiHash>
#include <QList>

#include "circuitevent.h"
#include "streamevent.h"
#include "logevent.h"
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
    Circuit,
    Stream
  };
  /** Signals that can be connected to interested slots */
  enum Signal {
    BandwidthSignal,
    CircuitSignal,
    LogSignal,
    StreamSignal,
  };
  /** Default Constructor */
  TorEvents();
  /** Default Destructor */
  ~TorEvents();

  /** Adds an event to the event list */
  void add(TorEvent event);
  /** Removes an event from the event list */
  void remove(TorEvent event);
  /** Returns true if an event is in the list */
  bool contains(TorEvent event);
  /** Returns the list of events in which we're interested */
  QList<TorEvent> eventList();
  /** Connects a slot to one of the Tor event signals */
  bool connect(Signal signal, QObject *obj, const char *method);
  /** Disconnects a slot from one of the Tor event signals */
  bool disconnect(Signal signal, QObject *obj, const char *method);

  /** Parses an event message and emits the proper signal */
  void handleEvent(ControlReply reply);
 
  /** Converts an Event to a string */
  static QString toString(TorEvents::TorEvent e);

signals:
  /** Emitted when a bandwidth update is sent from Tor */
  void bandwidth(quint64 bytesRead, quint64 bytesWritten);
  /** Emitted when a log message is sent from Tor */
  void log(LogEvent::Severity severity, QString message);
  /** Emitted when a circuit status update is sent from Tor */
  void circuit(quint64 circId, CircuitEvent::Status status, QString path);
  /** Emitted when a stream status update is sent from Tor */
  void stream(quint64 streamId, StreamEvent::Status status,
              quint64 circId, QString targetAddr);

private:
  /** A list of events in which we're currently interested */
  QList<TorEvent> _eventList;
  
  /** Parses the event type from the event message */
  static TorEvent parseEventType(ReplyLine line);

  /** Converts a string to an Event */
  static TorEvent toTorEvent(QString event);
  /** Converts a log severity to an event */
  static TorEvent toTorEvent(LogEvent::Severity severity);
  /** Converts a Signal enum value to a SIGNAL() */
  const char *toSignal(Signal s);
  
  /** Handle a bandwidth update event */
  void handleBandwidthUpdate(ReplyLine line);
  /** Handle a circuit status event */
  void handleCircuitStatus(ReplyLine line);
  /** Handle a stream status event */
  void handleStreamStatus(ReplyLine line);
  /** Handle a log message event */
  void handleLogMessage(ReplyLine line);
};

#endif

