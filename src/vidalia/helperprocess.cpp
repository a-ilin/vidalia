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
**  This file was originally written by Steven J. Murdoch, and 
**  modified by Matt Edman. It is distributed under the following
**  license:
**
**  Copyright (C) 2007, Matt Edman
**  Copyright (C) 2007, Steven J. Murdoch 
**                      <http://www.cl.cam.ac.uk/users/sjm217/>
**
**  This program is free software; you can redistribute it and/or
**  modify it under the terms of the GNU General Public License
**  as published by the Free Software Foundation; either version 2
**  of the License, or (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program; if not, write to the Free Software
**  Foundation, Inc., 51 Franklin Street, Fifth Floor, 
**  Boston, MA  02110-1301, USA.
*/

/*
** \file helperprocess.cpp
** \version $Id$
** \brief Invokes a web browser process (originally by Steven. J. Murdoch)
*/

#include <QString>
  
#include "helperprocess.h"


/** Default constructor */
HelperProcess::HelperProcess(QObject *parent)
: QProcess(parent)
{
  // Call error handling routine on errors
  QObject::connect(this, SIGNAL(error(QProcess::ProcessError)),
                   this, SLOT(onError(QProcess::ProcessError)));
}

/** Start the specified application. */
void
HelperProcess::start(const QString &app, const QStringList &args) 
{
  // Start the specified application
  QProcess::start(app, args, QIODevice::ReadOnly | QIODevice::Text);
}

/** Invoked when underlying QProcess fails. */
void
HelperProcess::onError(QProcess::ProcessError error)
{
  // Pass up error messages on startup, but ignore the rest
  if (error == QProcess::FailedToStart) {
    emit startFailed(errorString());
  }
}


