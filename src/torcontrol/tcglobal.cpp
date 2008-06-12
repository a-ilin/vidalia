
#include "tcglobal.h"


namespace tc {

/** Creates a new message using <b>fmt</b> and a severity level of
 * QtDebugMsg. */
DebugMessage
debug(const QString &fmt)
{
  return DebugMessage(QtDebugMsg, fmt);
}

/** Creates a new message using <b>fmt</b> and a severity level of
 * QtWarningMsg. */
DebugMessage
warn(const QString &fmt)
{
  return DebugMessage(QtWarningMsg, fmt);
}

/** Creates a new message using <b>fmt</b> and a severity level of
 * QtCriticalMsg. */
DebugMessage
error(const QString &fmt)
{
  return DebugMessage(QtCriticalMsg, fmt);
}

/** Creates a new message using <b>fmt</b> and a severity level of
 * QtFatalMsg. */
DebugMessage
fatal(const QString &fmt)
{
  return DebugMessage(QtFatalMsg, fmt);
}

/** Converts <b>str</b> to a ConnectionStatusReason enum value. */
ConnectionStatusReason
connectionStatusReason(const QString &str)
{
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

}

