
#include <QHostAddress>

#include "../config/vidaliasettings.h"
#include "torcontrol.h"

/** Default constructor */
TorControl::TorControl()
{
}

/** Default destructor */
TorControl::~TorControl()
{
}

/** Start the Tor process. Returns true if the process was successfully
 * started, otherwise returns false. */
bool
TorControl::startTor(QString *errmsg)
{
  return false;
}


/** Stop the Tor process. */
void
TorControl::stopTor()
{
}

/** Connect to Tor's control port. The control port to use is determined by
 * Vidalia's configuration file. */
bool
TorControl::connect(QString *errmsg)
{
  VidaliaSettings settings;
  return _controlConn.connect(QHostAddress::LocalHost,
                              settings.getControlPort(), errmsg);
}

/** Disconnect from Tor's control port */
void
TorControl::disconnect()
{
  _controlConn.disconnect();
}

