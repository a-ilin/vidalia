/*
**  This file is part of Vidalia, and is subject to the license terms in the
**  LICENSE file, found in the top level directory of this distribution. If
**  you did not receive the LICENSE file with this file, you may obtain it
**  from the Vidalia source package distributed by the Vidalia Project at
**  http://www.torproject.org/projects/vidalia.html. No part of Vidalia,
**  including this file, may be copied, modified, propagated, or distributed
**  except according to the terms described in the LICENSE file.
*/

/*
** \file UPNPTestDialog.h
** \brief Dialog that displays the progress of a UPnP configuration test
*/

#ifndef _UPNPTESTDIALOG_H
#define _UPNPTESTDIALOG_H

#include "ui_UPNPTestDialog.h"
#include "UPNPControl.h"

#include <QDialog>
#include <QAbstractButton>
#include <QTimer>


class UPNPTestDialog : public QDialog
{
  Q_OBJECT

public:
  /** Default constructor. <b>orPort</b> and <b>dirPort</b> specify the ports
   * used to test UPnP port forwarding. The original UPnP state will be
   * restored when the test dialog is closed. */
  UPNPTestDialog(quint16 orPort, quint16 dirPort, QWidget *parent = 0);

signals:
  /** Emitted when the user clicks the 'Help' button requesting the help topic
   * on port forwarding. */
  void help();

protected slots:
  /** Shows or hides the dialog based on <b>visible</b>. The UPnP test will be
   * started when the dialog is first shown. */
  void setVisible(bool visible);

  /** Called when a user clicks on a button in the dialog's button box. If
   * Retry is clicked, another UPnP test will be conducted. If Close is clicked,
   * then the dialog is closed and the original UPnP state restored. */
  void clicked(QAbstractButton *button);

  /** Updates the test UI based on the UPnP <b>state</b>. */
  void upnpStateChanged(UPNPControl::UPNPState state);

  /** Updates the progress bar to indicate the device discovery portion of the
   * test is still in progress. */
  void discoverTimeout();

protected:
  /** Initiates a UPnP test. */
  void startTest();

  /** Called when the UPnP test successfully enables port forwarding. Enables
   * the Close button, allowing the user to exit the test dialog. */
  void testSuccessful();

  /** Called when the UPnP test fails due to an error. Enables the Close and
   * Retry buttons, allowing the user to either rerun the test or give up. */
  void testFailed();

private:
  /** Pointer to the UPNPControl singleton instance. */
  UPNPControl *_upnp;

  /** Timer used to update the progress bar while during the device discovery
   * portion of the test. */
  QTimer _discoverTimer;

  quint16 _oldOrPort;  /**< Original (pre-test) forwarded ORPort. */
  quint16 _oldDirPort; /**< Original (pre-test) forwarded DirPort. */
  quint16 _orPort;     /**< ORPort used during the test. */
  quint16 _dirPort;    /**< DirPort used during the test. */

  Ui::UPNPTestDialog ui;
};

#endif

