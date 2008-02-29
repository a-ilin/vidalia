#!/bin/sh
##
##  $Id$
## 
##  This file is part of Vidalia, and is subject to the license terms in the
##  LICENSE file, found in the top level directory of this distribution. If 
##  you did not receive the LICENSE file with this file, you may obtain it
##  from the Vidalia source package distributed by the Vidalia Project at
##  http://www.vidalia-project.net/. No part of Vidalia, including this file,
##  may be copied, modified, propagated, or distributed except according to
##  the terms described in the LICENSE file.
##


DIRURL="http://tor.noreply.org/tor/status/all"
GEOIPURL="http://geoip.vidalia-project.net/cgi-bin/geoip"
CACHEFILE="geoip-cache"

if [ "$1" == "-notimestamp" -o "$1" == "--notimestamp" ]; then
  timestamp=""
else
  timestamp=":$(date +%s)"
fi

# Fetch a list of server IP addresses
ipaddrs=$(wget -q -O - "$DIRURL" | awk '$1 == "r" { print $7 }' | sort | uniq | tr "\n" ",")

# Get GeoIP information for each IP address
geoips=$(wget -q -O - --post-data="ip=$ipaddrs" "$GEOIPURL")

# Cache the GeoIP information with timestamps
IFS=$'\n'
rm -f $CACHEFILE
for geoip in $geoips; do
  echo "$geoip""$timestamp" >> "$CACHEFILE"
done

