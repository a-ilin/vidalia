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
 
  /** Default Constructor */
  TorEvents();
  /** Default Destructor */
  ~TorEvents();

  /** Parses an event message and emits the proper signal */
  static void handleEvent(ControlReply reply);
 
  /** Converts an Event to a string */
  static QString toString(TorEvents::Event e);

  /** Converts a string to an Event */
  static Event toEvent(QString event);

private:
  /** Parses the event type from the event message */
  static Event parseEventType(ReplyLine line);

  /** Handle a bandwidth update event */
  static void handleBandwidthUpdate(ReplyLine line);
  /** Handle a circuit status event */
  static void handleCircuitStatus(ReplyLine line);
  /** Handle a stream status event */
  static void handleStreamStatus(ReplyLine line);
  /** Handle a log message event */
  static void handleLogMessage(ReplyLine line);
};

#endif

