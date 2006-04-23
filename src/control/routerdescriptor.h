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
  RouterDescriptor(QString id); 
  /** Constructor. */ 
  RouterDescriptor(QString id, QStringList descriptor);
  
  /** Returns the entire, unparsed router descriptor. */
  QString descriptor() { return _descriptor.join("\n"); }
  /** Returns the router's name. */
  QString name() { return _name; }
  /** Returns the router's IP address. */
  QString ip() { return _ip; }
  /** Returns the router's ORPort. */
  quint16 orPort() { return _orPort; }
  /** Returns the router's DirPort. */
  quint16 dirPort() { return _dirPort; }
  /** Returns the router's ID. */
  QString id() { return _id; }
  /** Returns the platform on which this router is running. */
  QString platform() { return _platform; }
  /** Returns the length of time this router has been up. */
  quint64 uptime() { return _uptime; }
  /** Returns the router's contact information. */
  QString contact() { return _contact; }
  /** Returns the date and time the router was published. */
  QDateTime published() { return _published; }
  /** Returns the fingerprint for this router. */
  QString fingerprint() { return _fingerprint; }
  /** Returns the average bandwidth for this router. */
  quint64 averageBandwidth() { return _avgBandwidth; }
  /** Returns the burst bandwidth for this router. */
  quint64 burstBandwidth() { return _burstBandwidth; }
  /** Returns the observed bandwidth for this router. */
  quint64 observedBandwidth() { return _observedBandwidth; }
  /** Returns true if this router is online and responsive. */
  bool online() { return _status == Online; }
  /** Returns true if this router is unresponsive. */
  bool offline() { return _status == Offline; }
  /** Returns true if this router is hibernating. */
  bool hibernating() { return _status == Hibernating; }
  /** Returns a string representation of the status of this router. */
  QString status();

private:
  /** Parses this router's descriptor for relevant information. */
  void parseDescriptor();

  RouterStatus _status;    /**< Availability status of this router. */
  QStringList _descriptor; /**< The router descriptor, in all its glory. */
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
};

#endif

