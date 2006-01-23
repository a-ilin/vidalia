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

#ifndef _STREAMEVENT_H
#define _STREAMEVENT_H

#include <QString>

class StreamEvent
{
public:
  /** Stream status events */
  enum Status {
    Unknown,      /* Unknown status type given */
    New,          /* New request to connect */
    NewResolve,   /* New request to resolve an address */
    SentConnect,  /* Sent a connect cell */
    SentResolve,  /* Sent a resolve cell */
    Succeeded,    /* Stream established */
    Failed,       /* Stream failed */
    Closed,       /* Stream closed */
    Detached      /* Detached from circuit */
  };

  /** Converts a string description of a stream's status to its enum value */
  static Status toStatus(QString strStatus);
};

#endif

