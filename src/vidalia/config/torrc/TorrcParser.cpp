/*
**  This file is part of Vidalia, and is subject to the license terms in the
**  LICENSE file, found in the top level directory of this distribution. If you
**  did not receive the LICENSE file with this file, you may obtain it from the
**  Vidalia source package distributed by the Vidalia Project at
**  http://www.torproject.org/projects/vidalia.html. No part of Vidalia, 
**  including this file, may be copied, modified, propagated, or distributed 
**  except according to the terms described in the LICENSE file.
*/

/*
** \file TorrcParser.cpp
** \brief Parser for the torrc file and other data representations for the file
*/

#include "TorrcParser.h"
#include "Vidalia.h"

TorrcParser::TorrcParser()
{
  _torOpts << TorOpt("AccountingMax", TorOpt::DataSize, "0 bytes");
  _torOpts << TorOpt("AccountingStart", TorOpt::String, "");
  _torOpts << TorOpt("Address", TorOpt::String, "");
  _torOpts << TorOpt("AllowDotExit", TorOpt::Boolean, "0");
  _torOpts << TorOpt("AllowInvalidNodes", TorOpt::CommaList, "middle,rendezvous");
  _torOpts << TorOpt("AllowNonRFC953Hostnames", TorOpt::Boolean, "0");
  _torOpts << TorOpt("AllowSingleHopCircuits", TorOpt::Boolean, "0");
  _torOpts << TorOpt("AllowSingleHopExits", TorOpt::Boolean, "0");
  _torOpts << TorOpt("AlternateBridgeAuthority", TorOpt::LineList, "");
  _torOpts << TorOpt("AlternateDirAuthority", TorOpt::LineList, "");
  _torOpts << TorOpt("AlternateHSAuthority", TorOpt::LineList, "");
  _torOpts << TorOpt("AssumeReachable", TorOpt::Boolean, "0");
  _torOpts << TorOpt("AuthDirBadDir", TorOpt::LineList, "");
  _torOpts << TorOpt("AuthDirBadExit", TorOpt::LineList, "");
  _torOpts << TorOpt("AuthDirInvalid", TorOpt::LineList, "");
  _torOpts << TorOpt("AuthDirReject", TorOpt::LineList, "");
  _torOpts << TorOpt("AuthDirRejectUnlisted", TorOpt::Boolean, "0");
  _torOpts << TorOpt("AuthDirListBadDirs", TorOpt::Boolean, "0");
  _torOpts << TorOpt("AuthDirListBadExits", TorOpt::Boolean, "0");
  _torOpts << TorOpt("AuthDirMaxServersPerAddr", TorOpt::Integer, "2");
  _torOpts << TorOpt("AuthDirMaxServersPerAuthAddr", TorOpt::Integer, "5");
  _torOpts << TorOpt("AuthoritativeDirectory", TorOpt::Boolean, "0");
  _torOpts << TorOpt("AutomapHostsOnResolve", TorOpt::Boolean, "0");
  _torOpts << TorOpt("AutomapHostsSuffixes", TorOpt::CommaList, ".onion,.exit");
  _torOpts << TorOpt("AvoidDiskWrites", TorOpt::Boolean, "0");
  _torOpts << TorOpt("BandwidthBurst", TorOpt::DataSize, "10 MB");
  _torOpts << TorOpt("BandwidthRate", TorOpt::DataSize, "5 MB");
  _torOpts << TorOpt("BridgeAuthoritativeDir", TorOpt::Boolean, "0");
  _torOpts << TorOpt("Bridge", TorOpt::LineList, "");
  _torOpts << TorOpt("BridgePassword", TorOpt::String, "");
  _torOpts << TorOpt("BridgeRecordUsageByCountry", TorOpt::Boolean, "1");
  _torOpts << TorOpt("BridgeRelay", TorOpt::Boolean, "0");
  _torOpts << TorOpt("CellStatistics", TorOpt::Boolean, "0");
  _torOpts << TorOpt("LearnCircuitBuildTimeout", TorOpt::Boolean, "1");
  _torOpts << TorOpt("CircuitBuildTimeout", TorOpt::TimeInterval, "0");
  _torOpts << TorOpt("CircuitIdleTimeout", TorOpt::TimeInterval, "1 hour");
  _torOpts << TorOpt("CircuitStreamTimeout", TorOpt::TimeInterval, "0");
  _torOpts << TorOpt("CircuitPriorityHalflife", TorOpt::Float, "-100.0");
  _torOpts << TorOpt("ClientDNSRejectInternalAddresses", TorOpt::Boolean, "1");
  _torOpts << TorOpt("ClientOnly", TorOpt::Boolean, "0");
  _torOpts << TorOpt("ClientRejectInternalAddresses", TorOpt::Boolean, "1");
  _torOpts << TorOpt("ClientTransportPlugin", TorOpt::LineList, "");
  _torOpts << TorOpt("ConsensusParams", TorOpt::String, "");
  _torOpts << TorOpt("ConnLimit", TorOpt::Integer, "1000");
  _torOpts << TorOpt("ConnDirectionStatistics", TorOpt::Boolean, "0");
  _torOpts << TorOpt("ConstrainedSockets", TorOpt::Boolean, "0");
  _torOpts << TorOpt("ConstrainedSockSize", TorOpt::DataSize, "8192");
  _torOpts << TorOpt("ContactInfo", TorOpt::String, "");
  _torOpts << TorOpt("ControlListenAddress", TorOpt::LineList, "");
  _torOpts << TorOpt("ControlPort", TorOpt::Port, "0");
  _torOpts << TorOpt("ControlPortFileGroupReadable", TorOpt::Boolean, "0");
  _torOpts << TorOpt("ControlPortWriteToFile", TorOpt::Filename, "");
  _torOpts << TorOpt("ControlSocket", TorOpt::LineList, "");
  _torOpts << TorOpt("ControlSocketsGroupWritable", TorOpt::Boolean, "0");
  _torOpts << TorOpt("CookieAuthentication", TorOpt::Boolean, "0");
  _torOpts << TorOpt("CookieAuthFileGroupReadable", TorOpt::Boolean, "0");
  _torOpts << TorOpt("CookieAuthFile", TorOpt::String, "");
  _torOpts << TorOpt("CountPrivateBandwidth", TorOpt::Boolean, "0");
  _torOpts << TorOpt("DataDirectory", TorOpt::Filename, "");
  _torOpts << TorOpt("DirAllowPrivateAddresses", TorOpt::Boolean, "");
  _torOpts << TorOpt("TestingAuthDirTimeToLearnReachability", TorOpt::TimeInterval, "30 minutes");
  _torOpts << TorOpt("DirListenAddress", TorOpt::LineList, "");
  _torOpts << TorOpt("DirPolicy", TorOpt::LineList, "");
  _torOpts << TorOpt("DirPort", TorOpt::Port, "0");
  _torOpts << TorOpt("DirPortFrontPage", TorOpt::Filename, "");
  _torOpts << TorOpt("DirReqStatistics", TorOpt::Boolean, "1");
  _torOpts << TorOpt("DirServer", TorOpt::LineList, "");
  _torOpts << TorOpt("DisableAllSwap", TorOpt::Boolean, "0");
  _torOpts << TorOpt("DisableIOCP", TorOpt::Boolean, "1");
  _torOpts << TorOpt("DNSPort", TorOpt::LineList, "");
  _torOpts << TorOpt("DNSListenAddress", TorOpt::LineList, "");
  _torOpts << TorOpt("DownloadExtraInfo", TorOpt::Boolean, "0");
  _torOpts << TorOpt("EnforceDistinctSubnets", TorOpt::Boolean, "1");
  _torOpts << TorOpt("EntryNodes", TorOpt::RouterList, "");
  _torOpts << TorOpt("EntryStatistics", TorOpt::Boolean, "0");
  _torOpts << TorOpt("TestingEstimatedDescriptorPropagationTime", TorOpt::TimeInterval, "10 minutes");
  _torOpts << TorOpt("ExcludeNodes", TorOpt::RouterList, "");
  _torOpts << TorOpt("ExcludeExitNodes", TorOpt::RouterList, "");
  _torOpts << TorOpt("ExcludeSingleHopRelays", TorOpt::Boolean, "1");
  _torOpts << TorOpt("ExitNodes", TorOpt::RouterList, "");
  _torOpts << TorOpt("ExitPolicy", TorOpt::LineList, "");
  _torOpts << TorOpt("ExitPolicyRejectPrivate", TorOpt::Boolean, "1");
  _torOpts << TorOpt("ExitPortStatistics", TorOpt::Boolean, "0");
  _torOpts << TorOpt("ExtraInfoStatistics", TorOpt::Boolean, "1");
  _torOpts << TorOpt("FallbackNetworkstatusFile", TorOpt::Filename, "/usr/local/share/tor/fallback-consensus");
  _torOpts << TorOpt("FascistFirewall", TorOpt::Boolean, "0");
  _torOpts << TorOpt("FirewallPorts", TorOpt::CommaList, "");
  _torOpts << TorOpt("FastFirstHopPK", TorOpt::Boolean, "1");
  _torOpts << TorOpt("FetchDirInfoEarly", TorOpt::Boolean, "0");
  _torOpts << TorOpt("FetchDirInfoExtraEarly", TorOpt::Boolean, "0");
  _torOpts << TorOpt("FetchServerDescriptors", TorOpt::Boolean, "1");
  _torOpts << TorOpt("FetchHidServDescriptors", TorOpt::Boolean, "1");
  _torOpts << TorOpt("FetchUselessDescriptors", TorOpt::Boolean, "0");
  _torOpts << TorOpt("FetchV2Networkstatus", TorOpt::Boolean, "0");
  _torOpts << TorOpt("GeoIPFile", TorOpt::Filename, "/usr/local/share/tor/geoip");
  _torOpts << TorOpt("GiveGuardFlagTo_CVE_2011_2768_VulnerableRelays", TorOpt::Boolean, "0");
  _torOpts << TorOpt("HardwareAccel", TorOpt::Boolean, "0");
  _torOpts << TorOpt("HeartbeatPeriod", TorOpt::TimeInterval, "6 hours");
  _torOpts << TorOpt("AccelName", TorOpt::String, "");
  _torOpts << TorOpt("AccelDir", TorOpt::Filename, "");
  // HashedControlPassword will only use one value, the salt prevents
  // treat it like the rest of the LineLists
  _torOpts << TorOpt("HashedControlPassword", TorOpt::String, "");
  _torOpts << TorOpt("HidServDirectoryV2", TorOpt::Boolean, "1");
  _torOpts << TorOpt("HiddenServiceDir", TorOpt::Dependant, "");
  _torOpts << TorOpt("HiddenServiceOptions", TorOpt::Virtual, "");
  _torOpts << TorOpt("HiddenServicePort", TorOpt::Dependant, "");
  _torOpts << TorOpt("HiddenServiceVersion", TorOpt::Dependant, "");
  _torOpts << TorOpt("HiddenServiceAuthorizeClient", TorOpt::Dependant, "");
  _torOpts << TorOpt("HidServAuth", TorOpt::LineList, "");
  _torOpts << TorOpt("HSAuthoritativeDir", TorOpt::Boolean, "0");
  _torOpts << TorOpt("HTTPProxy", TorOpt::String, "");
  _torOpts << TorOpt("HTTPProxyAuthenticator", TorOpt::String, "");
  _torOpts << TorOpt("HTTPSProxy", TorOpt::String, "");
  _torOpts << TorOpt("HTTPSProxyAuthenticator", TorOpt::String, "");
  _torOpts << TorOpt("ServerTransportPlugin", TorOpt::LineList, "");
  _torOpts << TorOpt("Socks4Proxy", TorOpt::String, "");
  _torOpts << TorOpt("Socks5Proxy", TorOpt::String, "");
  _torOpts << TorOpt("Socks5ProxyUsername", TorOpt::String, "");
  _torOpts << TorOpt("Socks5ProxyPassword", TorOpt::String, "");
  _torOpts << TorOpt("KeepalivePeriod", TorOpt::TimeInterval, "5 minutes");
  _torOpts << TorOpt("Log", TorOpt::LineList, "");
  _torOpts << TorOpt("LogMessageDomains", TorOpt::Boolean, "0");
  _torOpts << TorOpt("LogTimeGranularity", TorOpt::TimeMsecInterval, "1 second");
  _torOpts << TorOpt("LongLivedPorts", TorOpt::CommaList, "21,22,706,1863,5050,5190,5222,5223,6523,6667,6697,8300");
  _torOpts << TorOpt("MapAddress", TorOpt::LineList, "");
  _torOpts << TorOpt("MaxAdvertisedBandwidth", TorOpt::DataSize, "1 GB");
  _torOpts << TorOpt("MaxCircuitDirtiness", TorOpt::TimeInterval, "10 minutes");
  _torOpts << TorOpt("MaxClientCircuitsPending", TorOpt::Integer, "32");
  _torOpts << TorOpt("MaxOnionsPending", TorOpt::Integer, "100");
  _torOpts << TorOpt("MyFamily", TorOpt::String, "");
  _torOpts << TorOpt("NewCircuitPeriod", TorOpt::TimeInterval, "30 seconds");
  _torOpts << TorOpt("NamingAuthoritativeDirectory", TorOpt::Boolean, "0");
  _torOpts << TorOpt("NATDListenAddress", TorOpt::LineList, "");
  _torOpts << TorOpt("NATDPort", TorOpt::LineList, "");
  _torOpts << TorOpt("Nickname", TorOpt::String, "");
  _torOpts << TorOpt("WarnUnsafeSocks", TorOpt::Boolean, "1");
  _torOpts << TorOpt("NodeFamily", TorOpt::LineList, "");
  _torOpts << TorOpt("NumCPUs", TorOpt::Integer, "0");
  _torOpts << TorOpt("NumEntryGuards", TorOpt::Integer, "3");
  _torOpts << TorOpt("ORListenAddress", TorOpt::LineList, "");
  _torOpts << TorOpt("ORPort", TorOpt::Port, "0");
  _torOpts << TorOpt("OutboundBindAddress", TorOpt::String, "");
  _torOpts << TorOpt("PerConnBWBurst", TorOpt::DataSize, "0");
  _torOpts << TorOpt("PerConnBWRate", TorOpt::DataSize, "0");
  _torOpts << TorOpt("PidFile", TorOpt::String, "");
  _torOpts << TorOpt("TestingTorNetwork", TorOpt::Boolean, "0");
  _torOpts << TorOpt("OptimisticData", TorOpt::BooleanAuto, "auto");
  _torOpts << TorOpt("PortForwarding", TorOpt::Boolean, "0");
  _torOpts << TorOpt("PortForwardingHelper", TorOpt::Filename, "tor-fw-helper");
  _torOpts << TorOpt("PreferTunneledDirConns", TorOpt::Boolean, "1");
  _torOpts << TorOpt("ProtocolWarnings", TorOpt::Boolean, "0");
  _torOpts << TorOpt("PublishServerDescriptor", TorOpt::CommaList, "1");
  _torOpts << TorOpt("PublishHidServDescriptors", TorOpt::Boolean, "1");
  _torOpts << TorOpt("ReachableAddresses", TorOpt::LineList, "");
  _torOpts << TorOpt("ReachableDirAddresses", TorOpt::LineList, "");
  _torOpts << TorOpt("ReachableORAddresses", TorOpt::LineList, "");
  _torOpts << TorOpt("RecommendedVersions", TorOpt::LineList, "");
  _torOpts << TorOpt("RecommendedClientVersions", TorOpt::LineList, "");
  _torOpts << TorOpt("RecommendedServerVersions", TorOpt::LineList, "");
  _torOpts << TorOpt("RefuseUnknownExits", TorOpt::BooleanAuto, "auto");
  _torOpts << TorOpt("RejectPlaintextPorts", TorOpt::CommaList, "");
  _torOpts << TorOpt("RelayBandwidthBurst", TorOpt::DataSize, "0");
  _torOpts << TorOpt("RelayBandwidthRate", TorOpt::DataSize, "0");
  _torOpts << TorOpt("RendPostPeriod", TorOpt::TimeInterval, "1 hour");
  _torOpts << TorOpt("RephistTrackTime", TorOpt::TimeInterval, "24 hours");
  _torOpts << TorOpt("RunAsDaemon", TorOpt::Boolean, "0");
  _torOpts << TorOpt("SafeLogging", TorOpt::String, "1");
  _torOpts << TorOpt("SafeSocks", TorOpt::Boolean, "0");
  _torOpts << TorOpt("ServerDNSAllowBrokenConfig", TorOpt::Boolean, "1");
  _torOpts << TorOpt("ServerDNSAllowNonRFC953Hostnames", TorOpt::Boolean, "0");
  _torOpts << TorOpt("ServerDNSDetectHijacking", TorOpt::Boolean, "1");
  _torOpts << TorOpt("ServerDNSRandomizeCase", TorOpt::Boolean, "1");
  _torOpts << TorOpt("ServerDNSResolvConfFile", TorOpt::String, "");
  _torOpts << TorOpt("ServerDNSSearchDomains", TorOpt::Boolean, "0");
  _torOpts << TorOpt("ServerDNSTestAddresses", TorOpt::CommaList, "www.google.com,www.mit.edu,www.yahoo.com,www.slashdot.org");
  _torOpts << TorOpt("ShutdownWaitLength", TorOpt::TimeInterval, "30 seconds");
  _torOpts << TorOpt("SocksListenAddress", TorOpt::LineList, "");
  _torOpts << TorOpt("SocksPolicy", TorOpt::LineList, "");
  _torOpts << TorOpt("SocksPort", TorOpt::LineList, "");
  _torOpts << TorOpt("SocksTimeout", TorOpt::TimeInterval, "2 minutes");
  _torOpts << TorOpt("StrictNodes", TorOpt::Boolean, "0");
  _torOpts << TorOpt("TestSocks", TorOpt::Boolean, "0");
  _torOpts << TorOpt("TokenBucketRefillInterval", TorOpt::TimeMsecInterval, "100 msec");
  _torOpts << TorOpt("TrackHostExits", TorOpt::CommaList, "");
  _torOpts << TorOpt("TrackHostExitsExpire", TorOpt::TimeInterval, "30 minutes");
  _torOpts << TorOpt("TransListenAddress", TorOpt::LineList, "");
  _torOpts << TorOpt("TransPort", TorOpt::LineList, "");
  _torOpts << TorOpt("TunnelDirConns", TorOpt::Boolean, "1");
  _torOpts << TorOpt("UpdateBridgesFromAuthority", TorOpt::Boolean, "0");
  _torOpts << TorOpt("UseBridges", TorOpt::Boolean, "0");
  _torOpts << TorOpt("UseEntryGuards", TorOpt::Boolean, "1");
  _torOpts << TorOpt("UseMicrodescriptors", TorOpt::BooleanAuto, "auto");
  _torOpts << TorOpt("User", TorOpt::String, "");
  _torOpts << TorOpt("V1AuthoritativeDirectory", TorOpt::Boolean, "0");
  _torOpts << TorOpt("V2AuthoritativeDirectory", TorOpt::Boolean, "0");
  _torOpts << TorOpt("V3AuthoritativeDirectory", TorOpt::Boolean, "0");
  _torOpts << TorOpt("TestingV3AuthInitialVotingInterval", TorOpt::TimeInterval, "30 minutes");
  _torOpts << TorOpt("TestingV3AuthInitialVoteDelay", TorOpt::TimeInterval, "5 minutes");
  _torOpts << TorOpt("TestingV3AuthInitialDistDelay", TorOpt::TimeInterval, "5 minutes");
  _torOpts << TorOpt("V3AuthVotingInterval", TorOpt::TimeInterval, "1 hour");
  _torOpts << TorOpt("V3AuthVoteDelay", TorOpt::TimeInterval, "5 minutes");
  _torOpts << TorOpt("V3AuthDistDelay", TorOpt::TimeInterval, "5 minutes");
  _torOpts << TorOpt("V3AuthNIntervalsValid", TorOpt::Integer, "3");
  _torOpts << TorOpt("V3AuthUseLegacyKey", TorOpt::Boolean, "0");
  _torOpts << TorOpt("V3BandwidthsFile", TorOpt::Filename, "");
  _torOpts << TorOpt("VersioningAuthoritativeDirectory", TorOpt::Boolean, "0");
  _torOpts << TorOpt("VirtualAddrNetwork", TorOpt::String, "127.192.0.0/10");
  _torOpts << TorOpt("WarnPlaintextPorts", TorOpt::CommaList, "23,109,110,143");
  _torOpts << TorOpt("_UseFilteringSSLBufferevents", TorOpt::Boolean, "0");
  _torOpts << TorOpt("__ReloadTorrcOnSIGHUP", TorOpt::Boolean, "1");
  _torOpts << TorOpt("__AllDirActionsPrivate", TorOpt::Boolean, "0");
  _torOpts << TorOpt("__DisablePredictedCircuits", TorOpt::Boolean, "0");
  _torOpts << TorOpt("__LeaveStreamsUnattached", TorOpt::Boolean, "0");
  _torOpts << TorOpt("__HashedControlSessionPassword", TorOpt::LineList, "");
  _torOpts << TorOpt("__OwningControllerProcess", TorOpt::String, "");
  _torOpts << TorOpt("MinUptimeHidServDirectoryV2", TorOpt::TimeInterval, "25 hours");
  _torOpts << TorOpt("VoteOnHidServDirectoriesV2", TorOpt::Boolean, "1");
  _torOpts << TorOpt("_UsingTestNetworkDefaults", TorOpt::Boolean, "0");
 
  _regexp = createRegExp();
}

QRegExp
TorrcParser::createRegExp()
{
  QString reg = "";
  foreach(TorOpt opt, _torOpts) {
    reg += opt.name();
    reg += "|";
  }
  reg = reg.remove(reg.size() - 1, 1);

  return QRegExp(reg, Qt::CaseInsensitive);
}

TorOpt
TorrcParser::getTorOpt(const QString &name)
{
  foreach(const TorOpt &opt, _torOpts) {
    if(opt.name() == name)
      return opt;
  }

  return TorOpt::null();
}

QList<TorrcLine *>
TorrcParser::parse(const QString &path,
             QMap<QString, QPair<QString, TorOpt> > &map)
{
  QFile torrc(path);
  QFileInfo torrc_info;
  QList<TorrcLine *> lines;
  torrc_info.setFile(torrc);

  if(not torrc_info.isReadable()) {
    vWarn("Can't read torrc file, aborting parsing.");
    return lines;
  }
  
  if(not torrc.open(QIODevice::ReadOnly)) {
    vWarn("Can't open torrc file, aborting parsing.");
    return lines;
  }

  QString raw_contents(torrc.readAll());
  torrc.close();

  lines = toTorrcLines(raw_contents);
  
  QString key, value;
  int sp_pos = -1;

  foreach(TorrcLine *line, lines) {
    sp_pos = line->content().indexOf(" ");
    if(sp_pos == -1)
      continue;

    key = line->content().mid(0, sp_pos);
    value = "";
    if(line->isMultilined()) {
      QList<TorrcLine *> valueLines = findValueLines(line, lines);
      foreach(TorrcLine *valueLine, valueLines) {
        if(valueLine->isMultilined()) {
          value += valueLine->content().remove(valueLine->content().lastIndexOf("\\"), 1);
        } else
          // Last line, doesn't have \ at the end
          value += valueLine->content();
        // remove the contents, since they will live in the first line only
        valueLine->setContent("");
      }
      line->setContent(QString("%1 %2").arg(key).arg(value));
    } else {
      value = line->content().mid(sp_pos+1, line->content().size());
    }

    TorOpt opt(getTorOpt(key));
    opt.setLine(line);
    if(map.contains(key) and opt.isMultivalued())
      map.insertMulti(key, QPair<QString, TorOpt>(value, opt));
    else
      map.insert(key, QPair<QString, TorOpt>(value, opt));
  }
  
  return lines;
}

QString
TorrcParser::stripContents(QString contents)
{
  QString stripped_contents = "", current_line = "";
  int comment_pos = -1;

  foreach(QString line, contents.split("\n")) {
    comment_pos = line.indexOf("#");
    if(comment_pos == -1) {
      stripped_contents += line.trimmed();
      stripped_contents += "\n";
      continue;
    }
    
    current_line = line.mid(0, comment_pos).trimmed();
    if(current_line.size() == 0)
      continue;
    stripped_contents += current_line;
    stripped_contents += "\n";
  }

  return stripped_contents.replace("\t", " ");
}

QList<TorrcLine *>
TorrcParser::toTorrcLines(QString contents)
{
  QList<TorrcLine *> lines;
  QString stripped_contents = "", current_line = "";
  int comment_pos = -1;

  foreach(QString line, contents.split("\n")) {
    comment_pos = line.indexOf("#");
    if(comment_pos == -1) {
      lines << new TorrcLine(line.trimmed());
      continue;
    }
    
    current_line = line.mid(0, comment_pos).trimmed().replace("\t", " ");
    if(current_line.size() == 0) {
      // The line is just a comment
      lines << new TorrcLine("", line.trimmed());
      continue;
    }
    lines << new TorrcLine(current_line, line.mid(comment_pos, line.size()).trimmed());
  }

  return lines;
}
  
QList<TorrcLine *>
TorrcParser::findValueLines(TorrcLine *line, 
                            const QList<TorrcLine *> &lines) const
{
  QList<TorrcLine *> collected;

  int index = -1;
  index = lines.indexOf(line) + 1;
  if(index >= lines.size())
    return collected;

  for(int i = index; i<lines.size(); i++) {
    if(lines.at(i)->isMultilined())
      collected << lines.at(i);
    else {
      collected << lines.at(i);
      break;
    }
  }

  return collected;
}

bool
TorrcParser::isKeyword(const QString &contents, int pos)
{
  while(contents.at(pos) != '\n') {
    pos--;
    if(pos < 0 or contents.at(pos) == '\n')
      return true;
    if(contents.at(pos) != ' ')
      return false;
  }
  return true;
}

QDebug
operator<<(QDebug dbg, const TorOpt &opt) {
  dbg.nospace() << qPrintable(QString("TorOpt(%1)")
                              .arg(opt.name()));
  return dbg;
}

QDebug
operator<<(QDebug dbg, TorrcLine *line) {
  dbg.nospace() << qPrintable(QString("TorLine(%1, %2)")
                              .arg(line->content())
                              .arg(line->comment()));
  return dbg;
}
