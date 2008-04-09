/*
**  This file is part of Vidalia, and is subject to the license terms in the
**  LICENSE file, found in the top level directory of this distribution. If 
**  you did not receive the LICENSE file with this file, you may obtain it
**  from the Vidalia source package distributed by the Vidalia Project at
**  http://www.vidalia-project.net/. No part of Vidalia, including this file,
**  may be copied, modified, propagated, or distributed except according to
**  the terms described in the LICENSE file.
*/

/* 
** \file upnpcontrol.h
** \version $Id$
** \brief Singleton object for interacting with UPNP device
*/

#ifndef _UPNPCONTROL_H
#define _UPNPCONTROL_H

#include <QObject>

#define STATICLIB
#include <miniupnpc/miniwget.h>
#include <miniupnpc/miniupnpc.h>
#include <miniupnpc/upnpcommands.h>

class UPNPControl : public QObject
{
  Q_OBJECT

public:
  static UPNPControl* Instance();
  int forwardPort(quint16 port);
  int disableForwarding();
protected:
  UPNPControl();
private:
  static UPNPControl* pInstance;

  /** Used by miniupnpc library */
  struct UPNPUrls urls;
  struct IGDdatas data;
  char lanaddr[16];
  void init_upnp();
  void upnp_add_redir (const char * addr, int port);
  void upnp_rem_redir(int port);

  /* Currently forwarded port */
  quint16 forwardedPort;
};

#endif 
