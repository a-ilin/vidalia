#!/bin/sh
#
#  $Id$
# 
#  Vidalia is distributed under the following license:
#
#  Copyright (C) 2006,  Matt Edman, Justin Hipple
#
#  This program is free software; you can redistribute it and/or
#  modify it under the terms of the GNU General Public License
#  as published by the Free Software Foundation; either version 2
#  of the License, or (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  
#  02110-1301, USA.
#################################################################

DIRURL="http://tor.noreply.org/tor/status/all"
GEOIPURL="http://geoip.vidalia-project.net/cgi-bin/geoip"
CACHEFILE="geoip-cache"
timestamp=$(date -u +%s)

# Fetch a list of server IP addresses
ipaddrs=$(wget -q -O - "$DIRURL" | awk '$1 == "r" { print $7 }' | sort | uniq | tr "\n" ",")

# Get GeoIP information for each IP address
geoips=$(wget -q -O - --post-data="ip=$ipaddrs" "$GEOIPURL")

# Cache the GeoIP information with timestamps
IFS=$'\n'
rm -f $CACHEFILE
for geoip in $geoips; do
  echo "$geoip:$timestamp" >> "$CACHEFILE"
done

