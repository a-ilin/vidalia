#include <QHostAddress>

#include "controlconnection.h"

ControlConnection::ControlConnection()
{
}

ControlConnection::~ControlConnection()
{
}

/** Connects to Tor's control socket on the specified host and port. If the
 * connection is successful, true is returned. If the connection fails, then
 * this function returns false and sets <b>errmsg</b> appropriately, if not
 * null. */
bool
ControlConnection::connect(QHostAddress addr, quint16 port, QString *errmsg)
{
  connectToHost(addr, port);
  if (!waitForConnected(-1)) {
    if (errmsg) {
      *errmsg = errorString();
    }
    return false;
  }
  return true;
}

/** Disconnects from Tor's control socket */
void
ControlConnection::disconnect()
{
  disconnectFromHost();
}

