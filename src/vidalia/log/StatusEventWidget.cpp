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
** \file StatusEventWidget.h
** \brief Displays information on Tor status events
*/

#include "StatusEventWidget.h"
#include "StatusEventItem.h"
#include "StatusEventItemDelegate.h"
#include "Vidalia.h"

#include "TorEvents.h"
#include "stringutil.h"

#include <QTime>
#include <QMenu>
#include <QPainter>
#include <QPixmap>
#include <QStringList>
#include <QObject>
#include <QHeaderView>
#include <QClipboard>

bool compareStatusEventItems(const QTreeWidgetItem *a,
                             const QTreeWidgetItem *b)
{
  return (*a < *b);
}

StatusEventWidget::StatusEventWidget(QWidget *parent)
  : QTreeWidget(parent)
{
  TorControl *tc = Vidalia::torControl();
  tc->setEvent(TorEvents::GeneralStatus);
  tc->setEvent(TorEvents::ClientStatus);
  tc->setEvent(TorEvents::ServerStatus);

  connect(this, SIGNAL(customContextMenuRequested(QPoint)),
          this, SLOT(customContextMenuRequested(QPoint)));
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
  connect(tc, SIGNAL(externalAddressChanged(QHostAddress, QString)),
          this, SLOT(externalAddressChanged(QHostAddress, QString)));
  connect(tc, SIGNAL(dnsHijacked()), this, SLOT(dnsHijacked()));
  connect(tc, SIGNAL(dnsUseless()), this, SLOT(dnsUseless()));
  connect(tc, SIGNAL(checkingOrPortReachability(QHostAddress, quint16)),
          this, SLOT(checkingOrPortReachability(QHostAddress, quint16)));
  connect(tc, SIGNAL(orPortReachabilityFinished(QHostAddress, quint16, bool)),
          this, SLOT(orPortReachabilityFinished(QHostAddress, quint16, bool)));
  connect(tc, SIGNAL(checkingDirPortReachability(QHostAddress, quint16)),
          this, SLOT(checkingDirPortReachability(QHostAddress, quint16)));
  connect(tc, SIGNAL(dirPortReachabilityFinished(QHostAddress, quint16, bool)),
          this, SLOT(dirPortReachabilityFinished(QHostAddress, quint16, bool)));
  connect(tc, SIGNAL(serverDescriptorRejected(QHostAddress, quint16, QString)),
          this, SLOT(serverDescriptorRejected(QHostAddress, quint16, QString)));
  connect(tc, SIGNAL(serverDescriptorAccepted(QHostAddress, quint16)),
          this, SLOT(serverDescriptorAccepted(QHostAddress, quint16)));

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

QStringList
StatusEventWidget::selectedEvents() const
{
  QString text;
  QStringList out;
  QList<QTreeWidgetItem *> items = selectedItems();

  // We have to sort the items since selectedItems() returns the order in
  // which the items were selected, not the order in which they appear in the
  // current list.
  qStableSort(items.begin(), items.end(), compareStatusEventItems);

  for (int i = 0; i < items.size(); i++) {
    StatusEventItem *event = dynamic_cast<StatusEventItem *>(items.at(i));
    if (event)
      out.append(event->toString());
  }
  return out;
}

QStringList
StatusEventWidget::allEvents() const
{
  QStringList out;
  QList<QTreeWidgetItem *> items;

  for (int i = 0; i < topLevelItemCount(); i++)
    items.append(topLevelItem(i));

  // Ensure the items are sorted in ascending order according to timestamp
  qStableSort(items.begin(), items.end(), compareStatusEventItems);

  for (int i = 0; i < items.size(); i++) {
    StatusEventItem *event = dynamic_cast<StatusEventItem *>(items.at(i));
    if (event)
      out.append(event->toString());
  }
  return out;
}

void
StatusEventWidget::customContextMenuRequested(const QPoint &pos)
{
  QMenu menu(this);

  StatusEventItem *item = dynamic_cast<StatusEventItem *>(itemAt(pos));
  if (! item || ! item->isSelected())
    return;

  QAction *copyAction = menu.addAction(QIcon(":/images/22x22/edit-copy.png"),
                                       tr("Copy to Clipboard"));

  QAction *action = menu.exec(mapToGlobal(pos));
  if (action == copyAction) {
    QStringList eventText = selectedEvents();
    if (! eventText.isEmpty())
      QApplication::clipboard()->setText(eventText.join("\n"));
  }
}

QList<StatusEventItem *>
StatusEventWidget::find(const QString &text, bool highlight)
{
  QList<StatusEventItem *> items;

  for (int i = 0; i < topLevelItemCount(); i++) {
    StatusEventItem *item = dynamic_cast<StatusEventItem *>(topLevelItem(i));
    if (! item)
      continue;

    if (item->title().contains(text, Qt::CaseInsensitive)
        || item->description().contains(text, Qt::CaseInsensitive)) {
      items.append(item);
      if (highlight)
        item->setSelected(true);
    } else if (highlight) {
      item->setSelected(false);
    }
  }
  return items;
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
  item->setTimestamp(QDateTime::currentDateTime());
  item->setIcon(icon);
  item->setTitle(title);
  item->setDescription(description);
  item->setHelpUrl(helpUrl);
  item->setToolTip(string_wrap(description, 80));

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

QPixmap
StatusEventWidget::addBadgeToPixmap(const QPixmap &pixmap,
                                    const QString &badge)
{
  return StatusEventWidget::addBadgeToPixmap(pixmap, QPixmap(badge));
}

QPixmap
StatusEventWidget::addBadgeToPixmap(const QString &pixmap,
                                    const QString &badge)
{
  return StatusEventWidget::addBadgeToPixmap(QPixmap(pixmap), QPixmap(badge));
}

void
StatusEventWidget::authenticated()
{
  TorControl *tc = Vidalia::torControl();

  QString version = tc->getTorVersionString();
  QPixmap icon = addBadgeToPixmap(":/images/48x48/tor-logo.png",
                                  ":/images/32x32/dialog-ok-apply.png");
  addNotification(icon,
    tr("The Tor Software is Running"),
    tr("You are currently running version \"%1\" of the Tor software.")
                                                            .arg(version));

  // Check if Tor established a circuit before we were able to authenticate,
  // in which case we missed the CIRCUIT_ESTABLISHED event. So fake it.
  if (tc->isCircuitEstablished())
    circuitEstablished();

  // Check on the status of Tor's version, in case we missed that event too
  QString status = tc->getInfo("status/version/current").toString();
  if (! status.compare("old", Qt::CaseInsensitive)
        || ! status.compare("obsolete", Qt::CaseInsensitive)) {
    dangerousTorVersion(tc::ObsoleteTorVersion, version, QStringList());
  } else if (! status.compare("unrecommended", Qt::CaseInsensitive)) {
    dangerousTorVersion(tc::UnrecommendedTorVersion, version, QStringList());
  }
}

void
StatusEventWidget::disconnected()
{
  QPixmap icon = addBadgeToPixmap(":/images/48x48/tor-logo.png",
                                  ":/images/32x32/edit-delete.png");

  addNotification(icon,
    tr("The Tor Software is not Running"),
    tr("Click \"Start Tor\" in the Vidalia Control Panel to restart the Tor "
       "software. If Tor exited unexpectedly, select the \"Advanced\" tab "
       "above for details about any errors encountered."));

  _squelchDescriptorAcceptedEvent = false;
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
    icon = addBadgeToPixmap(":/images/48x48/tor-logo.png",
                            ":/images/32x32/security-medium.png");

    description =
      tr("You are currently running version \"%1\" of the Tor software, which "
         "is no longer recommended. Please upgrade to the most recent version "
         "of the software, which may contain important security, reliability "
         "and performance fixes.").arg(version);
  } else if (reason == tc::ObsoleteTorVersion) {
    icon = addBadgeToPixmap(":/images/48x48/tor-logo.png",
                            ":/images/32x32/security-low.png");

    description =
      tr("You are currently running version \"%1\" of the Tor software, which "
         "may no longer work with the current Tor network. Please upgrade "
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
  QPixmap icon = addBadgeToPixmap(":/images/48x48/tor-logo.png",
                                  ":/images/32x32/script-error.png");
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
  QPixmap icon = addBadgeToPixmap(":/images/48x48/chronometer.png",
                                  ":/images/32x32/dialog-warning.png");

  if (skew < 0) {
    description =
      tr("Tor has determined that your computer's clock may be set to %1 "
         "seconds in the past compared to the source \"%2\". If your "
         "clock is not correct, Tor will not be able to function. Please "
         "verify your computer displays the correct time.").arg(qAbs(skew))
                                                           .arg(source);
  } else {
    description =
      tr("Tor has determined that your computer's clock may be set to %1 "
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
    icon = addBadgeToPixmap(":/images/48x48/applications-internet.png",
                            ":/images/32x32/security-low.png");

    description =
      tr("One of the applications on your computer may have attempted to "
         "make an unencrypted connection through Tor to port %1. Sending "
         "unencrypted information over the Tor network is dangerous and not "
         "recommended. For your protection, Tor has automatically closed this "
         "connection.").arg(port);
  } else {
    icon = addBadgeToPixmap(":/images/48x48/applications-internet.png",
                            ":/images/32x32/security-medium.png");
    description =
      tr("One of the applications on your computer may have attempted to "
         "make an unencrypted connection through Tor to port %1. Sending "
         "unencrypted information over the Tor network is dangerous and not "
         "recommended.").arg(port);
  }

  addNotification(icon, tr("Potentially Dangerous Connection!"), description);
}

void
StatusEventWidget::socksError(tc::SocksError type, const QString &destination)
{
  QString title, description;
  QPixmap icon = QPixmap(":/images/48x48/applications-internet.png");

  if (type == tc::DangerousSocksTypeError) {
    icon  = addBadgeToPixmap(icon, ":/images/32x32/security-medium.png");

    title = tr("Potentially Dangerous Connection!");
    description =
      tr("One of your applications established a connection through Tor "
         "to \"%1\" using a protocol that may leak information about your "
         "destination. Please ensure you configure your applications to use "
         "only SOCKS4a or SOCKS5 with remote hostname resolution.")
                                                            .arg(destination);
  } else if (type == tc::UnknownSocksProtocolError) {
    icon = addBadgeToPixmap(icon, ":/images/32x32/dialog-warning.png");

    title = tr("Unknown SOCKS Protocol");
    description =
      tr("One of your applications tried to establish a connection through "
         "Tor using a protocol that Tor does not understand. Please ensure "
         "you configure your applications to use only SOCKS4a or SOCKS5 with "
         "remote hostname resolution.");
  } else if (type == tc::BadSocksHostnameError) {
    icon = addBadgeToPixmap(icon, ":/images/32x32/dialog-warning.png");

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

void
StatusEventWidget::externalAddressChanged(const QHostAddress &ip,
                                          const QString &hostname)
{
  QString hostString = hostname.isEmpty() ? QString()
                                          : QString(" (%1)").arg(hostname);

  addNotification(QPixmap(":/images/48x48/applications-internet.png"),
    tr("External IP Address Changed"),
    tr("Tor has determined your relay's public IP address is currently %1%2. "
       "If that is not correct, please consider setting the 'Address' option "
       "in your relay's configuration.").arg(ip.toString()).arg(hostString));
}

void
StatusEventWidget::dnsHijacked()
{
  QPixmap icon = addBadgeToPixmap(":/images/48x48/applications-internet.png",
                                  ":/images/32x32/dialog-warning.png");
  addNotification(icon,
    tr("DNS Hijacking Detected"),
    tr("Tor detected that your DNS provider is providing false responses for "
       "domains that do not exist. Some ISPs and other DNS providers, such as "
       "OpenDNS, are known to do this in order to display their own search or "
       "advertising pages."));
}

void
StatusEventWidget::dnsUseless()
{
  QPixmap icon = addBadgeToPixmap(":/images/48x48/applications-internet.png",
                                  ":/images/32x32/edit-delete.png");
  addNotification(icon,
    tr("DNS Hijacking Detected"),
    tr("Tor detected that your DNS provider is providing false responses for "
       "well known domains. Since clients rely on Tor network relays to "
       "provide accurate DNS repsonses, your relay will not be configured as "
       "an exit relay."));
}

void
StatusEventWidget::checkingOrPortReachability(const QHostAddress &ip,
                                              quint16 port)
{
  addNotification(QPixmap(":/images/48x48/network-wired.png"),
    tr("Checking Server Port Reachability"),
    tr("Tor is trying to determine if your relay's server port is reachable "
       "from the Tor network by connecting to itself at %1:%2. This test "
       "could take several minutes.").arg(ip.toString()).arg(port));
}

void
StatusEventWidget::orPortReachabilityFinished(const QHostAddress &ip,
                                              quint16 port,
                                              bool reachable)
{
  QString title, description;
  QPixmap icon = QPixmap(":/images/48x48/network-wired.png");
  if (reachable) {
    icon = addBadgeToPixmap(icon, ":/images/32x32/dialog-ok-apply.png");
    title = tr("Server Port Reachability Test Successful!");
    description =
      tr("Your relay's server port is reachable from the Tor network!");
  } else {
    icon = addBadgeToPixmap(icon, ":/images/32x32/dialog-warning.png");
    title = tr("Server Port Reachability Test Failed");
    description =
      tr("Your relay's server port is not reachable by other Tor clients. This "
         "can happen if you are behind a router or firewall that requires you "
         "to set up port forwarding. If %1:%2 is not your correct IP address "
         "and server port, please check your relay's configuration.")
                                                .arg(ip.toString()).arg(port);
  }

  addNotification(icon, title, description);
}

void
StatusEventWidget::checkingDirPortReachability(const QHostAddress &ip,
                                               quint16 port)
{
  addNotification(QPixmap(":/images/48x48/network-wired.png"),
    tr("Checking Directory Port Reachability"),
    tr("Tor is trying to determine if your relay's directory port is reachable "
       "from the Tor network by connecting to itself at %1:%2. This test "
       "could take several minutes.").arg(ip.toString()).arg(port));
}

void
StatusEventWidget::dirPortReachabilityFinished(const QHostAddress &ip,
                                               quint16 port,
                                               bool reachable)
{
  QString title, description;
  QPixmap icon = QPixmap(":/images/48x48/network-wired.png");
  if (reachable) {
    icon = addBadgeToPixmap(icon, ":/images/32x32/dialog-ok-apply.png");
    title = tr("Directory Port Reachability Test Successful!");
    description =
      tr("Your relay's directory port is reachable from the Tor network!");
  } else {
    icon = addBadgeToPixmap(icon, ":/images/32x32/dialog-warning.png");
    title = tr("Directory Port Reachability Test Failed");
    description =
      tr("Your relay's directory port is not reachable by other Tor clients. "
         "This can happen if you are behind a router or firewall that requires "
         "you to set up port forwarding. If %1:%2 is not your correct IP "
         "address and directory port, please check your relay's configuration.")
                                                .arg(ip.toString()).arg(port);
  }

  addNotification(icon, title, description);
}

void
StatusEventWidget::serverDescriptorRejected(const QHostAddress &ip,
                                            quint16 port,
                                            const QString &reason)
{
  QPixmap icon =
    addBadgeToPixmap(":/images/48x48/preferences-system-network-sharing.png",
                     ":/images/32x32/dialog-warning.png");

  addNotification(icon,
    tr("Relay Descriptor Rejected"),
    tr("Your relay's descriptor, which enables clients to connect to your "
       "relay, was rejected by the directory server at %1:%2. The reason "
       "given was: %3").arg(ip.toString()).arg(port).arg(reason));
}

void
StatusEventWidget::serverDescriptorAccepted(const QHostAddress &ip,
                                            quint16 port)
{
  Q_UNUSED(ip);
  Q_UNUSED(port);

  if (_squelchDescriptorAcceptedEvent)
    return;
   _squelchDescriptorAcceptedEvent = true;

  QPixmap icon =
    addBadgeToPixmap(":/images/48x48/preferences-system-network-sharing.png",
                     ":/images/32x32/dialog-ok-apply.png");

  addNotification(icon,
    tr("Your Relay is Online"),
    tr("Your relay is now online and available for Tor clients to use. You "
       "should see an increase in network traffic shown by the Bandwidth "
       "Graph within a few hours as more clients learn about your relay. "
       "Thank you for contributing to the Tor network!"));
}

