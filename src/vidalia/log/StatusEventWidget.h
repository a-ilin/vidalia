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
** \file StatusEventWidget.h
** \version $Id$
** \brief Displays information on Tor status events
*/

#ifndef _STATUSEVENTWIDGET_H
#define _STATUSEVENTWIDGET_H

#include <QTreeWidget>

#include "TorControl.h"

class QPixmap;
class QString;
class QStringList;

class StatusEventWidget : public QTreeWidget
{
  Q_OBJECT

public:
  /** Default constructor.
   */
  StatusEventWidget(QWidget *parent = 0);

  /** Sets the maximum number of status event items displayed to
   * <b>maximumItemCount</b>. If the current number of items displayed
   * exceeds <b>maximumItemCount</b>, the oldest items will be removed until
   * topLevelItemCount() is equal to the new limit.
   */
  void setMaximumItemCount(int maximumItemCount);

  /** Returns the maximum number of status items that can be displayed in 
   * this StatusEventWidget.
   */
  int maximumItemCount() const;

protected:
  /** Called when the user has changed the UI display language in Vidalia
   * indicating all the displayed text widgets need to be updated to
   * display their value in the newly chosen language.
   */
  virtual void retranslateUi();

private slots:
  /** Called when the control socket is connected and authenticated. */
  void authenticated();

  /** Called when the controller has disconnected from Tor */
  void disconnected();

  /** Indicates that Tor has decided the user's Tor software <b>version</b>
   * is no longer recommended for some <b>reason</b>. <b>recommended</b> is
   * a list of Tor software versions that are considered current.
   */
  void dangerousTorVersion(tc::TorVersionStatus reason,
                           const QString &version,
                           const QStringList &recommended);

  /** Indicates Tor has been able to successfully establish one or more
   * circuits.
   */
  void circuitEstablished();

  /** Emitted when Tor has encountered an internal bug. <b>reason</b> is
   * Tor's description of the bug.
   */
  void bug(const QString &reason);

  /** Indicates that Tor has determined the client's clock is potentially
   * skewed by <b>skew</b> seconds relative to <b>source</b>.
   */
  void clockSkewed(int skew, const QString &source);

  /** Called when the user attempts to establish a connection to some
   * destination on port <b>port</b>, which is a port known to use
   * plaintext connections (as determined by Tor's WarnPlaintextPorts and
   * RejectPlaintextPorts torrc options). <b>rejected</b> indicates whether
   * Tor rejected the connection or permitted it to connect anyway.
   */
  void dangerousPort(quint16 port, bool rejected);

  /** Called when Tor detects a problem with a SOCKS connection from the
   * user, such as a bad hostname, dangerous SOCKS protocol type, or a bad
   * hostname. <b>type</b> indicates the type of error encountered and
   * <b>destination</b> (if non-empty) specifies the attempted connection
   * destination address or hostname.
   */
  void socksError(tc::SocksError type, const QString &destination);

private:
  /** Adds a new status event notification item to the widget. The item will
   * be drawn using the specified <b>icon</b>, short event <b>title</b>,
   * and a longer detailed <b>description</b>. If <b>helpUrl</b> is not
   * empty, the item will also have a clickable question mark icon in the
   * corner.
   */
  void addNotification(const QPixmap &icon,
                       const QString &title,
                       const QString &description,
                       const QString &helpUrl = QString());

  /** Creates a new QPixmap using <b>pixmap</b> as the main image and
   * overlays <b>badge</b> in the lower-right corner of the image.
   */
  static QPixmap addBadgeToPixmap(const QPixmap &pixmap,
                                  const QPixmap &badge);

  /** Maximum number of event notifications contained in the StatusEventWidget
   * at any given time.
   * \sa setMaximumItemCount()
   */
  int _maximumItemCount;
};

#endif

