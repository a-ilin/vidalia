/*
**  This file is part of Vidalia, and is subject to the license terms in the
**  LICENSE file, found in the top level directory of this distribution. If you
**  did not receive the LICENSE file with this file, you may obtain it from the
**  Vidalia source package distributed by the Vidalia Project at
**  http://www.vidalia-project.net/. No part of Vidalia, including this file,
**  may be copied, modified, propagated, or distributed except according to the
**  terms described in the LICENSE file.
*/

/*
** \file routerdescriptorview.cpp
** \version $Id$
** \brief Formats and displays a router descriptor as HTML
*/

#include <html.h>
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

/** Adjusts the displayed uptime to include time since the router's descriptor
 * was last published. */
quint64
RouterDescriptorView::adjustUptime(quint64 uptime, QDateTime published)
{
  QDateTime now = QDateTime::currentDateTime().toUTC();
  
  if (now < published) {
    return uptime;
  }
  return (uptime + (now.toTime_t() - published.toTime_t()));
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
    uptime += tr("%1 days ").arg(days);
  }
  if (hours) {
    uptime += tr("%1 hours ").arg(hours);
  }
  if (mins) {
    uptime += tr("%1 mins ").arg(mins);
  }
  if (secs) {
    uptime += tr("%1 secs").arg(secs);
  }
  return uptime;
}

/** Format the bandwidth into KB/s. */
QString
RouterDescriptorView::formatBandwidth(quint64 bandwidth)
{
  return QString::number(bandwidth/1024);
}

/** Displays all router descriptors in the given list. */
void
RouterDescriptorView::display(QList<RouterDescriptor> rdlist)
{
  RouterDescriptor rd;
  QString html = "<html><body>";
  
  for (int r = 0; r < rdlist.size(); r++) { 
    rd = rdlist.at(r);
    
    /* Router name and status */
    html.append(p(b(rd.name()) + " (" + i(rd.status()) + ")"));

    /* IP and platform */
    html.append("<table>");
    
    /* If we have location information, show that first. */
    if (!rd.location().isEmpty()) {
      html.append(trow(tcol(b(tr("Location:"))) + tcol(rd.location())));
    }
    
    /* Add the IP address and router platform information */
    html.append(trow(tcol(b(tr("IP Address:"))) + tcol(rd.ip().toString())));
    html.append(trow(tcol(b(tr("Platform:")))   + tcol(rd.platform())));

    /* If the router is online, then show the uptime and bandwidth stats. */
    if (!rd.offline()) {
      html.append(trow(tcol(b(tr("Bandwidth:")))  + 
                       tcol(formatBandwidth(rd.observedBandwidth()) + " KB/s")));
      html.append(trow(tcol(b(tr("Uptime:")))   + 
                       tcol(formatUptime(
                              adjustUptime(rd.uptime(), rd.published())))));
    }
    
    /* Date the router was published */
    html.append(trow(tcol(b(tr("Last Updated:")))  +
                     tcol(formatPublished(rd.published()))));
    
    html.append("</table>");
    
    /* If there are multiple descriptors, and this isn't is the last one 
     * then separate them with a short horizontal line. */
    if (r+1 != rdlist.size()) {
      html.append("<center><hr width=\"50%\"/></center>");
    }
  }
  html.append("</body></html>");
  setHtml(html); 
}

/** Displays the given router descriptor. */
void
RouterDescriptorView::display(RouterDescriptor rd)
{
  display(QList<RouterDescriptor>() << rd);
}

