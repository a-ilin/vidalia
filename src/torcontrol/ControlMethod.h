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
** \file ControlMethod.h
** \brief Class for specifying which method is used to communicate to Tor
*/

#ifndef _CONTROLMETHOD_H
#define _CONTROLMETHOD_H

#include <QString>

class ControlMethod
{
  public:
    enum Method { Port, Socket };
    static QString toString(ControlMethod::Method method);
    static Method fromString(QString method);
};

#endif
