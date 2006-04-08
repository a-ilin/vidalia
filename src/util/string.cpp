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
 * \file string.cpp
 * \version $Id$
 */

#include "string.h"


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

/** Wraps the given string in "<p>", converts "\n" to "<br/>" and converts
 * "\n\n" to a new paragraph. */
QString
p(QString str)
{
  str = "<p>" + str + "</p>";
  str.replace("\n\n", "</p><p>");
  str.replace("\n", "<br/>");
  return str;
}

