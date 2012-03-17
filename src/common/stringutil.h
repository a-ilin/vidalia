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
** \file stringutil.h
** \brief Common string manipulation functions
*/

#ifndef _STRINGUTIL_H
#define _STRINGUTIL_H

#include <QStringList>
#include <QHash>
#include <QDateTime>


/** Creates a QStringList from the array of C strings. */
QStringList char_array_to_stringlist(char **arr, int len);

/** Ensures all characters in str are in validChars. If a character appears
 * in str but not in validChars, it will be removed and the resulting
 * string returned. */
QString ensure_valid_chars(const QString &str, const QString &validChars);

/** Scrubs an email address by replacing "@" with " at " and "." with " dot ". */
QString scrub_email_addr(const QString &email);

/** Conditionally assigns errmsg to string if str is not null and returns
 * false. */
bool err(QString *str, const QString &errmsg);

/** Wraps <b>str</b> at <b>width</b> characters wide, using <b>sep</b> as the
 * word separator (" ", for example), and placing the line ending <b>le</b> at
 * the end of each line, except the last.*/
QString string_wrap(const QString &str, int width,
                    const QString &sep = QString(" "),
                    const QString &le = QString("\n"));

/** Encodes the bytes in <b>buf</b> as an uppercase hexadecimal string and
 * returns the result. This function is derived from base16_encode() in Tor's
 * util.c. See LICENSE for details on Tor's license. */
QString base16_encode(const QByteArray &buf);

/** Given a string <b>str</b>, this function returns a quoted string with all
 * '"' and '\' characters escaped with a single '\'. */
QString string_escape(const QString &str);

/** Given a quoted string <b>str</b>, this function returns an unquoted,
 * unescaped string. <b>str</b> must start and end with an unescaped quote. */
QString string_unescape(const QString &str, bool *ok = 0);

/** Parses a series of space-separated key[=value|="value"] tokens from
 * <b>str</b> and returns the mappings in a QHash. If <b>str</b> was unable
 * to be parsed, <b>ok</b> is set to false. */
QHash<QString,QString> string_parse_keyvals(const QString &str, bool *ok = 0);

/** Parses a series of command line arguments from <b>str</b>. If <b>str</b>
 * was unable to be parsed, <b>ok</b> is set to false. */
QStringList string_parse_arguments(const QString &str, bool *ok = 0);

/** Formats the list of command line arguments in <b>args</b> as a string.
 * Arguments that contain ' ', '\', or '"' tokens will be escaped and wrapped
 * in double quotes. */
QString string_format_arguments(const QStringList &args);

/** Returns true if <b>str</b> is a valid hexademical string. Returns false
 * otherwise. */
bool string_is_hex(const QString &str);

/** Returns a human-readable description of the time elapsed given by
 * <b>seconds</b>, broken down into days, hours, minutes and seconds. */
QString string_format_uptime(quint64 seconds);

/** Returns a string representation of <b>date</b> formatted according to
 * "yyyy-MM-dd HH:mm:ss". */
QString string_format_datetime(const QDateTime &date);

/** Returns a string representation of <b>bytes</b> with the appropriate
 * (localized) suffix of either "B/s", "KB/s", "MB/s" or "GB/s". */
QString string_format_bandwidth(quint64 bytes);

#endif

