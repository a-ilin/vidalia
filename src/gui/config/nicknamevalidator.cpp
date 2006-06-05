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
 * \file nicknamevalidator.cpp
 * \version $Id$
 */

#include "nicknamevalidator.h"

/** Set of characters that are valid in a server's nickname. */
#define VALID_NICKNAME_CHARS \
  "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"
 

/** Constructor. */
NicknameValidator::NicknameValidator(QObject *parent)
: QValidator(parent)
{
}

/** Validates the given input contains only valid nickname characters starting
 * at the specified position. */
QValidator::State
NicknameValidator::validate(QString &input, int &pos) const
{
  int length = input.length();
  
  /* Make sure the input only contains valid characters. */
  QString validChars = VALID_NICKNAME_CHARS;  
  for (int i = 0; i < pos && i < length; i++) {
    if (validChars.indexOf(input.at(i)) < 0) {
      return QValidator::Invalid;
    }
  }
  return QValidator::Acceptable;
}

