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
** \file ControlReply.cpp
** \brief A response from Tor's control interface
*/

#include "ControlReply.h"


/** Default constructor */
ControlReply::ControlReply()
{
}

/** Add a line associated with this reply */
void
ControlReply::appendLine(ReplyLine line)
{
  _lines << line;
}

/** Returns the requested line from this reply */
ReplyLine
ControlReply::getLine(int idx) const
{
  return _lines.at(idx);
}

/** Returns all lines for this reply */
QList<ReplyLine>
ControlReply::getLines() const
{
  return _lines;
}

/** Returns the status of the first line in the reply */
QString
ControlReply::getStatus() const
{
  return getLine().getStatus();
}

/** Returns the message of the first line in the reply */
QString
ControlReply::getMessage() const
{
  return getLine().getMessage();
}

/** Returns the data for the first line in the reply. */
QStringList
ControlReply::getData() const
{
  return getLine().getData();
}

/** Returns the entire contents of the control reply. */
QString
ControlReply::toString() const
{
  QString str;
  foreach (ReplyLine line, _lines) {
    str.append(line.toString());
    str.append("\n");
  }
  return str.trimmed();
}

