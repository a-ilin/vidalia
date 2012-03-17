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
** \file ControlCommand.h
** \brief A command sent to Tor's control interface
*/

#ifndef _CONTROLCOMMAND_H
#define _CONTROLCOMMAND_H

#include <QStringList>


class ControlCommand
{
public:
  ControlCommand();
  ControlCommand(const QString &keyword);
  ControlCommand(const QString &keyword, const QString &arg);
  ControlCommand(const QString &keyword, const QStringList &args);

  /** Returns the keyword for this control command. */
  QString keyword() const { return _keyword; }

  /** Set the keyword for this control command */
  void setKeyword(const QString &keyword);

  /** Add an argument to this control command */
  void addArgument(const QString &arg);
  /** Adds all arguments in <b>args</b> to this control command. */
  void addArguments(const QStringList &args);

  /** Append a data line for this control command */
  void appendData(const QString &data);

  /** Format this control command into a format conforming to Tor's v1
   * protocol specification. */
  QString toString() const;

private:
  /** Escape special characters in the supplied string */
  QString escape(const QString &str) const;

  QString _keyword;
  QStringList _arguments;
  QStringList _data;
};

#endif

