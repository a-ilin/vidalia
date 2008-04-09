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
** \file upnpcontrol.cpp
** \version $Id$
** \brief Singleton object for interacting with UPNP device
*/

#include "upnpcontrol.h"

UPNPControl* UPNPControl::pInstance = 0;
UPNPControl* UPNPControl::Instance()
{
  if (0 == pInstance)
    pInstance = new UPNPControl;
  return pInstance;
}

UPNPControl::UPNPControl()
{
  init_upnp();
  forwardedPort = 0;
}

int
UPNPControl::forwardPort(quint16 port)
{
  int retval;
  
  char sPort[6];
  
  char intClient[16];
  char intPort[6];

  // Convert the port number to a string
  snprintf(sPort, sizeof(sPort), "%d", port);

  // Add the port mapping of external:port -> internal:port
  retval = UPNP_AddPortMapping(urls.controlURL, data.servicetype,
			       sPort, sPort, lanaddr, "Tor server", "TCP");
  if(UPNPCOMMAND_SUCCESS != retval) {
    printf("AddPortMapping(%s, %s, %s) failed with code %d\n",
	   sPort, sPort, lanaddr, retval);
    return 1;
  }
  
  // Check if the port mapping was accepted
  retval = UPNP_GetSpecificPortMappingEntry(urls.controlURL,
					    data.servicetype,
					    sPort, "TCP",
					    intClient, intPort);
  if(UPNPCOMMAND_SUCCESS != retval) {
    printf("GetSpecificPortMappingEntry() failed with code %d\n", retval);
    return 2;
  }
  
  if(! intClient[0]) {
    printf("GetSpecificPortMappingEntry failed.\n");
    return 3;
  }
  
  // Output the mapping
  printf("(external):%s -> %s:%s\n", sPort, intClient, intPort);
  fflush(stdout);

  // Save the mapping
  forwardedPort = port;

  return 0;
}

int
UPNPControl::disableForwarding()
{
  char sPort[6];

  if (0 == forwardedPort)
    return 0;

  // Convert the port number to a string
  snprintf(sPort, sizeof(sPort), "%d", forwardedPort);

  int retval = UPNP_DeletePortMapping(urls.controlURL, data.servicetype, sPort, "TCP");
  if(UPNPCOMMAND_SUCCESS != retval) {
    printf("DeletePortMapping() failed with code %d\n", retval);
    return 1;
  }

  // Output the cancelled mapping
  printf("(external):%s -> <>\n", sPort);
  fflush(stdout);

  // Save the mapping
  forwardedPort = 0;

  return 0;
}


/** Based on http://miniupnp.free.fr/files/download.php?file=xchat-upnp20061022.patch */
void
UPNPControl::init_upnp()
{
  struct UPNPDev * devlist;
  int retval;

  memset(&urls, 0, sizeof(struct UPNPUrls));
  memset(&data, 0, sizeof(struct IGDdatas));

  devlist = upnpDiscover(2000, NULL, NULL);
  retval = UPNP_GetValidIGD(devlist, &urls, &data, lanaddr, sizeof(lanaddr));
  printf("GetValidIGD returned: %d\n", retval);
  fflush(stdout);

  freeUPNPDevlist(devlist);
}
