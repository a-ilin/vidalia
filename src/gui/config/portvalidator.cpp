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
 * \file portvalidator.cpp
 * \version $Id$
 */

#include "portvalidator.h"

#define MIN_PORT  1     /**< Minimum valid port. */
#define MAX_PORT  65535 /**< Maximum valid port. */
#define MATCH_ALL "*"   /**< Matches all ports. */


/** Constructor. */
PortValidator::PortValidator(QObject *parent)
: QIntValidator(MIN_PORT, MAX_PORT, parent)
{
}

/** Validates that the given port is either a valid port or a "*". */
QValidator::State
PortValidator::validate(QString &input, int &pos) const
{
  if (input == MATCH_ALL) {
    return QValidator::Acceptable;
  }
  return QIntValidator::validate(input, pos);
}

