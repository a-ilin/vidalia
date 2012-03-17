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
** \file RouterInfoDialog.h
** \brief Displays detailed information about a particular router
*/


#ifndef _ROUTERINFODIALOG_H
#define _ROUTERINFODIALOG_H

#include "ui_RouterInfoDialog.h"

#include "RouterStatus.h"
#include "RouterDescriptor.h"

#include <QDialog>


class RouterInfoDialog : public QDialog
{
  Q_OBJECT

public:
  /** Default constructor.
   */
  RouterInfoDialog(QWidget *parent = 0);

  /** Populates the dialog's UI with information parsed from the router
   * descriptor <b>desc</b> and the router status information in
   * <b>status</b>.
   */
  void setRouterInfo(const QStringList &desc, const RouterStatus &status);

  /** Sets the geographic location information displayed in the dialog to
   * <b>location</b>.
   */
  void setLocation(const QString &location);

private:
  /** Adjusts <b>uptime</b> to be the greater of either <b>published</b> or
   * <b>uptime</b> plus the number of seconds elapsed since <b>published</b>.
   */
  quint64 adjustUptime(quint64 uptime, const QDateTime &published);

  Ui::RouterInfoDialog ui;
};

#endif

