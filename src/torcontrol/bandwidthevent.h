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
** \file bandwidthevent.h
** \version $Id$
** \brief Event dispatched upon receiving a bandwidth update from Tor
*/

#ifndef _BANDWIDTHEVENT_H
#define _BANDWIDTHEVENT_H

#include <QEvent>


class BandwidthEvent : public QEvent
{
public:
  /** Constructor */
  BandwidthEvent(quint64 bytesRead, quint64 bytesWritten)
  : QEvent((QEvent::Type)CustomEventType::BandwidthEvent)
  { _bytesRead = bytesRead; _bytesWritten = bytesWritten; }

  /** Returns the number of bytes read in the last second */
  quint64 bytesRead() const { return _bytesRead; }
  /** Returns the number of bytes written in the last second */
  quint64 bytesWritten() const { return _bytesWritten; }
  
private:
  quint64 _bytesWritten;
  quint64 _bytesRead;
};

#endif

