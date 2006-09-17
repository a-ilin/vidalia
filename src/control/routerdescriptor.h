/****************************************************************
 *  Vidalia is distributed under the following license:
 *
 *  Copyright (C) 2006,  Matt Edman, Justin Hipple
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, 
 *  Boston, MA  02110-1301, USA.
 ****************************************************************/

/** 
 * \file routerdescriptor.h
 * \version $Id$
 */

#ifndef _ROUTERDESCRIPTOR_H
#define _ROUTERDESCRIPTOR_H

#include <QStringList>
#include <QDateTime>
#include <QList>


class RouterDescriptor
{
public:
  /** Possible router states. */
  enum RouterStatus {
    Online,       /**< Router is online and reachable. */
    Hibernating,  /**< Router is currently hibernating. */
    Offline       /**< Router is unresponsive. */
  };

  /** Default constructor. */
  RouterDescriptor() {}
  /** Constructor. */ 
  RouterDescriptor(QStringList descriptor);
  
  /** Returns the router's name. */
  QString name() const { return _name; }
  /** Returns the router's IP address. */
  QString ip() const { return _ip; }
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

private:
  /** Parses this router's descriptor for relevant information. */
  void parseDescriptor(QStringList descriptor);

  RouterStatus _status;    /**< Availability status of this router. */
  QString _id;             /**< Router's descriptor ID. */
  QString _name;           /**< The router's name. */
  QString _fingerprint;    /**< Router's fingerprint. */
  QString _platform;       /**< Platform on which router is running. */
  QString _contact;        /**< Router operator contact information. */
  QString _ip;             /**< Router's IP address. */
  quint16 _orPort;         /**< Router's ORPort. */
  quint16 _dirPort;        /**< Router's DirPort. */
  QDateTime _published;    /**< Date router descriptor was published. */
  quint64 _uptime;         /**< Time the router has been online. */
  quint64 _avgBandwidth;   /**< Average bandwidth. */
  quint64 _burstBandwidth; /**< Burst bandwidth. */
  quint64 _observedBandwidth; /**< Observed bandwidth. */
  QString _location;       /**< Geographic location information. */
};

#endif

