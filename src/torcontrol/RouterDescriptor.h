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
** \file RouterDescriptor.h
** \brief Parses a blob of router descriptor text from Tor
*/

#ifndef _ROUTERDESCRIPTOR_H
#define _ROUTERDESCRIPTOR_H

#include <QCoreApplication>
#include <QStringList>
#include <QDateTime>
#include <QList>
#include <QHostAddress>

#include "RouterStatus.h"

class RouterDescriptor
{
  Q_DECLARE_TR_FUNCTIONS(RouterDescriptor)

public:
  /** Possible router states. */
  enum RouterStatusEnum {
    Online,       /**< Router is online and reachable. */
    Hibernating,  /**< Router is currently hibernating. */
    Offline       /**< Router is unresponsive. */
  };

  /** Default constructor. */
  RouterDescriptor(bool microdesc = false) : _microdesc(microdesc) {}
  /** Constructor. */ 
  RouterDescriptor(QStringList descriptor, bool microdesc = false);
  
  /** Returns the router's name. */
  QString name() const { return _name; }
  /** Returns the router's IP address. */
  QHostAddress ip() const { return _ip; }
  /** Returns the router's ORPort. */
  quint16 orPort() const { return _orPort; }
  /** Returns the router's DirPort. */
  quint16 dirPort() const { return _dirPort; }
  /** Returns the router's ID. */
  QString id() const { return _id; }
  /** Returns the platform on which this router is running. */
  QString platform() const { return _platform; }
  /** Returns the length of time this router has been up. */
  quint64 uptime() const { return _uptime; }
  /** Returns the router's contact information. */
  QString contact() const { return _contact; }
  /** Returns the date and time the router was published. */
  QDateTime published() const { return _published; }
  /** Returns the fingerprint for this router. */
  QString fingerprint() const { return _fingerprint; }
  /** Returns the average bandwidth for this router. */
  quint64 averageBandwidth() const { return _avgBandwidth; }
  /** Returns the burst bandwidth for this router. */
  quint64 burstBandwidth() const { return _burstBandwidth; }
  /** Returns the observed bandwidth for this router. */
  quint64 observedBandwidth() const { return _observedBandwidth; }
  /** Returns true if this router is online and responsive. */
  bool online() const { return _status == Online; }
  /** Returns true if this router is unresponsive. */
  bool offline() const { return _status == Offline; }
  /** Returns true if this router is hibernating. */
  bool hibernating() const { return _status == Hibernating; }
  /** Returns true if the router has neither a nickname or an ID. */
  bool isEmpty() { return (_id.isEmpty() && _name.isEmpty()); }
  /** Returns a string representation of the status of this router. */
  QString status();
  
  /** Returns geographic location information for this router. Note that this
   * information is NOT part of the Tor directory protocol, but can be
   * determined out of band and set using setLocation(). */
  QString location() const { return _location; }
  /** Sets geographic location information for this router. */
  void setLocation(QString location) { _location = location; }
  /** Sets the descriptors status to Offline if <b>offline</b> is true. */
  void setOffline(bool offline) { _status = (offline ? Offline : Online); }

  /** Microdescriptor */
  /** Returns the onion key for this router */
  QString onionKey() const { return _onionKey; }
  /** Returns this router's family */
  QString family() const { return _family; }
  /** Returns this router's exit policy */
  QString exitPolicy() const { return _exitPolicy; }

  /** Uses the RouterStatus information to update key elements of the
   *  descriptor */
  void appendRouterStatusInfo(const RouterStatus &rs);

private:
  /** Parses this router's descriptor for relevant information. */
  void parseDescriptor(QStringList descriptor);

  RouterStatusEnum _status;/**< Availability status of this router. */
  QString _id;             /**< Router's descriptor ID. */
  QString _name;           /**< The router's name. */
  QString _fingerprint;    /**< Router's fingerprint. */
  QString _platform;       /**< Platform on which router is running. */
  QString _contact;        /**< Router operator contact information. */
  QHostAddress _ip;        /**< Router's IP address. */
  quint16 _orPort;         /**< Router's ORPort. */
  quint16 _dirPort;        /**< Router's DirPort. */
  QDateTime _published;    /**< Date router descriptor was published. */
  quint64 _uptime;         /**< Time the router has been online. */
  quint64 _avgBandwidth;   /**< Average bandwidth. */
  quint64 _burstBandwidth; /**< Burst bandwidth. */
  quint64 _observedBandwidth; /**< Observed bandwidth. */
  QString _location;       /**< Geographic location information. */

  /** Microdescriptor info */
  bool _microdesc;         /** */
  QString _onionKey;       /** Router SSL key */
  QString _exitPolicy;     /** Exit policy */
  QString _family;         /** Family */
};

#endif

