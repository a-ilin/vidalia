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
** \file TorrcPrototype.h
** \brief Object for interacting with Torrc file
*/

#ifndef _TORRCPROTO_H
#define _TORRCPROTO_H

#include <QtGlobal>

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include <QtScript>

#include "Torrc.h"

class TorrcPrototype : public QObject, QScriptable
{
  Q_OBJECT

public:
  TorrcPrototype();

  static int metaTypeId();
  static QString name();

  /** Loads the torrc from the path provided and its defaults if provided */
  Q_INVOKABLE void load(const QString &torrcPath, const QString &defaultsPath);
  /** Applies the changes in configuration to the torrc file */
  Q_INVOKABLE bool apply(TorControl *tc = 0, QString *errmsg = 0);

  /** Clears the keys provided in the list */
  Q_INVOKABLE void clear(QStringList keys);
  /** Clears the _torrcMap and _lines values */
  Q_INVOKABLE void clearAll();

  /** Returns a list of values associated with the given key */
  Q_INVOKABLE QStringList value(const QString &key) const;
  /** Sets the value to the given key with a optional
   *  comment that will appear in the file */
  Q_INVOKABLE void setValue(const QString &key, const QString &value, const QString &comment = "");

  /** Returns the used torrc path */
  Q_INVOKABLE QString getTorrcPath() const;

  /** Returns true if the key=value is a valid config option */
  Q_INVOKABLE bool isValid(const QString &key, const QString &value);

  /** Resets the config values as if the torrc file contained the
   *  string in contents, and applies it. */
  Q_INVOKABLE bool setRawContents(const QString &contents, QString *errmsg, TorControl *tc = 0);
};

Q_DECLARE_METATYPE(Torrc *);

#endif
