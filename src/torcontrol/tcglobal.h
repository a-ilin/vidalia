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
** \file tcglobal.h
** \brief Provides common methods and constants used by the torcontrol library
*/

#ifndef _TCGLOBAL_H
#define _TCGLOBAL_H

#include <QString>
#include <QMetaType>

namespace tc {
  /** Helper class to handle formatting log messages with arguments. */
  class DebugMessage {
    struct Stream {
      Stream(QtMsgType t, const QString &fmt)
        : type(t), buf(fmt), ref(1) {}
      QtMsgType type; /**< DebugMessage severity level. */
      QString buf;    /**< Log message buffer. */
      int ref;        /**< Reference counter. */
    } *stream;

  public:
    /** Constructs a new DebugMessage with severity <b>t</b> and the message
     * format <b>fmt</b>. */
    inline DebugMessage(QtMsgType t, const QString &fmt)
      : stream(new Stream(t, fmt)) {}
    inline DebugMessage(const DebugMessage &o)
      : stream(o.stream) { ++stream->ref; }
    virtual ~DebugMessage() {
      if (!--stream->ref) {
        stream->buf.prepend("torcontrol: ");
        qt_message_output(stream->type, qPrintable(stream->buf));
        delete stream;
      }
    }

    inline DebugMessage arg(const QString &a)
      { stream->buf = stream->buf.arg(a); return *this; }
    inline DebugMessage arg(int a)
      { stream->buf = stream->buf.arg(a); return *this; }
  };
}

namespace tc {
  enum ConnectionStatusReason {
    UnrecognizedReason,
    MiscellaneousReason,
    IdentityMismatch,
    ConnectionDone,
    ConnectionRefused,
    ConnectionReset,
    ConnectionTimeout,
    ConnectionIoError,
    NoRouteToHost,
    ResourceLimitReached
  };
  /** Severity values used in log message and status events. */
  enum Severity {
    UnrecognizedSeverity = 0, /**< An unrecognized severity value. */
    DebugSeverity  = (1u<<4), /**< Hyper-verbose events used for debugging. */
    InfoSeverity   = (1u<<3), /**< Verbose events that can occur frequently. */
    NoticeSeverity = (1u<<2), /**< A not-so-bad event. */
    WarnSeverity   = (1u<<1), /**< An important, but non-fatal event. */
    ErrorSeverity  = (1u<<0)  /**< A critical event. */
  };
  /** SOCKS error types used by Tor status event notifications. These are
   * emitted in the TorControl::socksError() signal. */
  enum SocksError {
    DangerousSocksTypeError, /**< The SOCKS type uses only IP addresses. */
    UnknownSocksProtocolError, /**< Unknown SOCKS protocol type. */
    BadSocksHostnameError /**< Application provided an invalid hostname. */
  };
  /** Reasons that use of the user's current Tor version would be
   * discouraged. */
  enum TorVersionStatus {
    ObsoleteTorVersion,
    UnrecommendedTorVersion,
    NewTorVersion
  };

  /** Converts <b>str</b> to a Severity enum value. */
  Severity severityFromString(const QString &str);

  /** Converts <b>str</b> to a ConnectionStatusReason enum value. */
  ConnectionStatusReason connectionStatusReasonFromString(const QString &str);

  /** Creates a new message using <b>fmt</b> and a severity level of
   * QtDebugMsg. */
  DebugMessage debug(const QString &fmt);

  /** Creates a new message using <b>fmt</b> and a severity level of
   * QtWarningMsg. */
  DebugMessage warn(const QString &fmt);

  /** Creates a new message using <b>fmt</b> and a severity level of
   * QtCriticalMsg. */
  DebugMessage error(const QString &fmt);

  /** Creates a new message using <b>fmt</b> and a severity level of
   * QtFatalMsg. */
  DebugMessage fatal(const QString &fmt);
}

Q_DECLARE_METATYPE(tc::Severity)
Q_DECLARE_METATYPE(tc::SocksError)
Q_DECLARE_METATYPE(tc::TorVersionStatus)

#endif

