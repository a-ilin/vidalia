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
** \file TransportSettings.cpp
** \brief Pluggable transport settings
*/

#include "TransportSettings.h"

TransportSettings::TransportSettings()
  : VSettings("Transport")
{

}

/** Returns the available transports */
QStringList
TransportSettings::getTransports()
{
  return childKeys();
}

/** Returns the ServerTransportPlugin value for a given transport */
QString
TransportSettings::getSTP(const QString &transport)
{
  return value(transport, "").toString();
}
