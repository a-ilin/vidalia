/****************************************************************
 *  Vidalia is distributed under the following license:
 *
 *  Copyright (C) 2006,  Matt Edman, Justin Hipple
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, 
 *  Boston, MA  02110-1301, USA.
 ****************************************************************/

/** 
 * \file string.h
 * \version $Id$
 * \brief Common string manipulation functions
 */

#ifndef __STRING_H
#define __STRING_H

#include <QStringList>
#include <QHash>


/** Creates a QStringList from the array of C strings. */
QStringList char_array_to_stringlist(char **arr, int len);

/** Ensures all characters in str are in validChars. If a character appears
 * in str but not in validChars, it will be removed and the resulting
 * string returned. */
QString ensure_valid_chars(QString str, QString validChars);

/** Scrubs an email address by replacing "@" with " at " and "." with " dot ". */
QString scrub_email_addr(QString email);

/** Conditionally assigns errmsg to string if str is not null and returns
 * false. */
bool err(QString *str, QString errmsg);

/** Wraps <b>str</b> at <b>width</b> characters wide, using <b>sep</b> as the
 * word separator (" ", for example), and placing the line ending <b>le</b> at
 * the end of each line, except the last.*/
QString string_wrap(QString str, int width, QString sep, QString le);

/** Encodes the bytes in <b>buf</b> as an uppercase hexadecimal string and
 * returns the result. This function is derived from base16_encode() in Tor's
 * util.c. See LICENSE for details on Tor's license. */
QString base16_encode(const QByteArray buf);

/** Given a string <b>str</b>, this function returns a quoted string with all
 * '"' and '\' characters escaped with a single '\'. */
QString string_escape(const QString str);

/** Given a quoted string <b>str</b>, this function returns an unquoted,
 * unescaped string. <b>str</b> must start and end with an unescaped quote. */
QString string_unescape(const QString str, bool *ok = 0);

/** Parses a series of space-separated key[=value|="value"] tokens from
 * <b>str</b> and returns the mappings in a QHash. If <b>str</b> was unable
 * to be parsed, <b>ok</b> is set to false. */
QHash<QString,QString> string_parse_keyvals(const QString str, bool *ok = 0);

#endif

