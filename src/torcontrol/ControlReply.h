/*
**  This file is part of Vidalia, and is subject to the license terms in the
**  LICENSE file, found in the top level directory of this distribution. If
**  you did not receive the LICENSE file with this file, you may obtain it
**  from the Vidalia source package distributed by the Vidalia Project at
**  http://www.torproject.org/projects/vidalia.html. No part of Vidalia,
**  including this file, may be copied, modified, propagated, or distributed
**  except according to the terms described in the LICENSE file.
*/

/*
** \file ControlReply.h
** \brief A response from Tor's control interface
*/

#ifndef _CONTROLREPLY_H
#define _CONTROLREPLY_H

#include "ReplyLine.h"

#include <QStringList>


class ControlReply
{
public:
  /** Default constructor */
  ControlReply();

  /** Add a line associated with this reply */
  void appendLine(ReplyLine line);

  /** Returns a single line from this reply */
  ReplyLine getLine(int idx = 0) const;
  /** Returns all lines for this reply */
  QList<ReplyLine> getLines() const;

  /** Returns the status of the first line in the reply */
  QString getStatus() const;
  /** Returns the messasge of the first line in the reply */
  QString getMessage() const;
  /** Returns the data for the first line in the reply. */
  QStringList getData() const;
  /** Returns the entire contents of the control reply. */
  QString toString() const;

private:
  QList<ReplyLine> _lines;
};

#endif

