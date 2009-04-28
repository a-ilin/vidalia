/*
**  This file is part of Vidalia, and is subject to the license terms in the
**  LICENSE file, found in the top level directory of this distribution. If 
**  you did not receive the LICENSE file with this file, you may obtain it
**  from the Vidalia source package distributed by the Vidalia Project at
**  http://www.vidalia-project.net/. No part of Vidalia, including this file,
**  may be copied, modified, propagated, or distributed except according to
**  the terms described in the LICENSE file.
*/

/*
** \file NewDescriptorEvent.h
** \version $Id$
** \brief Event dispatched upon receiving a new descriptor from Tor
*/

#ifndef _NEWDESCRIPTOREVENT_H
#define _NEWDESCRIPTOREVENT_H

#include <QStringList>
#include <QEvent>


class NewDescriptorEvent : public QEvent
{
public:
  /** Default constructor */
  NewDescriptorEvent(QStringList idList)
  : QEvent((QEvent::Type)CustomEventType::NewDescriptorEvent)
  { _idList = idList; }
  
  /** Returns a list of new server IDs. */
  QStringList descriptorIDs() const { return _idList; }
  
private:
  /** A list of new descriptors available. */
  QStringList _idList;
};

#endif

