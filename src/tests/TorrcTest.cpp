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
** \file TorrcTest.cpp
** \brief  test of torrc handling
*/

#include "TorrcTest.h"
#include "TorrcParser.h"
#include "Torrc.h"
#include "Vidalia.h"
#include "MainWindow.h"
#include <QTimer>
#include <QEventLoop>

/** Constructor. */
TorrcTestSuite::TorrcTestSuite(QObject *parent)
  :QObject(parent)
{
  
}

/** Destructor. */
TorrcTestSuite::~TorrcTestSuite()
{

}

/** Test TorrcLine class */
void TorrcTestSuite::testTorrcLine()
{
  const int count = 100;

  // test single lined values
  for (int i=0 ; i<count ; ++i) {
    const QString content = QString("test_content_%1").arg(i);
    const QString comment = QString("test_comment_%1").arg(i);
    TorrcLine tLine(content,comment);
    QCOMPARE(content, tLine.content());
    QCOMPARE(comment, tLine.comment());
    QVERIFY(tLine.isMultilined() == false);
    tLine.setComment(content);
    tLine.setContent(comment);
    QCOMPARE(comment, tLine.content());
    QCOMPARE(content, tLine.comment());
    QVERIFY(tLine.isMultilined() == false);
  }

  // test multilined TorrcLine
  for (int i=0 ; i<count ; ++i) {
    const QString content = QString("test\\multiline\\content\\%1\\").arg(i);
    const QString comment = QString("test\\multiline\\comment\\%1\\").arg(i);
    TorrcLine tLine(content,comment);
    QCOMPARE(content, tLine.content());
    QCOMPARE(comment, tLine.comment());
    QVERIFY(tLine.isMultilined() == true);
    tLine.setComment(content);
    tLine.setContent(comment);
    QCOMPARE(comment, tLine.content());
    QCOMPARE(content, tLine.comment());
    QVERIFY(tLine.isMultilined() == true);
  }
}

/** Test parser class */
void TorrcTestSuite::testTorrcParser()
{
  const QString testFilePath("vidalia_test_suite_torrc_file.txt");
  QFile file(testFilePath);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    QVERIFY(false && "Cannot open test torrc file!");
  const int optCount = 100;
  QList< QPair<QString,QString> > testData;
  // create test file
  QString content;
  for (int i=0 ; i<optCount ; ++i) {
    const QString optName = QString("option_%1 value_%1").arg(i);
    const QString optComt = QString("#comment_%1").arg(i);
    content += optName + optComt;
    testData.append( QPair<QString,QString>(optName,optComt) );
    if (i<optCount-1)
        content += "\n";
  }
  file.write(content.toAscii());
  file.close();

  TorrcParser parser;
  QMap<QString, QPair<QString, TorOpt> > map;
  QList<TorrcLine*> lines = parser.parse(testFilePath,map);
  QVERIFY(lines.isEmpty() == false);
  QCOMPARE(lines.count(), optCount);

  for (int i=0 ; i<optCount ; ++i) {
    TorrcLine * line = lines.at(i);
    QVERIFY(line != NULL);
    QCOMPARE(line->content(), testData[i].first);
    QCOMPARE(line->comment(), testData[i].second);
  }

  // verify loaded values (all should be null)
  QCOMPARE(map.count(), optCount);
  foreach (QString key, map.keys()) {
    QString value = map[key].first;
    TorOpt opt = map[key].second;
    QVERIFY(value.startsWith("value_"));
    QVERIFY(opt.isNull());
  }

  qDeleteAll(lines);
  QVERIFY( QFile::remove(testFilePath) );
}

QList<TorOpt> createTestOpts();
QString createTestValue(TorOpt::OptType);
QString defaultValue(const QString &);

/** Test main Torrc class */
void TorrcTestSuite::testTorrc()
{
  const QString testFilePath("vidalia_test_suite_torrc_file.txt");
  QFile file(testFilePath);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    QVERIFY(false && "Cannot open test torrc file!");
  QList<TorOpt> opts = createTestOpts();
  QList< QPair<QString,QString> > testData;
  foreach (const TorOpt &opt, opts) {
    testData.append( QPair<QString,QString>(opt.name(),createTestValue(opt.type())) );
  }
  // create test file with real option names and values
  QString content;
  for (int i=0 ; i<testData.count() ; ++i) {
    const QString optName = testData[i].first + " " + testData[i].second;
    const QString optComt = QString("#comment_%1").arg(i);
    content += optName + optComt;
    if (i<testData.count()-1)
      content += "\n";
  }
  file.write(content.toAscii());
  file.close();

  TorrcParser parser;
  QMap<QString, QPair<QString, TorOpt> > map;
  QList<TorrcLine*> lines = parser.parse(testFilePath,map);
  QVERIFY(lines.isEmpty() == false);
  QCOMPARE(lines.count(), testData.count());

  for (int i=0 ; i<testData.count() ; ++i) {
    TorrcLine * line = lines.at(i);
    QVERIFY(line != NULL);
    QCOMPARE(line->content(), QString(testData[i].first + " " + testData[i].second).trimmed());
    QCOMPARE(line->comment(), QString("#comment_%1").arg(i));
  }

  // verify loaded values
  foreach (QString key, map.keys()) {
    QString value = map[key].first;
    TorOpt opt = map[key].second;
    QCOMPARE(value, createTestValue(opt.type()));
    QVERIFY(opt.isNull() == false);
    QVERIFY(testData.contains( QPair<QString,QString>(key,value) ));
  }

  qDeleteAll(lines);

  // test torrc object
  Torrc torrc(testFilePath);
  QCOMPARE(torrc.getTorrcPath(), testFilePath);
  for (int i=0 ; i<testData.count() ; ++i) {
    // test the data in torrc file
    QVERIFY(torrc.isValid(testData[i].first, testData[i].second));
    QStringList values = torrc.value(testData[i].first);
    if (testData[i].second.isEmpty()==false) {
      QVERIFY(values.contains(testData[i].second));
    }
    // test if clearing a value works ok
    torrc.clear(QStringList() << testData[i].first);
    values = torrc.value(testData[i].first);
    if (testData[i].second.isEmpty()==false) {
      QVERIFY(values.contains(testData[i].second) == false);
    }
  }
  QVERIFY(torrc.apply(Vidalia::torControl()));
  // after applying changes, we test the file content
  QVERIFY(file.open(QIODevice::ReadOnly | QIODevice::Text));
  content = file.readAll();
  file.close();
  for (int i=0 ; i<testData.count() ; ++i) {
    QVERIFY(content.contains(testData[i].first) == false);
  }
  // test adding values to empty file
  QVERIFY(QFile::remove(testFilePath));
  QVERIFY(file.open(QIODevice::WriteOnly | QIODevice::Text));
  file.write("");
  file.close();
  torrc.load(testFilePath);
  int lastKeyPos = -1; // value for checking if <key, value> pairs are present in the file
  for (int i=0 ; i<testData.count() ; ++i) {
    torrc.setValue(testData[i].first,testData[i].second);
    QVERIFY(torrc.apply(Vidalia::torControl()));
    QVERIFY(file.open(QIODevice::ReadOnly | QIODevice::Text));
    content = file.readAll();
    file.close();
    // if value is default for given key, file should not contain the entry
    if (testData[i].second == defaultValue(testData[i].first)) {
      QVERIFY(content.contains(testData[i].first)==false);
    } else {
      // check if key is in the file, and the value follows
      QVERIFY(content.contains(testData[i].first));
      lastKeyPos = content.indexOf(testData[i].first,lastKeyPos+1);
      int nlPos = content.indexOf("\n",lastKeyPos);
      if (nlPos == -1)
        nlPos = content.length();
      const int valPos = content.indexOf(testData[i].second,lastKeyPos);
      QVERIFY(valPos != -1);
      QVERIFY(valPos < nlPos);
    }
  }
  QVERIFY(QFile::remove(testFilePath));
}

inline bool
compareValues(const QVariant& var, const QString& value){
  bool ok = false;
  const double r = value.toDouble(&ok);
  if (ok)
    return qFuzzyCompare(var.toDouble(),r);
  else
    return var.toString() == value;
}

void TorrcTestSuite::testRunningTor()
{
  MainWindow * mw = new MainWindow;
  mw->show();
  TorControl * torControl = Vidalia::torControl();
  QVERIFY(torControl);

  // start tor, make sure we wont wait forever
  QEventLoop loop;
  QTimer timeoutCheck;
  timeoutCheck.setSingleShot(true);
  timeoutCheck.setInterval(120*1000);
  connect(&timeoutCheck, SIGNAL(timeout()), &loop, SLOT(quit()));
  connect(torControl, SIGNAL(started()), &loop, SLOT(quit()));
  QTimer::singleShot(0,mw,SLOT(start()));
  timeoutCheck.start();
  loop.exec();
  qApp->processEvents();

  if (timeoutCheck.isActive()) {
    vWarn("Tor process started.");
    QVERIFY(torControl->isConnected());
    QVERIFY(torControl->isRunning());

    // create test torrc file
    const QString testFilePath("vidalia_test_suite_torrc_file.txt");
    QFile file(testFilePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
      QVERIFY(false && "Cannot open test torrc file!");

    QHash<QString,QString> testData;
    testData["AccountingMax"] = "0";
    testData["AllowDotExit"] =  "0";
    testData["AllowInvalidNodes"] =  "middle,rendezvous";
    testData["AllowNonRFC953Hostnames"] = "0";
    testData["AllowSingleHopCircuits"] = "0";
    testData["BridgeRecordUsageByCountry"] = "0";
    testData["CircuitIdleTimeout"] = "3600";
    testData["CircuitPriorityHalflife"] = "-100.0";
    testData["ControlPort"] = "9051";
    testData["CookieAuthentication"] = "0";
    testData["DataDirectory"] = "C:\\DifferentDataDirectory";
    testData["PortForwarding"] = "0";
    testData["ProtocolWarnings"] = "0";
    testData["User"] = "userid";
    // data directory should remain unchanged
    const QString dataDir = torControl->getConf("DataDirectory").toString();

    QString content;
    foreach (QString key, testData) {
      content += key + " " + testData[key] + "#comment\n";
    }
    file.write(content.toAscii());
    file.close();
    Torrc torrc(testFilePath);
    QVERIFY( torrc.apply(torControl) );

    // check values in running tor instance
    foreach (QString key, testData) {
      const QVariant val = torControl->getConf(key);
      if (key == "DataDirectory"){
        QCOMPARE(dataDir,val.toString());
      } else
        QVERIFY(compareValues(val,testData[key]));
    }
    // create new test data, different from current tor values
    testData.remove("DataDirectory");
    testData["AccountingMax"] = "1";
    testData["AllowDotExit"] =  "1";
    testData["AllowInvalidNodes"] =  "middle";
    testData["AllowNonRFC953Hostnames"] = "1";
    testData["AllowSingleHopCircuits"] = "1";
    testData["BridgeRecordUsageByCountry"] = "1";
    testData["CircuitIdleTimeout"] = "900";
    testData["CircuitPriorityHalflife"] = "-150.0";
    testData["ControlPort"] = "8974";
    testData["CookieAuthentication"] = "1";
    testData["PortForwarding"] = "1";
    testData["ProtocolWarnings"] = "1";
    testData["User"] = "u2id";

    // check if values are applied ok
    QVERIFY( torrc.apply(torControl) );
    foreach (QString key, testData) {
      const QVariant val = torControl->getConf(key);
      QVERIFY(compareValues(val,testData[key]));
    }
    QVERIFY(QFile::remove(testFilePath));
  } else {
    QVERIFY(false && "Cannot start the tor process!");
  }
  mw->deleteLater();
}

QList<TorOpt>
createTestOpts()
{
  QList<TorOpt> opt;
  opt << TorOpt("AccountingMax", TorOpt::DataSize, "0");
  opt << TorOpt("AccountingStart", TorOpt::String, "");
  opt << TorOpt("Address", TorOpt::String, "");
  opt << TorOpt("AllowDotExit", TorOpt::Boolean, "0");
  opt << TorOpt("AllowInvalidNodes", TorOpt::CommaList, "middle,rendezvous");
  opt << TorOpt("AllowNonRFC953Hostnames", TorOpt::Boolean, "0");
  opt << TorOpt("AllowSingleHopCircuits", TorOpt::Boolean, "0");
  opt << TorOpt("AllowSingleHopExits", TorOpt::Boolean, "0");
  opt << TorOpt("AlternateBridgeAuthority", TorOpt::LineList, "");
  opt << TorOpt("AlternateDirAuthority", TorOpt::LineList, "");
  opt << TorOpt("AlternateHSAuthority", TorOpt::LineList, "");
  opt << TorOpt("AssumeReachable", TorOpt::Boolean, "0");
  opt << TorOpt("AuthDirBadDir", TorOpt::LineList, "");
  opt << TorOpt("AuthDirBadExit", TorOpt::LineList, "");
  opt << TorOpt("AuthDirInvalid", TorOpt::LineList, "");
  opt << TorOpt("AuthDirReject", TorOpt::LineList, "");
  opt << TorOpt("AuthDirRejectUnlisted", TorOpt::Boolean, "0");
  opt << TorOpt("AuthDirListBadDirs", TorOpt::Boolean, "0");
  opt << TorOpt("AuthDirListBadExits", TorOpt::Boolean, "0");
  opt << TorOpt("AuthDirMaxServersPerAddr", TorOpt::Integer, "2");
  opt << TorOpt("AuthDirMaxServersPerAuthAddr", TorOpt::Integer, "5");
  opt << TorOpt("AuthoritativeDirectory", TorOpt::Boolean, "0");
  opt << TorOpt("AutomapHostsOnResolve", TorOpt::Boolean, "0");
  opt << TorOpt("AutomapHostsSuffixes", TorOpt::CommaList, ".onion,.exit");
  opt << TorOpt("AvoidDiskWrites", TorOpt::Boolean, "0");
  opt << TorOpt("BandwidthBurst", TorOpt::DataSize, "10485760");
  opt << TorOpt("BandwidthRate", TorOpt::DataSize, "5242880");
  opt << TorOpt("BridgeAuthoritativeDir", TorOpt::Boolean, "0");
  opt << TorOpt("Bridge", TorOpt::LineList, "");
  opt << TorOpt("BridgePassword", TorOpt::String, "");
  opt << TorOpt("BridgeRecordUsageByCountry", TorOpt::Boolean, "1");
  opt << TorOpt("BridgeRelay", TorOpt::Boolean, "0");
  opt << TorOpt("CellStatistics", TorOpt::Boolean, "0");
  opt << TorOpt("LearnCircuitBuildTimeout", TorOpt::Boolean, "1");
  opt << TorOpt("CircuitBuildTimeout", TorOpt::TimeInterval, "0");
  opt << TorOpt("CircuitIdleTimeout", TorOpt::TimeInterval, "3600");
  opt << TorOpt("CircuitStreamTimeout", TorOpt::TimeInterval, "0");
  opt << TorOpt("CircuitPriorityHalflife", TorOpt::Float, "-100.0");
  opt << TorOpt("ClientDNSRejectInternalAddresses", TorOpt::Boolean, "1");
  opt << TorOpt("ClientOnly", TorOpt::Boolean, "0");
  opt << TorOpt("ClientRejectInternalAddresses", TorOpt::Boolean, "1");
  opt << TorOpt("ClientTransportPlugin", TorOpt::LineList, "");
  opt << TorOpt("ConsensusParams", TorOpt::String, "");
  opt << TorOpt("ConnLimit", TorOpt::Integer, "1000");
  opt << TorOpt("ConnDirectionStatistics", TorOpt::Boolean, "0");
  opt << TorOpt("ConstrainedSockets", TorOpt::Boolean, "0");
  opt << TorOpt("ConstrainedSockSize", TorOpt::DataSize, "8192");
  opt << TorOpt("ContactInfo", TorOpt::String, "");
  opt << TorOpt("ControlListenAddress", TorOpt::LineList, "");
  opt << TorOpt("ControlPort", TorOpt::Port, "9051");
  opt << TorOpt("ControlPortFileGroupReadable", TorOpt::Boolean, "0");
  opt << TorOpt("ControlPortWriteToFile", TorOpt::Filename, "");
  opt << TorOpt("ControlSocket", TorOpt::LineList, "");
  opt << TorOpt("ControlSocketsGroupWritable", TorOpt::Boolean, "0");
  opt << TorOpt("CookieAuthentication", TorOpt::Boolean, "0");
  opt << TorOpt("CookieAuthFileGroupReadable", TorOpt::Boolean, "0");
  opt << TorOpt("CookieAuthFile", TorOpt::String, "");
  opt << TorOpt("CountPrivateBandwidth", TorOpt::Boolean, "0");
  opt << TorOpt("DataDirectory", TorOpt::Filename, "");
  opt << TorOpt("DirAllowPrivateAddresses", TorOpt::Boolean, "0");
  opt << TorOpt("TestingAuthDirTimeToLearnReachability", TorOpt::TimeInterval, "1800");
  opt << TorOpt("DirListenAddress", TorOpt::LineList, "");
  opt << TorOpt("DirPolicy", TorOpt::LineList, "");
  opt << TorOpt("DirPort", TorOpt::Port, "0");
  opt << TorOpt("DirPortFrontPage", TorOpt::Filename, "");
  opt << TorOpt("DirServer", TorOpt::LineList, "");
  opt << TorOpt("DisableAllSwap", TorOpt::Boolean, "0");
  opt << TorOpt("DisableIOCP", TorOpt::Boolean, "1");
  opt << TorOpt("DisableNetwork", TorOpt::Boolean, "0");
  opt << TorOpt("DNSPort", TorOpt::LineList, "");
  opt << TorOpt("DNSListenAddress", TorOpt::LineList, "");
  opt << TorOpt("DownloadExtraInfo", TorOpt::Boolean, "0");
  opt << TorOpt("EnforceDistinctSubnets", TorOpt::Boolean, "1");
  opt << TorOpt("EntryNodes", TorOpt::RouterList, "");
  opt << TorOpt("EntryStatistics", TorOpt::Boolean, "0");
  opt << TorOpt("TestingEstimatedDescriptorPropagationTime", TorOpt::TimeInterval, "600");
  opt << TorOpt("ExcludeNodes", TorOpt::RouterList, "");
  opt << TorOpt("ExcludeExitNodes", TorOpt::RouterList, "");
  opt << TorOpt("ExcludeSingleHopRelays", TorOpt::Boolean, "1");
  opt << TorOpt("ExitNodes", TorOpt::RouterList, "");
  opt << TorOpt("ExitPolicy", TorOpt::LineList, "");
  opt << TorOpt("ExitPolicyRejectPrivate", TorOpt::Boolean, "1");
  opt << TorOpt("ExitPortStatistics", TorOpt::Boolean, "0");
  opt << TorOpt("ExtraInfoStatistics", TorOpt::Boolean, "1");
  opt << TorOpt("FallbackNetworkstatusFile", TorOpt::Filename, "/usr/local/share/tor/fallback-consensus");
  opt << TorOpt("FascistFirewall", TorOpt::Boolean, "0");
  opt << TorOpt("FirewallPorts", TorOpt::CommaList, "");
  opt << TorOpt("FastFirstHopPK", TorOpt::Boolean, "1");
  opt << TorOpt("FetchDirInfoEarly", TorOpt::Boolean, "0");
  opt << TorOpt("FetchDirInfoExtraEarly", TorOpt::Boolean, "0");
  opt << TorOpt("FetchServerDescriptors", TorOpt::Boolean, "1");
  opt << TorOpt("FetchHidServDescriptors", TorOpt::Boolean, "1");
  opt << TorOpt("FetchUselessDescriptors", TorOpt::Boolean, "0");
  opt << TorOpt("FetchV2Networkstatus", TorOpt::Boolean, "0");
  opt << TorOpt("GeoIPFile", TorOpt::Filename, "/usr/local/share/tor/geoip");
  opt << TorOpt("GiveGuardFlagTo_CVE_2011_2768_VulnerableRelays", TorOpt::Boolean, "0");
  opt << TorOpt("HardwareAccel", TorOpt::Boolean, "0");
  opt << TorOpt("HeartbeatPeriod", TorOpt::TimeInterval, "21300");
  opt << TorOpt("AccelName", TorOpt::String, "");
  opt << TorOpt("AccelDir", TorOpt::Filename, "");
  opt << TorOpt("HashedControlPassword", TorOpt::String, "");
  opt << TorOpt("HidServDirectoryV2", TorOpt::Boolean, "1");
  opt << TorOpt("HiddenServiceDir", TorOpt::Dependant, "");
  opt << TorOpt("HiddenServiceOptions", TorOpt::Virtual, "");
  opt << TorOpt("HiddenServicePort", TorOpt::Dependant, "");
  opt << TorOpt("HiddenServiceVersion", TorOpt::Dependant, "");
  opt << TorOpt("HiddenServiceAuthorizeClient", TorOpt::Dependant, "");
  opt << TorOpt("HidServAuth", TorOpt::LineList, "");
  opt << TorOpt("HSAuthoritativeDir", TorOpt::Boolean, "0");
  opt << TorOpt("HTTPProxy", TorOpt::String, "");
  opt << TorOpt("HTTPProxyAuthenticator", TorOpt::String, "");
  opt << TorOpt("HTTPSProxy", TorOpt::String, "");
  opt << TorOpt("HTTPSProxyAuthenticator", TorOpt::String, "");
  opt << TorOpt("ServerTransportPlugin", TorOpt::LineList, "");
  opt << TorOpt("Socks4Proxy", TorOpt::String, "");
  opt << TorOpt("Socks5Proxy", TorOpt::String, "");
  opt << TorOpt("Socks5ProxyUsername", TorOpt::String, "");
  opt << TorOpt("Socks5ProxyPassword", TorOpt::String, "");
  opt << TorOpt("KeepalivePeriod", TorOpt::TimeInterval, "300");
  opt << TorOpt("Log", TorOpt::LineList, "");
  opt << TorOpt("LogMessageDomains", TorOpt::Boolean, "0");
  opt << TorOpt("LogTimeGranularity", TorOpt::TimeMsecInterval, "1000");
  opt << TorOpt("LongLivedPorts", TorOpt::CommaList, "21,22,706,1863,5050,5190,5222,5223,6523,6667,6697,8300");
  opt << TorOpt("MapAddress", TorOpt::LineList, "");
  opt << TorOpt("MaxAdvertisedBandwidth", TorOpt::DataSize, "1073741824");
  opt << TorOpt("MaxCircuitDirtiness", TorOpt::TimeInterval, "600");
  opt << TorOpt("MaxClientCircuitsPending", TorOpt::Integer, "32");
  opt << TorOpt("MaxOnionsPending", TorOpt::Integer, "100");
  opt << TorOpt("MyFamily", TorOpt::String, "");
  opt << TorOpt("NewCircuitPeriod", TorOpt::TimeInterval, "30");
  opt << TorOpt("NamingAuthoritativeDirectory", TorOpt::Boolean, "0");
  opt << TorOpt("NATDListenAddress", TorOpt::LineList, "");
  opt << TorOpt("NATDPort", TorOpt::LineList, "0");
  opt << TorOpt("Nickname", TorOpt::String, "");
  opt << TorOpt("WarnUnsafeSocks", TorOpt::Boolean, "1");
  opt << TorOpt("NodeFamily", TorOpt::LineList, "");
  opt << TorOpt("NumCPUs", TorOpt::Integer, "0");
  opt << TorOpt("NumEntryGuards", TorOpt::Integer, "3");
  opt << TorOpt("ORListenAddress", TorOpt::LineList, "");
  opt << TorOpt("ORPort", TorOpt::Port, "0");
  opt << TorOpt("OutboundBindAddress", TorOpt::String, "");
  opt << TorOpt("PerConnBWBurst", TorOpt::DataSize, "0");
  opt << TorOpt("PerConnBWRate", TorOpt::DataSize, "0");
  opt << TorOpt("PidFile", TorOpt::String, "");
  opt << TorOpt("TestingTorNetwork", TorOpt::Boolean, "0");
  opt << TorOpt("OptimisticData", TorOpt::BooleanAuto, "auto");
  opt << TorOpt("PortForwarding", TorOpt::Boolean, "0");
  opt << TorOpt("PortForwardingHelper", TorOpt::Filename, "tor-fw-helper");
  opt << TorOpt("PreferTunneledDirConns", TorOpt::Boolean, "1");
  opt << TorOpt("ProtocolWarnings", TorOpt::Boolean, "0");
  opt << TorOpt("PublishServerDescriptor", TorOpt::CommaList, "1");
  opt << TorOpt("PublishHidServDescriptors", TorOpt::Boolean, "1");
  opt << TorOpt("ReachableAddresses", TorOpt::LineList, "");
  opt << TorOpt("ReachableDirAddresses", TorOpt::LineList, "");
  opt << TorOpt("ReachableORAddresses", TorOpt::LineList, "");
  opt << TorOpt("RecommendedVersions", TorOpt::LineList, "");
  opt << TorOpt("RecommendedClientVersions", TorOpt::LineList, "");
  opt << TorOpt("RecommendedServerVersions", TorOpt::LineList, "");
  opt << TorOpt("RefuseUnknownExits", TorOpt::BooleanAuto, "auto");
  opt << TorOpt("RejectPlaintextPorts", TorOpt::CommaList, "");
  opt << TorOpt("RelayBandwidthBurst", TorOpt::DataSize, "0");
  opt << TorOpt("RelayBandwidthRate", TorOpt::DataSize, "0");
  opt << TorOpt("RendPostPeriod", TorOpt::TimeInterval, "3600");
  opt << TorOpt("RephistTrackTime", TorOpt::TimeInterval, "36000");
  opt << TorOpt("RunAsDaemon", TorOpt::Boolean, "0");
  opt << TorOpt("SafeLogging", TorOpt::String, "1");
  opt << TorOpt("SafeSocks", TorOpt::Boolean, "0");
  opt << TorOpt("ServerDNSAllowBrokenConfig", TorOpt::Boolean, "1");
  opt << TorOpt("ServerDNSAllowNonRFC953Hostnames", TorOpt::Boolean, "0");
  opt << TorOpt("ServerDNSDetectHijacking", TorOpt::Boolean, "1");
  opt << TorOpt("ServerDNSRandomizeCase", TorOpt::Boolean, "1");
  opt << TorOpt("ServerDNSResolvConfFile", TorOpt::String, "");
  opt << TorOpt("ServerDNSSearchDomains", TorOpt::Boolean, "0");
  opt << TorOpt("ServerDNSTestAddresses", TorOpt::CommaList, "www.google.com,www.mit.edu,www.yahoo.com,www.slashdot.org");
  opt << TorOpt("ShutdownWaitLength", TorOpt::TimeInterval, "30");
  opt << TorOpt("SocksListenAddress", TorOpt::LineList, "");
  opt << TorOpt("SocksPolicy", TorOpt::LineList, "");
  opt << TorOpt("SocksPort", TorOpt::LineList, "9051");
  opt << TorOpt("SocksTimeout", TorOpt::TimeInterval, "200");
  opt << TorOpt("StrictNodes", TorOpt::Boolean, "0");
  opt << TorOpt("TestSocks", TorOpt::Boolean, "0");
  opt << TorOpt("TokenBucketRefillInterval", TorOpt::TimeMsecInterval, "100");
  opt << TorOpt("TrackHostExits", TorOpt::CommaList, "");
  opt << TorOpt("TrackHostExitsExpire", TorOpt::TimeInterval, "1800");
  opt << TorOpt("TransListenAddress", TorOpt::LineList, "");
  opt << TorOpt("TransPort", TorOpt::LineList, "0");
  opt << TorOpt("TunnelDirConns", TorOpt::Boolean, "1");
  opt << TorOpt("UpdateBridgesFromAuthority", TorOpt::Boolean, "0");
  opt << TorOpt("UseBridges", TorOpt::Boolean, "0");
  opt << TorOpt("UseEntryGuards", TorOpt::Boolean, "1");
  opt << TorOpt("UseMicrodescriptors", TorOpt::BooleanAuto, "auto");
  opt << TorOpt("User", TorOpt::String, "");
  opt << TorOpt("V1AuthoritativeDirectory", TorOpt::Boolean, "0");
  opt << TorOpt("V2AuthoritativeDirectory", TorOpt::Boolean, "0");
  opt << TorOpt("V3AuthoritativeDirectory", TorOpt::Boolean, "0");
  opt << TorOpt("TestingV3AuthInitialVotingInterval", TorOpt::TimeInterval, "1800");
  opt << TorOpt("TestingV3AuthInitialVoteDelay", TorOpt::TimeInterval, "300");
  opt << TorOpt("TestingV3AuthInitialDistDelay", TorOpt::TimeInterval, "300");
  opt << TorOpt("V3AuthVotingInterval", TorOpt::TimeInterval, "3600");
  opt << TorOpt("V3AuthVoteDelay", TorOpt::TimeInterval, "300");
  opt << TorOpt("V3AuthDistDelay", TorOpt::TimeInterval, "300");
  opt << TorOpt("V3AuthNIntervalsValid", TorOpt::Integer, "3");
  opt << TorOpt("V3AuthUseLegacyKey", TorOpt::Boolean, "0");
  opt << TorOpt("V3BandwidthsFile", TorOpt::Filename, "");
  opt << TorOpt("VersioningAuthoritativeDirectory", TorOpt::Boolean, "0");
  opt << TorOpt("VirtualAddrNetwork", TorOpt::String, "127.192.0.0/10");
  opt << TorOpt("WarnPlaintextPorts", TorOpt::CommaList, "23,109,110,143");
  opt << TorOpt("_UseFilteringSSLBufferevents", TorOpt::Boolean, "0");
  opt << TorOpt("__ReloadTorrcOnSIGHUP", TorOpt::Boolean, "1");
  opt << TorOpt("__AllDirActionsPrivate", TorOpt::Boolean, "0");
  opt << TorOpt("__DisablePredictedCircuits", TorOpt::Boolean, "0");
  opt << TorOpt("__LeaveStreamsUnattached", TorOpt::Boolean, "0");
  opt << TorOpt("__HashedControlSessionPassword", TorOpt::LineList, "");
  opt << TorOpt("__OwningControllerProcess", TorOpt::String, "");
  opt << TorOpt("MinUptimeHidServDirectoryV2", TorOpt::TimeInterval, "1500");
  opt << TorOpt("VoteOnHidServDirectoriesV2", TorOpt::Boolean, "1");
  opt << TorOpt("_UsingTestNetworkDefaults", TorOpt::Boolean, "0");
  return opt;
}

QString
createTestValue(TorOpt::OptType type)
{
  switch (type) {
  case TorOpt::TimeInterval: return "32684";
  case TorOpt::TimeMsecInterval: return "5000";
  case TorOpt::String: return "test_string";
  case TorOpt::RouterList: return "";
  case TorOpt::LineList: return "123 456 789";
  case TorOpt::Float: return "3.14";
  case TorOpt::Dependant: return "dep";
  case TorOpt::Virtual: return "virt";
  case TorOpt::Filename: return "/usr/share/test/test.txt";
  case TorOpt::CommaList: return "12,43,521,123";
  case TorOpt::Boolean: return "1";
  case TorOpt::BooleanAuto: return "1";
  case TorOpt::Integer: return "7";
  case TorOpt::DataSize: return "128";
  case TorOpt::Port: return "9053";
  case TorOpt::NIL: return "";
  }
}

QString
defaultValue(const QString& optName)
{
  static QList<TorOpt> opts = createTestOpts();
  foreach (const TorOpt& opt, opts) {
    if (opt.name() == optName)
      return opt.defaultValue();
  }
  return QString();
}
