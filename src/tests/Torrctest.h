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
** \file TorrcTest.h
** \brief test of torrc file handling
*/

#ifndef _TORRC_TEST_H
#define _TORRC_TEST_H

#include <QtTest/QTest>

/** Full torrc test suite. */
class TorrcTestSuite : public QObject
{
  Q_OBJECT

public:
  /** Constructor. */
  TorrcTestSuite(QObject * parent = NULL);
  /** Destructor. */
  ~TorrcTestSuite();

private slots:
  /** Test TorrcLine class */
  void testTorrcLine();
  /** Test parser class */
  void testTorrcParser();
  /** Test main Torrc class */
  void testTorrc();
  /** Test with running Tor */
  void testRunningTor();

};

#endif
