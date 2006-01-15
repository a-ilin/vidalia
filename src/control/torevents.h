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

#include "controlreply.h"

class TorEvents : public QObject
{
  Q_OBJECT

public:
  /** Asynchronous events sent from Tor to the controller */
  enum Event {
    Unknown,
    Bandwidth,
    LogDebug, LogInfo, LogNotice, LogWarn, LogError,
    Circuit,
    Stream
  };
  /** Types of signals that can be connected to slots */
  enum Signal {
    BandwidthSignal,
    CircuitSignal,
    StreamSignal,
    LogSignal
  };
  /** Log message severity levels */
  enum LogSeverity {
    Debug,
    Info,
    Notice,
    Warn,
    Error
  };
  /** Circuit status events */
  enum CircuitStatus {
    CircuitLaunched,  /* Circuit ID assigned to new circuit */
    CircuitBuilt,     /* All hops finished */
    CircuitExtended,  /* Circuit extended by one hop */
    CircuitFailed,    /* Circuit closed (was not built) */
    CircuitClosed     /* Circuit closed (was built) */
  };
  /** Stream status events */
  enum StreamStatus {
    StreamNew,          /* New request to connect */
    StreamNewResolve,   /* New request to resolve an address */
    StreamSentConnect,  /* Sent a connect cell */
    StreamSentResolve,  /* Sent a resolve cell */
    StreamSucceeded,    /* Stream established */
    StreamFailed,       /* Stream failed */
    StreamClosed,       /* Stream closed */
    StreamDetached      /* Detached from circuit */
  };
 
  /** Default Constructor */
  TorEvents();
  /** Default Destructor */
  ~TorEvents();

  /** Connects a signal in this class to the specified slot */
  bool connect(Signal e, QObject *obj, const char *method);
  /** Disconnects a signal in this class from the specified slot */
  bool disconnect(Signal e, QObject *obj, const char *method);
  
  /** Parses an event message and emits the proper signal */
  void handleEvent(ControlReply reply);
 
  /** Converts an Event to a string */
  static QString toString(TorEvents::Event e);


signals:
  /** Reports the bandwidth used in the last second */
  void bandwidth(quint64 bytesRead, quint64 bytesWritten);
  /** Tor sent a log message to the controller */
  void log(LogSeverity severity, QString msg);
  /** Circuit status changed event */
  void circuit(quint64 circId, CircuitStatus status, QString path);
  /** Stream status changed event */
  void stream(quint64 streamId, StreamStatus status, 
              quint64 circId, QString target);


private:
  /** Converts a Signal enum to a SIGNAL() */
  const char* toSignal(Signal sig);

  /** Parses the event type from the event message */
  Event parseEventType(ReplyLine line);

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

