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
 * \file logfile.h
 * \version $Id$ 
 * \brief Logs messages from Tor to a file
 */

#ifndef _LOGFILE_H
#define _LOGFILE_H

#include <QFile>
#include <QObject>
#include <QString>
#include <QTextStream>


class LogFile : QObject
{
  Q_OBJECT

public:
  /** Default constructor. */
  LogFile();
  /** Destructor. */
  ~LogFile();

  /** Opens a log file for writing. */
  bool open(QString filename, QString *errmsg = 0);
  /** Closes an open log file. */
  void close();
  
  /** Returns true if the logfile is currently open. */
  bool isOpen();
  /** Returns the filename of the current log file. */
  QString filename();

  /** Overloaded ostream operator. */
  LogFile& operator<<(const QString &s);
  
private:
  /** Creates a path to the given log file */
  bool createPathToFile(QString filename);

  QFile* _file;        /**< The log file. */
  QTextStream _stream; /**< Stream used to write to the log file. */
};

#endif

