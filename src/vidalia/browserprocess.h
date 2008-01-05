/****************************************************************
 *  This file was originally written by Steven J. Murdoch, and 
 *  modified by Matt Edman. It is distributed under the following
 *  license:
 * 
 *  Copyright (C) 2007, Matt Edman
 *  Copyright (C) 2007, Steven J. Murdoch 
 *                      <http://www.cl.cam.ac.uk/users/sjm217/>
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
 * \file browserprocess.cpp
 * \version $Id$ 
 * \brief Invokes a web browser process (originally by Steven. J. Murdoch)
 */

#ifndef _BROWSERPROCESS_H
#define _BROWSERPROCESS_H

#include <QProcess>


class BrowserProcess : public QProcess
{
  Q_OBJECT

public:
  /** Default constructor */
  BrowserProcess(QObject *parent = 0);
  /** Start the specified application. */
  void start(const QString &app, const QStringList &args);

private slots:
  /** Invoked when underlying QProcess fails. */
  void onError(QProcess::ProcessError error);

signals:
  /** Invoked when start() fails. */
  void startFailed(const QString &errorMessage);
};

#endif
