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
** \file ControlMethod.cpp
** \brief Class for specifying which method is used to communicate to Tor
*/

#include "ControlMethod.h"

QString
ControlMethod::toString(ControlMethod::Method method)
{
  QString str = "";
  switch(method) {
    case ControlMethod::Port:
      str = "ControlPort";
      break;

    case ControlMethod::Socket:
      str = "ControlSocket";
      break;
  }

  return str;
}

ControlMethod::Method
ControlMethod::fromString(QString method)
{
  if(method == QString("ControlPort"))
    return ControlMethod::Port;
  else// if(method == QString("ControlSocket"))
    return ControlMethod::Socket;
}
