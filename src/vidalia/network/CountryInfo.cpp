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
** \file CountryInfo.cpp
** \brief Provides a method to look up a localized country name given its
** ISO 3166 2-letter country code.
*/

#include "CountryInfo.h"
#include "Vidalia.h"

#include <QMap>
#include <QHash>
#include <QFile>
#include <QStringList>

#define COUNTRY_LOCATION_FILE ":/geoip/country-coordinates.csv"


CountryInfo::CountryInfo(QObject *parent)
  : QObject(parent)
{
}

QString
CountryInfo::countryName(const QString &countryCode)
{
  static QMap<QString,QString> db;
  if (db.isEmpty()) {
    db.insert("af", QT_TR_NOOP("Afghanistan"));
    db.insert("al", QT_TR_NOOP("Albania "));
    db.insert("dz", QT_TR_NOOP("Algeria "));
    db.insert("ad", QT_TR_NOOP("Andorra"));
    db.insert("ao", QT_TR_NOOP("Angola"));
    db.insert("ag", QT_TR_NOOP("Antigua & Barbuda"));
    db.insert("ar", QT_TR_NOOP("Argentina"));
    db.insert("am", QT_TR_NOOP("Armenia"));
    db.insert("au", QT_TR_NOOP("Australia"));
    db.insert("at", QT_TR_NOOP("Austria "));
    db.insert("az", QT_TR_NOOP("Azerbaijan"));
    db.insert("bs", QT_TR_NOOP("Bahamas"));
    db.insert("bh", QT_TR_NOOP("Bahrain "));
    db.insert("bd", QT_TR_NOOP("Bangladesh"));
    db.insert("bb", QT_TR_NOOP("Barbados"));
    db.insert("by", QT_TR_NOOP("Belarus"));
    db.insert("be", QT_TR_NOOP("Belgium"));
    db.insert("bz", QT_TR_NOOP("Belize"));
    db.insert("bj", QT_TR_NOOP("Benin "));
    db.insert("bt", QT_TR_NOOP("Bhutan"));
    db.insert("bo", QT_TR_NOOP("Bolivia"));
    db.insert("ba", QT_TR_NOOP("Bosnia & Herzegovina"));
    db.insert("bw", QT_TR_NOOP("Botswana"));
    db.insert("br", QT_TR_NOOP("Brazil"));
    db.insert("bn", QT_TR_NOOP("Brunei Darussalam"));
    db.insert("bg", QT_TR_NOOP("Bulgaria"));
    db.insert("bf", QT_TR_NOOP("Burkina Faso"));
    db.insert("bi", QT_TR_NOOP("Burundi"));
    db.insert("kh", QT_TR_NOOP("Cambodia"));
    db.insert("cm", QT_TR_NOOP("Cameroon"));
    db.insert("ca", QT_TR_NOOP("Canada"));
    db.insert("cv", QT_TR_NOOP("Cape Verde"));
    db.insert("cf", QT_TR_NOOP("Central African Republic"));
    db.insert("td", QT_TR_NOOP("Chad"));
    db.insert("cl", QT_TR_NOOP("Chile"));
    db.insert("cn", QT_TR_NOOP("China"));
    db.insert("co", QT_TR_NOOP("Colombia"));
    db.insert("km", QT_TR_NOOP("Comoros"));
    db.insert("cd", QT_TR_NOOP("Congo, The Democratic Republic of the"));
    db.insert("cg", QT_TR_NOOP("Congo"));
    db.insert("cr", QT_TR_NOOP("Costa Rica"));
    db.insert("ci", QT_TR_NOOP("Cote d’Ivoire"));
    db.insert("hr", QT_TR_NOOP("Croatia"));
    db.insert("cu", QT_TR_NOOP("Cuba"));
    db.insert("cy", QT_TR_NOOP("Cyprus"));
    db.insert("cz", QT_TR_NOOP("Czech Republic"));
    db.insert("dk", QT_TR_NOOP("Denmark"));
    db.insert("dj", QT_TR_NOOP("Djibouti"));
    db.insert("dm", QT_TR_NOOP("Dominica"));
    db.insert("do", QT_TR_NOOP("Dominican Republic"));
    db.insert("ec", QT_TR_NOOP("Ecuador"));
    db.insert("eg", QT_TR_NOOP("Egypt"));
    db.insert("sv", QT_TR_NOOP("El Salvador"));
    db.insert("gq", QT_TR_NOOP("Equatorial Guinea"));
    db.insert("er", QT_TR_NOOP("Eritrea"));
    db.insert("ee", QT_TR_NOOP("Estonia"));
    db.insert("et", QT_TR_NOOP("Ethiopia "));
    db.insert("fj", QT_TR_NOOP("Fiji "));
    db.insert("fi", QT_TR_NOOP("Finland "));
    db.insert("fr", QT_TR_NOOP("France"));
    db.insert("ga", QT_TR_NOOP("Gabon"));
    db.insert("gm", QT_TR_NOOP("Gambia"));
    db.insert("ge", QT_TR_NOOP("Georgia"));
    db.insert("de", QT_TR_NOOP("Germany"));
    db.insert("gh", QT_TR_NOOP("Ghana"));
    db.insert("gr", QT_TR_NOOP("Greece "));
    db.insert("gd", QT_TR_NOOP("Grenada"));
    db.insert("gt", QT_TR_NOOP("Guatemala"));
    db.insert("gu", QT_TR_NOOP("Guam "));
    db.insert("gn", QT_TR_NOOP("Guinea"));
    db.insert("gw", QT_TR_NOOP("Guinea-Bissau"));
    db.insert("gy", QT_TR_NOOP("Guyana"));
    db.insert("hk", QT_TR_NOOP("Hong Kong"));
    db.insert("ht", QT_TR_NOOP("Haiti"));
    db.insert("hn", QT_TR_NOOP("Honduras"));
    db.insert("hu", QT_TR_NOOP("Hungary "));
    db.insert("is", QT_TR_NOOP("Iceland "));
    db.insert("in", QT_TR_NOOP("India "));
    db.insert("id", QT_TR_NOOP("Indonesia "));
    db.insert("ir", QT_TR_NOOP("Iran "));
    db.insert("iq", QT_TR_NOOP("Iraq "));
    db.insert("ie", QT_TR_NOOP("Ireland "));
    db.insert("il", QT_TR_NOOP("Israel"));
    db.insert("it", QT_TR_NOOP("Italy"));
    db.insert("jm", QT_TR_NOOP("Jamaica"));
    db.insert("jp", QT_TR_NOOP("Japan"));
    db.insert("jo", QT_TR_NOOP("Jordan"));
    db.insert("kz", QT_TR_NOOP("Kazakhstan"));
    db.insert("ke", QT_TR_NOOP("Kenya"));
    db.insert("ki", QT_TR_NOOP("Kiribati"));
    db.insert("kp", QT_TR_NOOP("Korea, North "));
    db.insert("kr", QT_TR_NOOP("Korea, South "));
    db.insert("kw", QT_TR_NOOP("Kuwait"));
    db.insert("kg", QT_TR_NOOP("Kyrgyzstan"));
    db.insert("la", QT_TR_NOOP("Laos"));
    db.insert("lv", QT_TR_NOOP("Latvia"));
    db.insert("lb", QT_TR_NOOP("Lebanon"));
    db.insert("ls", QT_TR_NOOP("Lesotho"));
    db.insert("lr", QT_TR_NOOP("Liberia"));
    db.insert("ly", QT_TR_NOOP("Libya "));
    db.insert("li", QT_TR_NOOP("Liechtenstein"));
    db.insert("lt", QT_TR_NOOP("Lithuania"));
    db.insert("lu", QT_TR_NOOP("Luxembourg"));
    db.insert("mk", QT_TR_NOOP("Macedonia"));
    db.insert("mg", QT_TR_NOOP("Madagascar"));
    db.insert("mw", QT_TR_NOOP("Malawi"));
    db.insert("my", QT_TR_NOOP("Malaysia"));
    db.insert("mv", QT_TR_NOOP("Maldives "));
    db.insert("ml", QT_TR_NOOP("Mali"));
    db.insert("mt", QT_TR_NOOP("Malta"));
    db.insert("mh", QT_TR_NOOP("Marshall Islands"));
    db.insert("mr", QT_TR_NOOP("Mauritania"));
    db.insert("mu", QT_TR_NOOP("Mauritius"));
    db.insert("mx", QT_TR_NOOP("Mexico "));
    db.insert("fm", QT_TR_NOOP("Micronesia"));
    db.insert("md", QT_TR_NOOP("Moldova"));
    db.insert("mc", QT_TR_NOOP("Monaco"));
    db.insert("mn", QT_TR_NOOP("Mongolia"));
    db.insert("me", QT_TR_NOOP("Montenegro"));
    db.insert("ma", QT_TR_NOOP("Morocco"));
    db.insert("mz", QT_TR_NOOP("Mozambique"));
    db.insert("mm", QT_TR_NOOP("Myanmar "));
    db.insert("na", QT_TR_NOOP("Namibia"));
    db.insert("nr", QT_TR_NOOP("Nauru"));
    db.insert("np", QT_TR_NOOP("Nepal"));
    db.insert("nl", QT_TR_NOOP("Netherlands"));
    db.insert("nz", QT_TR_NOOP("New Zealand"));
    db.insert("ni", QT_TR_NOOP("Nicaragua"));
    db.insert("ne", QT_TR_NOOP("Niger"));
    db.insert("ng", QT_TR_NOOP("Nigeria"));
    db.insert("no", QT_TR_NOOP("Norway"));
    db.insert("om", QT_TR_NOOP("Oman"));
    db.insert("pk", QT_TR_NOOP("Pakistan"));
    db.insert("pw", QT_TR_NOOP("Palau"));
    db.insert("ps", QT_TR_NOOP("Palestine"));
    db.insert("pa", QT_TR_NOOP("Panama"));
    db.insert("pg", QT_TR_NOOP("Papua New Guinea"));
    db.insert("py", QT_TR_NOOP("Paraguay"));
    db.insert("pe", QT_TR_NOOP("Peru"));
    db.insert("ph", QT_TR_NOOP("Philippines"));
    db.insert("pl", QT_TR_NOOP("Poland"));
    db.insert("pt", QT_TR_NOOP("Portugal"));
    db.insert("qa", QT_TR_NOOP("Qatar"));
    db.insert("ro", QT_TR_NOOP("Romania"));
    db.insert("ru", QT_TR_NOOP("Russia"));
    db.insert("rw", QT_TR_NOOP("Rwanda"));
    db.insert("kn", QT_TR_NOOP("Saint Kitts & Nevis"));
    db.insert("lc", QT_TR_NOOP("Saint Lucia"));
    db.insert("vc", QT_TR_NOOP("Saint Vincent & the Grenadines"));
    db.insert("ws", QT_TR_NOOP("Samoa"));
    db.insert("sm", QT_TR_NOOP("San Marino"));
    db.insert("st", QT_TR_NOOP("Sao Tome & Principe"));
    db.insert("sa", QT_TR_NOOP("Saudi Arabia"));
    db.insert("sn", QT_TR_NOOP("Senegal"));
    db.insert("rs", QT_TR_NOOP("Serbia"));
    db.insert("sc", QT_TR_NOOP("Seychelles"));
    db.insert("sl", QT_TR_NOOP("Sierra Leone"));
    db.insert("sg", QT_TR_NOOP("Singapore"));
    db.insert("sk", QT_TR_NOOP("Slovakia"));
    db.insert("si", QT_TR_NOOP("Slovenia"));
    db.insert("sb", QT_TR_NOOP("Solomon Islands"));
    db.insert("so", QT_TR_NOOP("Somalia"));
    db.insert("za", QT_TR_NOOP("South Africa"));
    db.insert("es", QT_TR_NOOP("Spain"));
    db.insert("lk", QT_TR_NOOP("Sri Lanka"));
    db.insert("sd", QT_TR_NOOP("Sudan"));
    db.insert("sr", QT_TR_NOOP("Suriname"));
    db.insert("sz", QT_TR_NOOP("Swaziland"));
    db.insert("se", QT_TR_NOOP("Sweden"));
    db.insert("ch", QT_TR_NOOP("Switzerland"));
    db.insert("sy", QT_TR_NOOP("Syria"));
    db.insert("tw", QT_TR_NOOP("Taiwan "));
    db.insert("tj", QT_TR_NOOP("Tajikistan"));
    db.insert("tz", QT_TR_NOOP("Tanzania"));
    db.insert("th", QT_TR_NOOP("Thailand"));
    db.insert("tl", QT_TR_NOOP("Timor-Leste (East Timor)"));
    db.insert("tg", QT_TR_NOOP("Togo"));
    db.insert("to", QT_TR_NOOP("Tonga"));
    db.insert("tt", QT_TR_NOOP("Trinidad & Tobago"));
    db.insert("tn", QT_TR_NOOP("Tunisia"));
    db.insert("tr", QT_TR_NOOP("Turkey"));
    db.insert("tm", QT_TR_NOOP("Turkmenistan"));
    db.insert("tv", QT_TR_NOOP("Tuvalu"));
    db.insert("ug", QT_TR_NOOP("Uganda"));
    db.insert("ua", QT_TR_NOOP("Ukraine"));
    db.insert("ae", QT_TR_NOOP("United Arab Emirates"));
    db.insert("gb", QT_TR_NOOP("United Kingdom"));
    db.insert("us", QT_TR_NOOP("United States"));
    db.insert("uy", QT_TR_NOOP("Uruguay"));
    db.insert("uz", QT_TR_NOOP("Uzbekistan"));
    db.insert("vu", QT_TR_NOOP("Vanuatu"));
    db.insert("va", QT_TR_NOOP("Vatican"));
    db.insert("ve", QT_TR_NOOP("Venezuela"));
    db.insert("vn", QT_TR_NOOP("Vietnam"));
    db.insert("eh", QT_TR_NOOP("Western Sahara"));
    db.insert("ye", QT_TR_NOOP("Yemen"));
    db.insert("zr", QT_TR_NOOP("Zaire"));
    db.insert("zm", QT_TR_NOOP("Zambia"));
    db.insert("zw", QT_TR_NOOP("Zimbabwe"));
  }

  QString cc = countryCode.toLower();
  if (db.contains(cc))
    return  tr(db.value(cc).toLocal8Bit().data());
  return QString();
}

QPair<float,float>
CountryInfo::countryLocation(const QString &countryCode)
{
  static QHash<QString,QPair<float,float> > db;
  if (db.isEmpty()) {
    /* Load the country coordinates database */
    QFile infile(COUNTRY_LOCATION_FILE);
    if (! infile.open(QIODevice::ReadOnly | QIODevice::Text))
      return QPair<float,float>(-180.0, -180.0);

    while (! infile.atEnd()) {
      /* Read and parse a single line from the input file */
      bool ok;
      QString line = infile.readLine().trimmed();
      vInfo(line);
      QStringList parts = line.split(",");
      if (parts.size() >= 3) {
        float latitude = parts[1].toFloat(&ok);
        if (! ok)
          continue;
        float longitude = parts[2].toFloat(&ok);
        if (! ok)
          continue;
        db.insert(parts[0], QPair<float,float>(latitude, longitude));
      }
    }
    vInfo("Loaded %1 country location entries from built-in database.").arg(db.size());
    infile.close();
  }

  QString cc = countryCode.toLower();
  if (db.contains(cc))
    return db.value(cc);
  return QPair<float,float>(-180.0, -180.0);
}

