/*
**  This file is part of Vidalia, and is subject to the license terms in the
**  LICENSE file, found in the top level directory of this distribution. If you
**  did not receive the LICENSE file with this file, you may obtain it from the
**  Vidalia source package distributed by the Vidalia Project at
**  http://www.vidalia-project.net/. No part of Vidalia, including this file,
**  may be copied, modified, propagated, or distributed except according to the
**  terms described in the LICENSE file.
*/

/*
** \file TrayIconImpl_mac.h
** \version $Id$
** \brief Tray icon implementation on OS X (Dock icon)
*/

#ifndef _TRAYICON_MAC_H
#define _TRAYICON_MAC_H

#include <QWidget>
#include <QString>

#include <Carbon/Carbon.h>


class TrayIconImpl : public QWidget
{
  Q_OBJECT

protected:
  /** Default Constructor */
  TrayIconImpl(QWidget *parent = 0);
  /** Destructor */
  ~TrayIconImpl();

  /** Show the tray icon. */
  void show();
  /** Hide the tray icon. */
  void hide();
  /** Update the tray icon's image. */
  void setIcon(const QString &iconFile);
  /** Update the tray icon's tooltip. */
  void setToolTip(const QString &toolTip);

private:
  /** Load icon data from the given file and create a CGImageRef. */
  CGImageRef createIconFromFile(FSSpec fileSpec);
  /** Create an icon from the given filename in the application bundle. */
  CGImageRef createIcon(const QString &iconFile);
  /** Callback used by CGDataProviderCreateWithData(). */
  static void releaseCallback(void *info, const void *data, size_t size);
  
  CGImageRef _imageRef; /**< Tray icon image. */
  bool _shown; /**< True if the dock icon is to be displayed. */
};

#endif

