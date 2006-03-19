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
 * \file http.cpp
 * \version $Id$
 */

#include <QCoreApplication>

#include "http.h"

/** Constructor */
Http::Http(QString host, quint16 port)
: QHttp(host, port)
{
}

/** Executes a single request for the given file. User input is blocked while
 * the request is executing, so this should only be used for very small requests.
 * \param path the file to request.
 * \return true if the request executed successfully.
 */
bool
Http::request(QString path)
{
  get(path);
  
  while (hasPendingRequests() || currentId() != 0) {
    QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
  }
  if (bytesAvailable() > 0) {
    _response = readAll();
    return true;
  }
  return false;
}

/** Returns the body of an HTTP response. If no body tags are present, then
 * the whole response is returned. */
QString
Http::body()
{
  int length, end;
  int start = _response.indexOf("<body>");
  if (start >= 0) {
    start += 6;
    end = _response.indexOf("</body>");
    length = (end > 0 ? end-start : -1);      
    return _response.mid(start, length);
  }
  return _response;
}

