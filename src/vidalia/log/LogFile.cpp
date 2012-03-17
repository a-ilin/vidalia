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
** \file LogFile.cpp
** \brief Logs messages from Tor to a file
*/

#include "LogFile.h"

#include "stringutil.h"

#include <QDir>


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
  QDir dir = QFileInfo(filename).absoluteDir();
  if (!dir.exists()) {
    return dir.mkpath(dir.absolutePath());
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

