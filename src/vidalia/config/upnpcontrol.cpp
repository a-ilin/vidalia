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
  return 0;
}

/** Based on http://miniupnp.free.fr/files/download.php?file=xchat-upnp20061022.patch */
void
UPNPControl::init_upnp()
{
  struct UPNPDev * devlist;
  int retval;

  printf("TB : init_upnp()\n");

  memset(&urls, 0, sizeof(struct UPNPUrls));
  memset(&data, 0, sizeof(struct IGDdatas));

  devlist = upnpDiscover(2000, NULL, NULL);
  retval = UPNP_GetValidIGD(devlist, &urls, &data, lanaddr, sizeof(lanaddr));
  printf("UPNP: %d", retval);

  freeUPNPDevlist(devlist);
}

/** Based on http://miniupnp.free.fr/files/download.php?file=xchat-upnp20061022.patch */
void
UPNPControl::upnp_add_redir(const char * addr, int port)
{
	char port_str[16];
	int r;
	printf("TB : upnp_add_redir (%s, %d)\n", addr, port);
	if(urls.controlURL[0] == '\0')
	{
		printf("TB : the init was not done !\n");
		fflush(stdout);
		return;
	}
	
	r = UPNP_AddPortMapping(urls.controlURL, data.servicetype,
	                        port_str, port_str, addr, 0, "TCP");
	if(r==0)
		printf("AddPortMapping(%s, %s, %s) failed\n", port_str, port_str, addr);
	fflush(stdout);
}

/** Based on http://miniupnp.free.fr/files/download.php?file=xchat-upnp20061022.patch */
void
UPNPControl::upnp_rem_redir(int port)
{
	char port_str[16];
	int t;
	printf("TB : upnp_rem_redir (%d)\n", port);
	if(urls.controlURL[0] == '\0')
	{
		printf("TB : the init was not done !\n");
		fflush(stdout);
		return;
	}
	sprintf(port_str, "%d", port);
	UPNP_DeletePortMapping(urls.controlURL, data.servicetype, port_str, "TCP");
}
