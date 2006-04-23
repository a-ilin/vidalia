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
 * \file routerdescriptorview.cpp
 * \version $Id$
 */

#include <util/html.h>
#include "routerdescriptorview.h"

#define DATE_FORMAT   "yyyy-MM-dd HH:mm:ss"

/** Default constructor. */
RouterDescriptorView::RouterDescriptorView(QWidget *parent)
: QTextEdit(parent)
{
}

/** Format the date the descriptor was published. */
QString
RouterDescriptorView::formatPublished(QDateTime date)
{
  return date.toString(DATE_FORMAT) + " GMT";
}

/** Format the uptime for this router in a readable format. */
QString
RouterDescriptorView::formatUptime(quint64 seconds)
{
  QString uptime;
  int secs  = (seconds % 60);
  int mins  = (seconds / 60 % 60);
  int hours = (seconds / 3600 % 24);
  int days  = (seconds / 86400);

  if (days) {
    uptime += QString("%1 days ").arg(days);
  }
  if (hours) {
    uptime += QString("%1 hours ").arg(hours);
  }
  if (mins) {
    uptime += QString("%1 mins ").arg(mins);
  }
  if (secs) {
    uptime += QString("%1 secs").arg(secs);
  }
  return uptime;
}

/** Format the bandwidth into KB/s. */
QString
RouterDescriptorView::formatBandwidth(quint64 bandwidth)
{
  return QString::number(bandwidth/1024);
}

/** Displays the given router descriptor. */
void
RouterDescriptorView::display(RouterDescriptor rd)
{
  QString html = "<html><body>";

  /* Router name and status */
  html.append(p(b(rd.name()) + " (" + i(rd.status()) + ")"));
  
  /* IP and platform */
  html.append("<table>");
  html.append(trow(tcol(b(tr("IP Address:"))) + tcol(rd.ip())));
  html.append(trow(tcol(b(tr("Platform:")))   + tcol(rd.platform())));

  /* If there is contact information, then show it */
  if (!rd.contact().isEmpty()) {
    html.append(trow(tcol(b(tr("Contact:")))    + tcol(escape(rd.contact()))));
  }
  
  /* Date the router was published */
  html.append(trow(tcol(b(tr("Published:")))  +
                   tcol(formatPublished(rd.published()))));

  /* If the router is online, then show the uptime and bandwidth stats. */
  if (!rd.offline()) {
    html.append(trow(tcol(b(tr("Uptime:")))   + 
                     tcol(formatUptime(rd.uptime()))));
    html.append(trow(tcol(b(tr("Bandwidth:")))  + 
                     tcol(formatBandwidth(rd.observedBandwidth()) + " KB/s")));
  }
  
  html.append("</table>");
  html.append("</body></html>");
  setHtml(html); 
}

