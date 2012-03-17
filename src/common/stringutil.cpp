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
** \file stringutil.cpp
** \brief Common string manipulation functions
*/

#include "stringutil.h"

#include <QCoreApplication>
#include <QApplication>


/** Create a QStringList from the array of C-style strings. */
QStringList
char_array_to_stringlist(char **arr, int len)
{
  QStringList list;
  for (int i = 0; i < len; i++) {
    list << QString(arr[i]);
  }
  return list;
}

/** Conditionally assigns errmsg to str if str is not null and returns false.
 * This is a seemingly pointless function, but it saves some messiness in
 * methods whose QString *errmsg parameter is optional. */
bool
err(QString *str, const QString &errmsg)
{
  if (str) {
    *str = errmsg;
  }
  return false;
}

/** Ensures all characters in str are in validChars. If a character appears
 * in str but not in validChars, it will be removed and the resulting
 * string returned. */
QString
ensure_valid_chars(const QString &str, const QString &validChars)
{
  QString out = str;
  for (int i = 0; i < str.length(); i++) {
    QChar c = str.at(i);
    if (validChars.indexOf(c) < 0) {
      out.remove(c);
    }
  }
  return out;
}

/** Scrubs an email address by replacing "@" with " at " and "." with " dot ". */
QString
scrub_email_addr(const QString &email)
{
  QString scrubbed = email;
  scrubbed = scrubbed.replace("@", " at ");
  scrubbed = scrubbed.replace(".", " dot ");
  return scrubbed;
}

/** Wraps <b>str</b> at <b>width</b> characters wide, using <b>sep</b> as the
 * word separator (" ", for example), and placing the line ending <b>le</b> at
 * the end of each line, except the last. */
QString
string_wrap(const QString &str, int width,
            const QString &sep, const QString &le)
{
  QString wrapped;
  int pos, nextsep, wordlen, n;
  int seplen = sep.length();

  if (str.length() < width) {
    return str;
  }

  pos = 0;
  n = width;
  while (pos < str.length()) {
    /* Get the length of a "word" */
    nextsep = str.indexOf(sep, pos);
    if (nextsep < 0) {
      nextsep = str.length();
    }
    wordlen = nextsep-pos;

    /* Check if there is room for the word on this line */
    if (wordlen > n) {
      /* Create a new line */
      wrapped.append(le);
      n = width;
    }

    /* Add the word to the current line */
    wrapped.append(str.mid(pos, wordlen+seplen));
    n = n - wordlen - seplen;
    pos += wordlen + seplen;
  }
  return wrapped.trimmed();
}

/** Encodes the bytes in <b>buf</b> as an uppercase hexadecimal string and
 * returns the result. This function is derived from base16_encode() in Tor's
 * util.c. See LICENSE for details on Tor's license. */
QString
base16_encode(const QByteArray &buf)
{
  QString hex;
  for (int i = 0; i < buf.size(); i++) {
    hex += "0123456789ABCDEF"[((quint8)buf[i]) >>  4];
    hex += "0123456789ABCDEF"[((quint8)buf[i]) & 0xf];
  }
  return hex;
}

/** Given an ASCII string <b>str</b>, this function returns a quoted string
 * with all escaped characters unescaped. Non-ASCII characters in the string
 * will be converted to the local 8-bit character encoding and encoded using
 * an escaped octal sequence. The returned string will thus contain only
 * printable ASCII characters. */
QString
string_escape(const QString &str)
{
  QByteArray in;
  QByteArray out;
  char c;

  in = str.toLocal8Bit();
  out.append('\"');
  for (int i = 0; i < in.length(); i++) {
    c = in[i];
    switch (c) {
      case '\"':
        out.append("\\\"");
        break;
      case '\\':
        out.append("\\\\");
        break;
      case '\n':
        out.append("\\n");
        break;
      case '\r':
        out.append("\\r");
        break;
      case '\t':
        out.append("\\t");
        break;
      default:
        if (QChar(c).isPrint() && c < 127) {
          out.append(c);
        } else {
          out.append('\\');
          out.append(QString::number(c, 8).toAscii());
        }
    }
  }
  out.append('\"');
  return QString::fromAscii(out);
}

/** Given a quoted string <b>str</b>, this function returns an unquoted,
 * unescaped string. <b>str</b> must start and end with an unescaped DQUOTE,
 * The input string must contain only ASCII characters; however, non-ASCII
 * characters can be included by encoding their byte sequences in either
 * escaped hexadecimal (e.g., "\xFF") or octal (e.g., "\301"). The result
 * will be converted to a QString using the local 8-bit encoding. */
QString
string_unescape(const QString &str, bool *ok)
{
  QByteArray out;
  int i;

  /* The string must start and end with an unescaped dquote */
  if (str.length() < 2)
    goto err;
  if (! str.startsWith("\"") || ! str.endsWith("\""))
    goto err;
  if (str.endsWith("\\\"") && ! str.endsWith("\\\\\""))
    goto err;

  i = 1;
  while (i < str.length()-1) {
    if (str[i] == QLatin1Char('\\')) {
      QChar c = str[++i];
      if (c == QLatin1Char('n')) {
        out.append('\n');
      } else if (c == QLatin1Char('r')) {
        out.append('\r');
      } else if (c == QLatin1Char('t')) {
        out.append('\t');
      } else if (c == QLatin1Char('x')) {
        if (i + 2 >= str.length())
          goto err;
        bool isHex;
        char val = static_cast<char>(str.mid(i+1, 2).toUInt(&isHex, 16));
        if (! isHex)
          goto err;
        out.append(val);
        i = i + 2;
      } else if (c.isDigit()) {
        if (i + 2 >= str.length())
          goto err;
        bool isOctal;
        uint val = str.mid(i, 3).toUInt(&isOctal, 8);
        if (! isOctal || val > 255)
          goto err;
        out.append(static_cast<char>(val));
        i = i + 2;
      } else {
        out.append(str[i].toLatin1());
      }
    } else if (str[i] == QLatin1Char('\"')) {
      /* Unescaped DQUOTE in the middle of the string, so terminate
       * processing and return a failure. */
      goto err;
    } else {
      out.append(str[i].toLatin1());
    }
    i++;
  }
  if (ok)
    *ok = true;
  return QString::fromLocal8Bit(out.data());

err:
  if (ok)
    *ok = false;
  return QString();
}

/** Parses a series of space-separated key[=value|="value"] tokens from
 * <b>str</b> and returns the mappings in a QHash. If <b>str</b> was unable
 * to be parsed, <b>ok</b> is set to false. */
QHash<QString,QString>
string_parse_keyvals(const QString &str, bool *ok)
{
  int i, len;
  bool tmp_ok;
  QHash<QString,QString> keyvals;

  i = 0;
  len = str.length();
  while (i < len && str[i].isSpace())
    i++; /* Skip initial whitespace */
  while (i < len) {
    QString key, val;

    while (i < len && !str[i].isSpace() && str[i] != '=')
      key.append(str[i++]);

    if (i < len && str[i] == '=') {
      if (++i < len && str[i] == '\"') {
        /* The value is wrapped in quotes */
        val.append(str[i]);
        while (++i < len) {
          val.append(str[i]);
          if (str[i] == '\\') {
            if (++i == len)
              goto error;
            val.append(str[i]);
          } else if (str[i] == '\"') {
            i++;
            break;
          }
        }
        val = string_unescape(val, &tmp_ok);
        if (!tmp_ok)
          goto error;
        keyvals.insert(key, val);
      } else {
        /* The value was not wrapped in quotes */
        while (i < len && !str[i].isSpace())
          val.append(str[i++]);
        keyvals.insert(key, val);
      }
    } else {
      /* The key had no value */
      keyvals.insert(key, QString(""));
    }
    while (i < len && str[i].isSpace())
      i++;
  }
  if (ok)
    *ok = true;
  return keyvals;

error:
  if (ok)
    *ok = false;
  return QHash<QString,QString>();
}

/** Parses a series of command line arguments from <b>str</b>. If <b>str</b>
 * was unable to be parsed, <b>ok</b> is set to false. */
QStringList
string_parse_arguments(const QString &str, bool *ok)
{
  QStringList args;
  int i, len;
  bool tmp_ok;

  i = 0;
  len = str.length();
  while (i < len && str[i].isSpace())
    i++; /* Skip initial whitespace */
  while (i < len) {
    QString arg;

    if (str[i] == '\"') {
      /* The value is wrapped in quotes */
      arg.append(str[i]);
      while (++i < len) {
        arg.append(str[i]);
        if (str[i] == '\\') {
          if (++i == len)
            goto error;
          arg.append(str[i]);
        } else if (str[i] == '\"') {
          i++;
          break;
        }
      }
      arg = string_unescape(arg, &tmp_ok);
      if (!tmp_ok)
        goto error;
      args << arg;
    } else {
      /* The value was not wrapped in quotes */
      while (i < len && !str[i].isSpace())
        arg.append(str[i++]);
      args << arg;
    }
    while (i < len && str[i].isSpace())
      i++;
  }

  if (ok)
    *ok = true;
  return args;

error:
  if (ok)
    *ok = false;
  return QStringList();
}

/** Formats the list of command line arguments in <b>args</b> as a string.
 * Arguments that contain ' ', '\', or '"' tokens will be escaped and
 * wrapped in double quotes. */
QString
string_format_arguments(const QStringList &args)
{
  QStringList out;
  foreach (QString arg, args) {
    if (arg.contains("\"") || arg.contains("\\") || arg.contains(" "))
      out << string_escape(arg);
    else
      out << arg;
  }
  return out.join(" ");
}

/** Returns true if <b>str</b> is a valid hexademical string. Returns false
 * otherwise. */
bool
string_is_hex(const QString &str)
{
  for (int i = 0; i < str.length(); i++) {
    char c = str[i].toUpper().toAscii();
    if ((c < 'A' || c > 'F') && (c < '0' || c > '9'))
      return false;
  }
  return true;
}

/** Returns a human-readable description of the time elapsed given by
 * <b>seconds</b>, broken down into days, hours, minutes and seconds. */
QString
string_format_uptime(quint64 seconds)
{
  QString uptime;
  int secs  = (seconds % 60);
  int mins  = (seconds / 60 % 60);
  int hours = (seconds / 3600 % 24);
  int days  = (seconds / 86400);

  if (days)
    uptime += qApp->translate("stringutil.h", "%1 days ").arg(days);
  if (hours)
    uptime += qApp->translate("stringutil.h", "%1 hours ").arg(hours);
  if (mins)
    uptime += qApp->translate("stringutil.h", "%1 mins ").arg(mins);
  if (secs)
    uptime += qApp->translate("stringutil.h", "%1 secs").arg(secs);

  return uptime;
}

/** Returns a string representation of <b>date</b> formatted according to
 * "yyyy-MM-dd HH:mm:ss". */
QString
string_format_datetime(const QDateTime &date)
{
  return date.toString("yyyy-MM-dd HH:mm:ss");
}

/** Returns a string representation of <b>bytes</b> with the appropriate
 * suffix of either "B/s", "KB/s", "MB/s" or "GB/s". */
QString
string_format_bandwidth(quint64 bytes)
{
  if (bytes < 1024)
    return qApp->translate("stringutil.h", "%1 B/s").arg(bytes);
  if (bytes < 1048576)
    return qApp->translate("stringutil.h", "%1 KB/s").arg(bytes/1024.0, 0, 'f', 2);
  if (bytes < 1073741824)
    return qApp->translate("stringutil.h", "%1 MB/s").arg(bytes/1048576.0, 0, 'f', 2);

  return qApp->translate("stringutil.h", "%1 GB/s").arg(bytes/1073741824.0, 0, 'f', 2);
}

