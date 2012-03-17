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
** \file LogEvent.h
** \brief Event dispatched containing a log message from Tor
*/

#ifndef _LOGEVENT_H
#define _LOGEVENT_H

#include <QCoreApplication>
#include <QString>
#include <QEvent>


class LogEvent : public QEvent
{
  Q_DECLARE_TR_FUNCTIONS(LogEvent)

public:
  /** Log message severity levels */
  enum Severity {
    Unknown = 0,
    Debug      = (1u<<4),  /**< Debug level log message. */
    Info       = (1u<<3),  /**< Info level log message. */
    Notice     = (1u<<2),  /**< Notice level log message. */
    Warn       = (1u<<1),  /**< Warn level log message. */
    Error      = (1u<<0)   /**< Error level log message. */
  };

  /** Default constructor */
  LogEvent(Severity severity, QString message);

  /** Converts the string description of a severity to its enum value */
  static Severity toSeverity(QString strSeverity);
  /** Converts the Severity enum value to a string description */
  static QString severityToString(Severity severity);

  /** Returns the severity of this log event */
  Severity severity() const;
  /** Returns the message for this log event */
  QString message() const;

private:
  Severity _severity;
  QString  _message;
};

#endif

