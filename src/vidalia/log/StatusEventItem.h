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
** \file StatusEventItem.cpp
** \brief Represents a single status event item in a StatusEventWidget
*/

#ifndef _STATUSEVENTITEM_H
#define _STATUSEVENTITEM_H

#include <QTreeWidgetItem>

class QDateTime;
class QPixmap;
class QString;

class StatusEventItem : public QTreeWidgetItem
{
public:
  /** QModelIndex data roles used to store information with this status
   * event item, allowing it to be retrieved by StatusEventItemDelegate for
   * painting.
   */
  enum DataRole {
    IconRole = Qt::UserRole,
    TimestampRole,
    TitleRole,
    DescriptionRole,
    HelpUrlRole,
  };

  /** Default constructor.
   */
  StatusEventItem(QTreeWidget *parent = 0);

  /** Sets the <b>timestamp</b> at which this status event occurred.
   * \sa timestamp()
   */
  void setTimestamp(const QDateTime &timestamp);

  /** Returns the timestamp at which this status event occurred.
   * \sa setTimestamp()
   */
  QDateTime timestamp() const;

  /** Sets the icon to be drawn along with this status event to <b>pixmap</b>.
   * \sa icon()
   */
  void setIcon(const QPixmap &pixmap);

  /** Returns the icon draw along with this status event.
   * \sa setIcon()
   */
  QPixmap icon() const;

  /** Sets the <b>title</b> text for this status event. The title is a short
   * (fewer than 10 words or so) summary of the event.
   * \sa title()
   */
  void setTitle(const QString &title);

  /** Returns the title text for this status event.
   * \sa setTitle()
   */
  QString title() const;

  /** Sets the detailed <b>description</b> text for this status event. The
   * text should explain what the event means to the user, and any corrective
   * action they might need to take.
   * \sa description()
   */
  void setDescription(const QString &description);

  /** Returns the detailed description text for this status event.
   * \sa setDescription()
   */
  QString description() const;

  /** Sets the help topic URL that contains more information about this
   * particular status event.
   * \sa helpUrl
   */
  void setHelpUrl(const QString &url);

  /** Returns the help topic URL associated with this event item.
   * \sa setHelpUrl
   */
  QString helpUrl() const;

  /** Sets <b>toolTip</b> as the text displayed when the user hovers the mouse
   * over a StatusEventItem. It is more useful for particularly long item
   * descriptions.
   */
  void setToolTip(const QString &toolTip);

  /** Returns a formatted QString containing this item's timestamp, title
   * and description text.
   * \sa timestamp()
   * \sa title()
   * \sa description()
   */
  QString toString() const;

  /** Overloaded comparison operator that allows sorting StatusEventItem
   * objects based on timestamp. Returns true if <i>this</i> StatusEventItem
   * occurred before <b>other</b>.
   */
  virtual bool operator<(const QTreeWidgetItem &other) const;
};

#endif

