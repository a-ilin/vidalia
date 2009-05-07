#!/usr/bin/python
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

from mod_python import apache
from mod_python import util
import GeoIP
import os

def escape_output(f):
  f.replace("\\", "\\\\")
  f.replace("\"", "\\\"")
  return ("\"%s\"" % f)

# Get the form fields
def handler(req):
  form = util.FieldStorage(req)

  # Get the specified output format and verify it is a valid format. If
  # none is specified, default to the old "short" output format.
  format = form.getfirst("format", "short").lower()
  if format not in ("short", "long"):
    format = "short"

  # Get a list of all IP addresses in this request. If none is specified,
  # display the GeoIP information for the client's address.
  iplist = []
  if form.has_key("ip"):
    for ip in form.getlist("ip"):
      iplist.extend(ip.split(","))
  else:
    iplist.append(req.connection.remote_ip)

  # Open the GeoIP database
  gi = GeoIP.open("/var/lib/geoip/GeoLiteCity.dat", GeoIP.GEOIP_MEMORY_CACHE)

  # Display the output according to the specified format
  req.content_type = 'text/plain'

  if format == "long":
    for ip in iplist:
      r = gi.record_by_addr(ip)
      if r != None:
        fields = []
        fields.append("IP=%s" % ip)
        fields.append("LAT=%f" % r['latitude'])
        fields.append("LON=%f" % r['longitude'])
        fields.append("CITY=%s" % escape_output(r['city']))
        fields.append("REGION=%s" % escape_output(r['region_name']))
        fields.append("COUNTRY=%s" % escape_output(r['country_name']))
        fields.append("CC=%s" % r['country_code'])

        r = gi.range_by_ip(ip)
        if len(r) == 2:
          fields.append("FROM=%s" % r[0])
          fields.append("TO=%s" % r[1])
        
        response = " ".join(fields)
        req.write(response)

  elif format == "short":
    for ip in iplist:
      r = gi.record_by_addr(ip)
      if r != None:
        response = "%s,%s,%s,%s,%f,%f" % (ip, r['city'].replace(",", " "), r['region'],
                                          r['country_code'],r['latitude'],r['longitude'])
        req.write(response)

  return apache.OK

