/****************************************************************
 *  This file is distributed under the following license:
 *
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
 ** Test invoking Firefox from Qt
 ** Steven J. Murdoch <http://www.cl.cam.ac.uk/users/sjm217/>
 ** $Id$
 **/

#include <QString>
  
#include "browserprocess.h"

BrowserProcess::BrowserProcess(QObject *parent)
: QProcess(parent)
{
  // Call error handling routine on errors
  QObject::connect(this, SIGNAL(error(QProcess::ProcessError)), this, SLOT(onError(QProcess::ProcessError)));
}

void
BrowserProcess::start(QString app, QStringList args) 
{
  // Start the specified application
  QProcess::start(app, args, QIODevice::ReadOnly | QIODevice::Text);
}

void
BrowserProcess::onError(QProcess::ProcessError error)
{
  // Pass up error messages on startup, but ignore the rest
  if (error == QProcess::FailedToStart) {
    emit startFailed(errorString());
  }
}
