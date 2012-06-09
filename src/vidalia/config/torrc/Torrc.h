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
** \file Torrc.h
** \brief Handles the interaction with the torrc file
*/

#ifndef TORRC_H
#define TORRC_H

#include <QtCore>

#include "TorControl.h"
#include "TorrcParser.h"

class Torrc : public QObject
{
  Q_OBJECT
 public:
  /** Constructor */
  Torrc(const QString &torrcPath, const QString &defaultsPath = "");

  /** Loads the torrc from the path provided and its defaults if provided */
  void load(const QString &torrcPath, const QString &defaultsPath = "");

  /** Applies the changes in configuration to the torrc file */
  bool apply(TorControl *tc = 0, QString *errmsg = 0);
  /** Clears the keys provided in the list */
  void clear(QStringList &keys);
  /** Clears the _torrcMap and _lines values */
  void clearAll();

  /** Returns a list of values associated with the given key */
  QStringList value(const QString &key) const;
  /** Sets the value to the given key with a optional
   *  comment that will appear in the file */
  void setValue(const QString &key, const QString &value, const QString &comment = "");

  /** Returns the used torrc path */
  QString getTorrcPath() const
  { return _torrcPath; }

  /** Returns true if the key=value is a valid config option */
  bool isValid(const QString &key, const QString &value);

  /** Resets the config values as if the torrc file contained the
   *  string in contents, and applies it. */
  bool setRawContents(const QString &contents, QString *errmsg, TorControl *tc = 0);

 private:
  /** Map that contains the key=value config options and a
      representation to the actual line in the torrc file */
  QMap<QString, QPair<QString,TorOpt> > _torrcMap;
  /** Map that contains the key=value config options and a
      representation to the actual line in the defaults torrc file */
  QMap<QString, QPair<QString,TorOpt> > _defaultsMap;
  /** List of torrc lines */
  QList<TorrcLine *> _lines;
  /** Parser used to analyze the torrc and defaults files */
  TorrcParser _parser;

  /** Path to the used torrc file */
  QString _torrcPath;

  /** True if the config options have been changed */
  bool _changed;
};

#endif
