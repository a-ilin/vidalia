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
** \file DefaultSettings.h
** \brief
*/

#ifndef _DEFAULTSETTINGS_H
#define _DEFAULTSETTINGS_H

#include <QSettings>

class DefaultSettings
{
 public:
  /** Default constructor. */
  DefaultSettings();
  /** Default desrtuctor */
  ~DefaultSettings();

  /** Return the stored default value */
  QVariant getDefault(const QString &key,
                      const QVariant &defaultVal = QVariant()) const;

 private:
  QSettings *_settings;
};

#endif
