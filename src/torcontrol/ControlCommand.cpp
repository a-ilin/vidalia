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
** \file ControlCommand.cpp
** \brief A command sent to Tor's control interface
*/

#include "ControlCommand.h"


/** Default constructor. */
ControlCommand::ControlCommand()
{
}

/** Creates a command using the specified keyword. */
ControlCommand::ControlCommand(const QString &keyword)
{
  _keyword = keyword;
}

/** Creates a control command using the specified keyword and argument. */
ControlCommand::ControlCommand(const QString &keyword, const QString &arg)
{
  _keyword = keyword;
  addArgument(arg);
}

/** Creates a control command using the specified keyword and list of
 * arguments. */
ControlCommand::ControlCommand(const QString &keyword, const QStringList &args)
{
  _keyword = keyword;
  _arguments = args;
}

/** Sets the keyword for this command. */
void
ControlCommand::setKeyword(const QString &keyword)
{
  _keyword = keyword;
}

/** Adds an argument to this command's argument list. */
void
ControlCommand::addArgument(const QString &arg)
{
  _arguments << arg;
}

/** Adds all arguments in <b>args</b> to this control command. */
void
ControlCommand::addArguments(const QStringList &args)
{
  foreach (QString arg, args) {
    addArgument(arg);
  }
}

/** Adds data to the end of this command. */
void
ControlCommand::appendData(const QString &data)
{
  _data << data;
}

/** Escapes any special characters in this command. */
QString
ControlCommand::escape(const QString &unescaped) const
{
  QString str = unescaped;
  if (str.startsWith(".")) {
    str.prepend(".");
  }
  if (str.endsWith("\r")) {
    str.append("\n");
  } else {
    str.append("\r\n");
  }
  return str;
}

/** Formats a command according to Tor's Control Protocol V1. The proper
 * format of a command is as follows:
 *
 *   Command = Keyword Arguments CRLF / "+" Keyword Arguments CRLF Data
 *   Keyword = 1*ALPHA
 *   Arguments = *(SP / VCHAR)
 */
QString
ControlCommand::toString() const
{
  int i;
  QString str;

  /* If this command contains data, then a "+" is prepended to the keyword */
  if (_data.size() > 0) {
    str = "+";
  }
  str += _keyword + " ";

  /* Append all specified arguments separated by a space */
  str += _arguments.join(" ");

  /* Append whatever data lines have been specified */
  if (_data.size() > 0) {
    str += "\r\n";
    for (i = 0; i < _data.size(); i++) {
      str += escape(_data.at(i));
    }
    str += ".";
  }
  return str.append("\r\n");
}

