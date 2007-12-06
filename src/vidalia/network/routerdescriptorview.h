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
 * \file routerdescriptorview.h
 * \version $Id$
 * \brief Formats and displays a router descriptor as HTML
 */

#ifndef _ROUTERDESCRIPTORVIEW_H
#define _ROUTERDESCRIPTORVIEW_H

#include <QObject>
#include <QTextEdit>
#include <QList>

#include <routerdescriptor.h>


class RouterDescriptorView : public QTextEdit
{
  Q_OBJECT

public:
  /** Default constructor. */
  RouterDescriptorView(QWidget *parent = 0);

public slots:
  /** Shows the given router descriptor. */
  void display(RouterDescriptor rd);
  /** Shows all router descriptors in the given list. */
  void display(QList<RouterDescriptor> rdlist);
  
private:
  /** Adjusts the displayed uptime to include time since the
   * router's descriptor was last published. */
  quint64 adjustUptime(quint64 uptime, QDateTime published);
  /** Formats the descriptor's published date. */
  QString formatPublished(QDateTime date);
  /** Formats the router's uptime. */
  QString formatUptime(quint64 seconds);
  /** Formats the observed bandwidth into KB/s. */
  QString formatBandwidth(quint64 bandwidth);
};

#endif

