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

#include "StatusEventWidget.h"
#include "StatusEventItem.h"
#include "StatusEventItemDelegate.h"
#include "Vidalia.h"

#include "TorEvents.h"

#include <QTime>
#include <QPainter>
#include <QPixmap>
#include <QStringList>
#include <QObject>
#include <QHeaderView>

StatusEventWidget::StatusEventWidget(QWidget *parent)
  : QTreeWidget(parent)
{
  TorControl *tc = Vidalia::torControl();
  tc->setEvent(TorEvents::GeneralStatus);
  tc->setEvent(TorEvents::ClientStatus);
  tc->setEvent(TorEvents::ServerStatus);

  connect(tc, SIGNAL(authenticated()), this, SLOT(authenticated()));
  connect(tc, SIGNAL(disconnected()), this, SLOT(disconnected()));
  connect(tc, SIGNAL(dangerousTorVersion(tc::TorVersionStatus, QString,
                                         QStringList)),
         this, SLOT(dangerousTorVersion(tc::TorVersionStatus, QString,
                                        QStringList)));
  connect(tc, SIGNAL(circuitEstablished()), this, SLOT(circuitEstablished()));
  connect(tc, SIGNAL(bug(QString)), this, SLOT(bug(QString)));
  connect(tc, SIGNAL(clockSkewed(int, QString)),
          this, SLOT(clockSkewed(int, QString)));
  connect(tc, SIGNAL(dangerousPort(quint16, bool)),
          this, SLOT(dangerousPort(quint16, bool)));
  connect(tc, SIGNAL(socksError(tc::SocksError, QString)),
          this, SLOT(socksError(tc::SocksError, QString)));

  setItemDelegate(new StatusEventItemDelegate(this));
}

void
StatusEventWidget::retranslateUi()
{
  /* XXX: We need to store the untranslated text for each of the status
   *      event items, iterate through all items in the list, and then
   *      retranslate them. The trick is that some of the messages are
   *      generated dynamically based on data sent by Tor (which is NOT
   *      translated). Those messages we can't retranslate correctly
   *      without also storing the variables used to generate the message.
   */
}

void
StatusEventWidget::setMaximumItemCount(int maximumItemCount)
{
  _maximumItemCount = maximumItemCount;

  QTreeWidgetItem *item;
  Qt::SortOrder order = header()->sortIndicatorOrder();
  while (topLevelItemCount() > _maximumItemCount) {
    if (order == Qt::AscendingOrder)
      item = takeTopLevelItem(0);
    else
      item = takeTopLevelItem(topLevelItemCount()-1);
    if (item)
      delete item;
  }
}

int
StatusEventWidget::maximumItemCount() const
{
  return _maximumItemCount;
}

void
StatusEventWidget::addNotification(const QPixmap &icon,
                                   const QString &title,
                                   const QString &description,
                                   const QString &helpUrl)
{
  // Check if we first need to remove the oldest item in the list in order
  // to avoid exceeding the maximum number of notification items
  if (topLevelItemCount() == maximumItemCount()) {
    QTreeWidgetItem *item;
    if (header()->sortIndicatorOrder() == Qt::AscendingOrder)
      item = takeTopLevelItem(0);
    else
      item = takeTopLevelItem(topLevelItemCount()-1);
    if (item)
      delete item;
  }

  // Create the new notification item
  StatusEventItem *item = new StatusEventItem(this);
  item->setTimestamp(QTime::currentTime());
  item->setIcon(icon);
  item->setTitle(title);
  item->setDescription(description);
  item->setHelpUrl(helpUrl);

  // Add the new item to the list and ensure it is visible
  addTopLevelItem(item);
  scrollToItem(item, QAbstractItemView::EnsureVisible);
}

QPixmap
StatusEventWidget::addBadgeToPixmap(const QPixmap &pixmap,
                                    const QPixmap &badge)
{
  QPixmap out = pixmap;
  QPainter painter(&out);
  painter.drawPixmap(pixmap.width() - badge.width(),
                     pixmap.height() - badge.height(),
                     badge);
  return out;
}

void
StatusEventWidget::authenticated()
{
  QString version = Vidalia::torControl()->getTorVersionString();
  QPixmap icon = addBadgeToPixmap(QPixmap(":/images/48x48/tor-logo.png"),
                                  QPixmap(":/images/32x32/dialog-ok-apply.png"));
  addNotification(icon,
    tr("The Tor Software is Running"),
    tr("You are currently running version \"%1\" of the Tor software.")
                                                            .arg(version));
}

void
StatusEventWidget::disconnected()
{
  QPixmap icon = addBadgeToPixmap(QPixmap(":/images/48x48/tor-logo.png"),
                                  QPixmap(":/images/32x32/edit-delete.png"));

  addNotification(icon,
    tr("The Tor Software is not Running"),
    tr("Click \"Start Tor\" in the Vidalia Control Panel to restart the Tor "
       "software. If Tor exited unexpectedly, select the \"Advanced\" tab "
       "above for details about any errors encountered."));
}

void
StatusEventWidget::dangerousTorVersion(tc::TorVersionStatus reason,
                                     const QString &version,
                                     const QStringList &recommended)
{
  Q_UNUSED(recommended);
  QString description;
  QPixmap icon;

  if (reason == tc::UnrecommendedTorVersion) {
    icon = addBadgeToPixmap(QPixmap(":/images/48x48/tor-logo.png"),
                            QPixmap(":/images/32x32/security-medium.png"));

    description = 
      tr("You are currently running version \"%1\" of the Tor software, which "
         "is no longer recommended. Please upgrade to the most recent version "
         "of the software, which may contain important security, reliability "
         "and performance fixes.").arg(version);
  } else if (reason == tc::ObsoleteTorVersion) {
    icon = addBadgeToPixmap(QPixmap(":/images/48x48/tor-logo.png"),
                            QPixmap(":/images/32x32/security-low.png"));

    description =
      tr("You are currently running version \"%1\" of the Tor software, which "
         "likely no longer works with the current Tor network. Please upgrade "
         "to the most recent version of the software, which may contain "
         "important security, reliability and performance fixes.").arg(version);
  }

  addNotification(icon, tr("Your Tor Software is Out-of-date"), description);
}

void
StatusEventWidget::circuitEstablished()
{
  addNotification(QPixmap(":/images/48x48/network-connect.png"),
    tr("Connected to the Tor Network"),
    tr("We were able to successfully establish a connection to the Tor "
       "network. You can now configure your applications to use the Internet "
       "anonymously."));
}

void
StatusEventWidget::bug(const QString &description)
{
  QString bugsUrl = "https://bugs.torproject.org/";
  QPixmap icon = addBadgeToPixmap(QPixmap(":/images/48x48/tor-logo.png"),
                                  QPixmap(":/images/32x32/script-error.png"));
  addNotification(icon,
    tr("Tor Software Error"),
    tr("The Tor software encountered an internal bug. Please report the "
       "following error message to the Tor developers at bugs.torproject.org: "
       "\"%1\"").arg(description));
}

void
StatusEventWidget::clockSkewed(int skew, const QString &source)
{
  if (source.startsWith("OR:", Qt::CaseInsensitive)) {
    // Tor versions 0.2.1.19 and earlier, and 0.2.2.1 and earlier, throw
    // this message a little too liberally in this case.
    quint32 torVersion = Vidalia::torControl()->getTorVersion();
    if (torVersion <= 0x00020113)
      return;
    QString str = Vidalia::torControl()->getTorVersionString();
    if (str.startsWith("0.2.2.") && torVersion <= 0x00020201)
      return;
  }

  QString description;
  QPixmap icon = addBadgeToPixmap(QPixmap(":/images/48x48/chronometer.png"),
                                  QPixmap(":/images/48x48/dialog-warning.png"));

  if (skew < 0) {
    description = 
      tr("Tor has determined that your computer's clock may be set to %1 "
         "seconds in the past compared to the source \"%2\". If your "
         "clock is not correct, Tor will not be able to function. Please "
         "verify your computer displays the correct time.").arg(qAbs(skew))
                                                           .arg(source);
  } else {
    description = 
      tr("Tor has determined th at your computer's clock may be set to %1 "
         "seconds in the future compared to the source \"%2\". If "
         "your clock is not correct, Tor will not be able to function. Please "
         "verify your computer displays the correct time.").arg(qAbs(skew))
                                                           .arg(source);
  }
  addNotification(icon, tr("Your Computer's Clock is Potentially Incorrect"),
                  description);
}

void
StatusEventWidget::dangerousPort(quint16 port, bool rejected)
{
  QPixmap icon;
  QString description;

  if (rejected) {
    icon = addBadgeToPixmap(QPixmap(":/images/48x48/applications-internet.png"),
                            QPixmap(":/images/32x32/security-low.png"));

    description =
      tr("One of the applications on your computer may have attempted to "
         "make an unencrypted connection through Tor to port %1. Sending "
         "unencrypted information over the Tor network is dangerous and not "
         "recommended. For your protection, Tor has automatically closed this "
         "connection.");
  } else {
    icon = addBadgeToPixmap(QPixmap(":/images/48x48/applications-internet.png"),
                            QPixmap(":/images/32x32/security-medium.png"));
    description =
      tr("One of the applications on your computer may have attempted to "
         "make an unencrypted connection through Tor to port %1. Sending "
         "unencrypted information over the Tor network is dangerous and not "
         "recommended.");
  }

  addNotification(icon, tr("Potentially Dangerous Connection!"), description);
}

void
StatusEventWidget::socksError(tc::SocksError type, const QString &destination)
{
  QString title, description;
  QPixmap icon = QPixmap(":/images/48x48/applications-internet.png");

  if (type == tc::DangerousSocksTypeError) {
    icon  = addBadgeToPixmap(icon,
                             QPixmap(":/images/32x32/security-medium.png"));
    title = tr("Potentially Dangerous Connection!");

    description =
      tr("One of your applications established a connection through Tor "
         "to \"%1\" using a protocol that may leak information about your "
         "destination. Please ensure you configure your applications to use "
         "only SOCKS4a or SOCKS5 with remote hostname resolution.")
                                                            .arg(destination);
  } else if (type == tc::UnknownSocksProtocolError) {
    icon = addBadgeToPixmap(icon,
                            QPixmap(":/images/32x32/dialog-warning.png"));
    title = tr("Unknown SOCKS Protocol");

    description =
      tr("One of your applications tried to establish a connection through "
         "Tor using a protocol that Tor does not understand. Please ensure "
         "you configure your applications to use only SOCKS4a or SOCKS5 with "
         "remote hostname resolution.");
  } else if (type == tc::BadSocksHostnameError) {
    icon = addBadgeToPixmap(icon,
                            QPixmap(":/images/32x32/dialog-warning.png"));

    title = tr("Invalid Destination Hostname");
    description =
      tr("One of your applications tried to establish a connection through "
         "Tor to \"%1\", which Tor does not recognize as a valid hostname. "
         "Please check your application's configuration.").arg(destination);
  } else {
    return;
  }

  addNotification(icon, title, description);
}

