/*
**  This file is part of Vidalia, and is subject to the license terms in the
**  LICENSE file, found in the top level directory of this distribution. If you
**  did not receive the LICENSE file with this file, you may obtain it from the
**  Vidalia source package distributed by the Vidalia Project at
**  http://www.torproject.org/projects/vidalia.html. No part of Vidalia,
**  including this file, may be copied, modified, propagated, or distributed
**  except according to the terms described in the LICENSE file.
*/

/*
** \file tcglobal.cpp
** \brief Provides common methods and constants used by the torcontrol library
*/

#include "tcglobal.h"


namespace tc {

/* Creates a new message using <b>fmt</b> and a severity level of
 * QtDebugMsg. */
DebugMessage
debug(const QString &fmt)
{
  return DebugMessage(QtDebugMsg, fmt);
}

/* Creates a new message using <b>fmt</b> and a severity level of
 * QtWarningMsg. */
DebugMessage
warn(const QString &fmt)
{
  return DebugMessage(QtWarningMsg, fmt);
}

/* Creates a new message using <b>fmt</b> and a severity level of
 * QtCriticalMsg. */
DebugMessage
error(const QString &fmt)
{
  return DebugMessage(QtCriticalMsg, fmt);
}

/* Creates a new message using <b>fmt</b> and a severity level of
 * QtFatalMsg. */
DebugMessage
fatal(const QString &fmt)
{
  return DebugMessage(QtFatalMsg, fmt);
}

/* Converts <b>str</b> to a ConnectionStatusReason enum value. */
ConnectionStatusReason
connectionStatusReasonFromString(const QString &str)
{
  if (str.isEmpty())
    return UnrecognizedReason;
  if (!str.compare("MISC", Qt::CaseInsensitive))
    return MiscellaneousReason;
  if (!str.compare("IDENTITY", Qt::CaseInsensitive))
    return IdentityMismatch;
  if (!str.compare("RESOURCELIMIT", Qt::CaseInsensitive))
    return ResourceLimitReached;
  if (!str.compare("DONE", Qt::CaseInsensitive))
    return ConnectionDone;
  if (!str.compare("CONNECTREFUSED"))
    return ConnectionRefused;
  if (!str.compare("CONNECTRESET", Qt::CaseInsensitive))
    return ConnectionRefused;
  if (!str.compare("TIMEOUT", Qt::CaseInsensitive))
    return ConnectionTimeout;
  if (!str.compare("NOROUTE", Qt::CaseInsensitive))
    return NoRouteToHost;
  if (!str.compare("IOERROR", Qt::CaseInsensitive))
    return ConnectionIoError;
  return UnrecognizedReason;
}

/* Converts <b>str</b> to a Severity enum value. */
Severity
severityFromString(const QString &str)
{
  if (!str.compare("DEBUG", Qt::CaseInsensitive))
    return DebugSeverity;
  if (!str.compare("INFO", Qt::CaseInsensitive))
    return InfoSeverity;
  if (!str.compare("NOTICE", Qt::CaseInsensitive))
    return NoticeSeverity;
  if (!str.compare("WARN", Qt::CaseInsensitive))
    return WarnSeverity;
  if (!str.compare("ERR", Qt::CaseInsensitive))
    return ErrorSeverity;
  return UnrecognizedSeverity;
}

}

