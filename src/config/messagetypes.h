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
 * \file messagetypes.h
 * \version $Id$
 */

/* Message Types */
#define MSG_TOR_ERROR  QT_TRANSLATE_NOOP("MessageTypes", "Error")
#define MSG_TOR_WARN   QT_TRANSLATE_NOOP("MessageTypes", "Warning")
#define MSG_TOR_NOTICE QT_TRANSLATE_NOOP("MessageTypes", "Notice")
#define MSG_TOR_INFO   QT_TRANSLATE_NOOP("MessageTypes", "Info")
#define MSG_TOR_DEBUG  QT_TRANSLATE_NOOP("MessageTypes", "Debug")

#define TOR_ERROR      (1u<<0)
#define TOR_WARN       (1u<<1)
#define TOR_NOTICE     (1u<<2)
#define TOR_INFO       (1u<<3)
#define TOR_DEBUG      (1u<<4)
