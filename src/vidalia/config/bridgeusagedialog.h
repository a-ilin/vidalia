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
** \file bridgeusagedialog.h
** \version $Id$
** \brief Displays a summary of bridge usage information, including client
** geographic location history.
*/

#ifndef _BRIDGEUSAGEDIALOG_H
#define _BRIDGEUSAGEDIALOG_H

#include <QDialog>
#include <QDateTime>
#include <QHash>

#include "ui_bridgeusagedialog.h"


class BridgeUsageDialog : public QDialog
{
  Q_OBJECT

public:
  /** Default constructor.
   */
  BridgeUsageDialog(QWidget *parent = 0);

  /** Updates the dialog with current bridge usage information.
   */
  void update(const QDateTime &timeStarted,
              const QHash<QString,int> &countrySummary);

protected:
  /** Called when the dialog is displayed. Adjusts the size of the column
   * headers.
   */
  void showEvent(QShowEvent *e);

private:
  Ui::BridgeUsageDialog ui;
};

#endif

