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
** \file RouterDescriptor.cpp
** \brief Parses a blob of router descriptor text from Tor
*/

#include "RouterDescriptor.h"

#include <QtGlobal>


/** Constructor. Just assigns the ID and determines whether the router is
 * responsive or not based on the presence of a "!" at the start of the ID.
 * See tor-spec.txt for details. */
RouterDescriptor::RouterDescriptor(QStringList descriptor, bool microdesc)
  : _microdesc(microdesc), _observedBandwidth(0), _uptime(0) {
  _status = Online;
  parseDescriptor(descriptor);
}

/** Parses this router's descriptor for relevant information. */
void
RouterDescriptor::parseDescriptor(QStringList descriptor)
{
  if(_microdesc) {
    bool key = false;
    foreach(QString line, descriptor) {
      if(line.startsWith("onion-key")) {
        key = true;
      } else if(line.startsWith("p ")) {
        _exitPolicy = line.remove(0,qstrlen("p "));
      } else if(line.startsWith("family ")) {
        _family = line.remove(0,qstrlen("family "));
      }

      if(key)
        _onionKey += line;
      if(line.startsWith("-----END RSA PUBLIC KEY-----"))
        key = false;
    }
  } else {
    foreach (QString line, descriptor) {
      if (line.startsWith("router ")) {
        QStringList parts = line.remove(0,qstrlen("router ")).split(" ");
        _name    = parts.at(0);
        _ip      = QHostAddress(parts.at(1));
        _orPort  = (quint16)parts.at(2).toUInt();
        _dirPort = (quint16)parts.at(4).toUInt();
      } else if (line.startsWith("platform ")) {
        _platform = line.remove(0,qstrlen("platform "));
      } else if (line.startsWith("published ")) {
        _published = QDateTime::fromString(
                                           line.remove(0,qstrlen("published ")),
                                           "yyyy-MM-dd HH:mm:ss");
        _published.setTimeSpec(Qt::UTC);
      } else if (line.startsWith("opt fingerprint ")) {
        _fingerprint = line.remove(0,qstrlen("opt fingerprint "));
        _id = _fingerprint.remove(" ");
      } else if (line.startsWith("fingerprint ")) {
        _fingerprint = line.remove(0,qstrlen("fingerprint "));
        _id = _fingerprint.remove(" ");
      } else if (line.startsWith("uptime ")) {
        _uptime = (quint64)line.remove(0,qstrlen("uptime ")).toULongLong();
      } else if (line.startsWith("bandwidth ")) {
        QStringList bw = line.remove(0,qstrlen("bandwidth ")).split(" ");
        _avgBandwidth      = (quint64)bw.at(0).toULongLong();
        _burstBandwidth    = (quint64)bw.at(1).toULongLong();
        _observedBandwidth = (quint64)bw.at(2).toULongLong();
      } else if (line.startsWith("contact ")) {
        _contact = line.remove(0,qstrlen("contact "));
      } else if (line.startsWith("hibernating ")) {
        if (line.remove(0,qstrlen("hibernating ")).trimmed() == "1") {
          _status = Hibernating;
        }
      }
    }
  }
}

/** Returns a string representation of the status of this router. */
QString
RouterDescriptor::status()
{
  if (_status == Online) {
    return tr("Online");
  } else if (_status == Hibernating) {
    return tr("Hibernating");
  }
  return tr("Offline");
}

void
RouterDescriptor::appendRouterStatusInfo(const RouterStatus &rs)
{
  _id = rs.id();
  _name = rs.name();
  _ip = rs.ipAddress();
  _orPort = rs.orPort();
  _dirPort = rs.dirPort();
  _avgBandwidth = rs.bandwidth();
  _burstBandwidth = rs.bandwidth();
  _observedBandwidth = rs.bandwidth();
}
