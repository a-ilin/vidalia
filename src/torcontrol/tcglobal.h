
#ifndef _TCGLOBAL_H
#define _TCGLOBAL_H

#include <QString>

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
  
  /** Converts <b>str</b> to a ConnectionStatusReason enum value. */
  ConnectionStatusReason connectionStatusReason(const QString &str);

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

#endif

