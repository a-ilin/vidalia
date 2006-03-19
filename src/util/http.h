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
 * \file http.h
 * \version $Id$
 */

#ifndef _HTTP_H
#define _HTTP_H

#include <QHttp>


class Http : private QHttp
{
  Q_OBJECT

public:
  /** Constructor */
  Http(QString host, quint16 port = 80);

  /** Requests the given file from the specified host. */
  bool request(QString path);
  /** Returns the body of the HTML response. */
  QString body();

private:
  /** Contains the HTTP server's response after a successful request */
  QByteArray _response;  
};

#endif

