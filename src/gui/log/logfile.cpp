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
 * \file logfile.cpp
 * \version $Id$
 * \brief Logs messages from Tor to a file
 */

#include <QDir>
#include <util/string.h>

#include "logfile.h"


/** Default constructor. */
LogFile::LogFile()
{
  _file = 0;
}

/** Destructor. */
LogFile::~LogFile()
{
  if (_file) {
    delete _file;
  }
}

/** Creates a path to the given log file. */
bool
LogFile::createPathToFile(QString filename)
{
  QString path = filename.left(filename.lastIndexOf(QDir::separator()));
  QDir dir(path);
  if (!dir.exists()) {
    return dir.mkpath(path);  
  }
  return true;
}

/** Opens a log file for writing. */
bool
LogFile::open(QString filename, QString *errmsg)
{
  QFile *newLogFile;
 
  /* If the file is already open, then no need to open it again */
  if (_file && _file->isOpen()) {
    if (_file->fileName() == filename) {
      return true;
    }
  }

  /* Create the path to the log file, if necessary */
  if (!createPathToFile(filename)) {
    return err(errmsg, "Unable to create path to log file.");
  }
 
  /* Try to open the new log file */
  newLogFile = new QFile(filename);
  if (!newLogFile->open(QFile::WriteOnly|QIODevice::Append|QIODevice::Text)) {
    delete newLogFile;
    return err(errmsg, newLogFile->errorString());
  }
 
  /* Rotate the new log file in place of the old one */
  if (_file) {
    delete _file;
  }
  _file = newLogFile;
  _stream.setDevice(_file);
  return true;
}

/** Closes an open log file. */
void
LogFile::close()
{
  if (_file) {
    delete _file;
    _file = 0;
  }
}

/** Returns true if the logfile is currently open. */
bool
LogFile::isOpen()
{
  return (_file && _file->isOpen());
}

/** Returns the filename of the current log file. */
QString
LogFile::filename()
{
  return (_file ? _file->fileName() : QString());;
}

/** Overloaded ostream operator. */
LogFile&
LogFile::operator<<(const QString &s)
{
  if (_file) {
    _stream << s;
    _stream.flush();
  }
  return *this;
}

