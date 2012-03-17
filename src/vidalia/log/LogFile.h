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
** \file LogFile.h
** \brief Logs messages from Tor to a file
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

