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
 * \file ipvalidator.cpp
 * \version $Id$
 */

#include "ipvalidator.h"

/** Regular expression to validate that input is a valid IP address. */
#define IP_REGEXP "\\b(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)"\
                  "\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)"\
                  "\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)"\
                  "\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b"

#define MATCH_ALL "*" /**< Match all IP addresses. */


/** Constructor. */
IPValidator::IPValidator(QObject *parent)
: QRegExpValidator(QRegExp(IP_REGEXP), parent)
{
}

/** Validates the given input is either a valid IP or a "*". */
QValidator::State
IPValidator::validate(QString &input, int &pos) const
{
  if (input == MATCH_ALL) {
    return QValidator::Acceptable;
  }
  return QRegExpValidator::validate(input, pos);
}

/** Validates the given input from position 0. */
QValidator::State
IPValidator::validate(QString &input) const
{
  int discard = 0;
  return validate(input, discard);
}

