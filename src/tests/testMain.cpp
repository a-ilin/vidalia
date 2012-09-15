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
** \file testMain.cpp
** \brief Main Vidalia entry point for test target
*/

#include "Vidalia.h"
#include "stringutil.h"
#include "TorrcTest.h"

#include <QtTest/QTest>

/** Helper method to run test cases. */
template<typename T>
void runTest(){
  T * t = new T();
  const int ret = QTest::qExec(t,QApplication::arguments());
  if( ret != 0 ){
    vError(QString("%1: test failed!").arg(t->metaObject()->className()));
  }
  delete t;
}

/** Main application entry point. */
int
main(int argc, char *argv[])
{
  const QStringList args = char_array_to_stringlist(argv+1, argc-1);
  Vidalia vidalia(args, argc, argv);
  /* Launch each test class */
  runTest<TorrcTestSuite>();
  return 0;
}

