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
 * \file mainwindow.h
 * \version $Id$
 *
 * Functions in this file contain system calls with enough #ifdef mojo to be
 * platform-independent. They should be differentiated from the real system
 * calls by prepending each function's name with a "v_" (the v stands for
 * "vidalia").
 */

#include <QtGlobal>
 
#if defined(Q_OS_WIN32)
#include <windows.h>
#else
#include <unistd.h>
#endif

#include "compat.h"


/** Causes the calling process to sleep for the specified number of seconds */
void
v_sleep(quint32 seconds)
{
#if defined(Q_OS_WIN32)
  /* Sleep() on win32 is measured in milliseconds instead of seconds */
  Sleep((DWORD)(seconds * 1000));
#else
  sleep(seconds);
#endif
}
