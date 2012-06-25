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
    db.insert("a1", QT_TR_NOOP("Anonymous Proxy"));
    db.insert("a2", QT_TR_NOOP("Satellite Provider"));
    db.insert("ad", QT_TR_NOOP("Andorra"));
    db.insert("ae", QT_TR_NOOP("United Arab Emirates"));
    db.insert("af", QT_TR_NOOP("Afghanistan"));
    db.insert("ag", QT_TR_NOOP("Antigua and Barbuda"));
    db.insert("ai", QT_TR_NOOP("Anguilla"));
    db.insert("al", QT_TR_NOOP("Albania"));
    db.insert("am", QT_TR_NOOP("Armenia"));
    db.insert("an", QT_TR_NOOP("Netherlands Antilles"));
    db.insert("ao", QT_TR_NOOP("Angola"));
    db.insert("ap", QT_TR_NOOP("Asia/Pacific Region"));
    db.insert("aq", QT_TR_NOOP("Antarctica"));
    db.insert("ar", QT_TR_NOOP("Argentina"));
    db.insert("as", QT_TR_NOOP("American Samoa"));
    db.insert("at", QT_TR_NOOP("Austria"));
    db.insert("au", QT_TR_NOOP("Australia"));
    db.insert("aw", QT_TR_NOOP("Aruba"));
    db.insert("ax", QT_TR_NOOP("Aland Islands"));
    db.insert("az", QT_TR_NOOP("Azerbaijan"));
    db.insert("ba", QT_TR_NOOP("Bosnia and Herzegovina"));
    db.insert("bb", QT_TR_NOOP("Barbados"));
    db.insert("bd", QT_TR_NOOP("Bangladesh"));
    db.insert("be", QT_TR_NOOP("Belgium"));
    db.insert("bf", QT_TR_NOOP("Burkina Faso"));
    db.insert("bg", QT_TR_NOOP("Bulgaria"));
    db.insert("bh", QT_TR_NOOP("Bahrain"));
    db.insert("bi", QT_TR_NOOP("Burundi"));
    db.insert("bj", QT_TR_NOOP("Benin"));
    db.insert("bl", QT_TR_NOOP("Saint Barthelemy"));
    db.insert("bm", QT_TR_NOOP("Bermuda"));
    db.insert("bn", QT_TR_NOOP("Brunei Darussalam"));
    db.insert("bo", QT_TR_NOOP("Bolivia"));
    db.insert("br", QT_TR_NOOP("Brazil"));
    db.insert("bs", QT_TR_NOOP("Bahamas"));
    db.insert("bt", QT_TR_NOOP("Bhutan"));
    db.insert("bv", QT_TR_NOOP("Bouvet Island"));
    db.insert("bw", QT_TR_NOOP("Botswana"));
    db.insert("by", QT_TR_NOOP("Belarus"));
    db.insert("bz", QT_TR_NOOP("Belize"));
    db.insert("ca", QT_TR_NOOP("Canada"));
    db.insert("cc", QT_TR_NOOP("Cocos (Keeling) Islands"));
    db.insert("cd", QT_TR_NOOP("Congo, The Democratic Republic of the"));
    db.insert("cf", QT_TR_NOOP("Central African Republic"));
    db.insert("cg", QT_TR_NOOP("Congo"));
    db.insert("ch", QT_TR_NOOP("Switzerland"));
    db.insert("ci", QT_TR_NOOP("Cote D'Ivoire"));
    db.insert("ck", QT_TR_NOOP("Cook Islands"));
    db.insert("cl", QT_TR_NOOP("Chile"));
    db.insert("cm", QT_TR_NOOP("Cameroon"));
    db.insert("cn", QT_TR_NOOP("China"));
    db.insert("co", QT_TR_NOOP("Colombia"));
    db.insert("cr", QT_TR_NOOP("Costa Rica"));
    db.insert("cu", QT_TR_NOOP("Cuba"));
    db.insert("cv", QT_TR_NOOP("Cape Verde"));
    db.insert("cx", QT_TR_NOOP("Christmas Island"));
    db.insert("cy", QT_TR_NOOP("Cyprus"));
    db.insert("cz", QT_TR_NOOP("Czech Republic"));
    db.insert("de", QT_TR_NOOP("Germany"));
    db.insert("dj", QT_TR_NOOP("Djibouti"));
    db.insert("dk", QT_TR_NOOP("Denmark"));
    db.insert("dm", QT_TR_NOOP("Dominica"));
    db.insert("do", QT_TR_NOOP("Dominican Republic"));
    db.insert("dz", QT_TR_NOOP("Algeria"));
    db.insert("ec", QT_TR_NOOP("Ecuador"));
    db.insert("ee", QT_TR_NOOP("Estonia"));
    db.insert("eg", QT_TR_NOOP("Egypt"));
    db.insert("eh", QT_TR_NOOP("Western Sahara"));
    db.insert("er", QT_TR_NOOP("Eritrea"));
    db.insert("es", QT_TR_NOOP("Spain"));
    db.insert("et", QT_TR_NOOP("Ethiopia"));
    db.insert("eu", QT_TR_NOOP("Europe"));
    db.insert("fi", QT_TR_NOOP("Finland"));
    db.insert("fj", QT_TR_NOOP("Fiji"));
    db.insert("fk", QT_TR_NOOP("Falkland Islands (Malvinas)"));
    db.insert("fm", QT_TR_NOOP("Micronesia, Federated States of"));
    db.insert("fo", QT_TR_NOOP("Faroe Islands"));
    db.insert("fr", QT_TR_NOOP("France"));
    db.insert("ga", QT_TR_NOOP("Gabon"));
    db.insert("gb", QT_TR_NOOP("United Kingdom"));
    db.insert("gd", QT_TR_NOOP("Grenada"));
    db.insert("ge", QT_TR_NOOP("Georgia"));
    db.insert("gf", QT_TR_NOOP("French Guiana"));
    db.insert("gg", QT_TR_NOOP("Guernsey"));
    db.insert("gh", QT_TR_NOOP("Ghana"));
    db.insert("gi", QT_TR_NOOP("Gibraltar"));
    db.insert("gl", QT_TR_NOOP("Greenland"));
    db.insert("gm", QT_TR_NOOP("Gambia"));
    db.insert("gn", QT_TR_NOOP("Guinea"));
    db.insert("gp", QT_TR_NOOP("Guadeloupe"));
    db.insert("gq", QT_TR_NOOP("Equatorial Guinea"));
    db.insert("gr", QT_TR_NOOP("Greece"));
    db.insert("gs", QT_TR_NOOP("South Georgia and the South Sandwich Islands"));
    db.insert("gt", QT_TR_NOOP("Guatemala"));
    db.insert("gu", QT_TR_NOOP("Guam"));
    db.insert("gw", QT_TR_NOOP("Guinea-Bissau"));
    db.insert("gy", QT_TR_NOOP("Guyana"));
    db.insert("hk", QT_TR_NOOP("Hong Kong"));
    db.insert("hm", QT_TR_NOOP("Heard Island and McDonald Islands"));
    db.insert("hn", QT_TR_NOOP("Honduras"));
    db.insert("hr", QT_TR_NOOP("Croatia"));
    db.insert("ht", QT_TR_NOOP("Haiti"));
    db.insert("hu", QT_TR_NOOP("Hungary"));
    db.insert("id", QT_TR_NOOP("Indonesia"));
    db.insert("ie", QT_TR_NOOP("Ireland"));
    db.insert("il", QT_TR_NOOP("Israel"));
    db.insert("im", QT_TR_NOOP("Isle of Man"));
    db.insert("in", QT_TR_NOOP("India"));
    db.insert("io", QT_TR_NOOP("British Indian Ocean Territory"));
    db.insert("iq", QT_TR_NOOP("Iraq"));
    db.insert("ir", QT_TR_NOOP("Iran, Islamic Republic of"));
    db.insert("is", QT_TR_NOOP("Iceland"));
    db.insert("it", QT_TR_NOOP("Italy"));
    db.insert("je", QT_TR_NOOP("Jersey"));
    db.insert("jm", QT_TR_NOOP("Jamaica"));
    db.insert("jo", QT_TR_NOOP("Jordan"));
    db.insert("jp", QT_TR_NOOP("Japan"));
    db.insert("ke", QT_TR_NOOP("Kenya"));
    db.insert("kg", QT_TR_NOOP("Kyrgyzstan"));
    db.insert("kh", QT_TR_NOOP("Cambodia"));
    db.insert("ki", QT_TR_NOOP("Kiribati"));
    db.insert("km", QT_TR_NOOP("Comoros"));
    db.insert("kn", QT_TR_NOOP("Saint Kitts and Nevis"));
    db.insert("kp", QT_TR_NOOP("Korea, Democratic People's Republic of"));
    db.insert("kr", QT_TR_NOOP("Korea, Republic of"));
    db.insert("kw", QT_TR_NOOP("Kuwait"));
    db.insert("ky", QT_TR_NOOP("Cayman Islands"));
    db.insert("kz", QT_TR_NOOP("Kazakhstan"));
    db.insert("la", QT_TR_NOOP("Lao People's Democratic Republic"));
    db.insert("lb", QT_TR_NOOP("Lebanon"));
    db.insert("lc", QT_TR_NOOP("Saint Lucia"));
    db.insert("li", QT_TR_NOOP("Liechtenstein"));
    db.insert("lk", QT_TR_NOOP("Sri Lanka"));
    db.insert("lr", QT_TR_NOOP("Liberia"));
    db.insert("ls", QT_TR_NOOP("Lesotho"));
    db.insert("lt", QT_TR_NOOP("Lithuania"));
    db.insert("lu", QT_TR_NOOP("Luxembourg"));
    db.insert("lv", QT_TR_NOOP("Latvia"));
    db.insert("ly", QT_TR_NOOP("Libyan Arab Jamahiriya"));
    db.insert("ma", QT_TR_NOOP("Morocco"));
    db.insert("mc", QT_TR_NOOP("Monaco"));
    db.insert("md", QT_TR_NOOP("Moldova, Republic of"));
    db.insert("me", QT_TR_NOOP("Montenegro"));
    db.insert("mf", QT_TR_NOOP("Saint Martin"));
    db.insert("mg", QT_TR_NOOP("Madagascar"));
    db.insert("mh", QT_TR_NOOP("Marshall Islands"));
    db.insert("mk", QT_TR_NOOP("Macedonia"));
    db.insert("ml", QT_TR_NOOP("Mali"));
    db.insert("mm", QT_TR_NOOP("Myanmar"));
    db.insert("mn", QT_TR_NOOP("Mongolia"));
    db.insert("mo", QT_TR_NOOP("Macau"));
    db.insert("mp", QT_TR_NOOP("Northern Mariana Islands"));
    db.insert("mq", QT_TR_NOOP("Martinique"));
    db.insert("mr", QT_TR_NOOP("Mauritania"));
    db.insert("ms", QT_TR_NOOP("Montserrat"));
    db.insert("mt", QT_TR_NOOP("Malta"));
    db.insert("mu", QT_TR_NOOP("Mauritius"));
    db.insert("mv", QT_TR_NOOP("Maldives"));
    db.insert("mw", QT_TR_NOOP("Malawi"));
    db.insert("mx", QT_TR_NOOP("Mexico"));
    db.insert("my", QT_TR_NOOP("Malaysia"));
    db.insert("mz", QT_TR_NOOP("Mozambique"));
    db.insert("na", QT_TR_NOOP("Namibia"));
    db.insert("nc", QT_TR_NOOP("New Caledonia"));
    db.insert("ne", QT_TR_NOOP("Niger"));
    db.insert("nf", QT_TR_NOOP("Norfolk Island"));
    db.insert("ng", QT_TR_NOOP("Nigeria"));
    db.insert("ni", QT_TR_NOOP("Nicaragua"));
    db.insert("nl", QT_TR_NOOP("Netherlands"));
    db.insert("no", QT_TR_NOOP("Norway"));
    db.insert("np", QT_TR_NOOP("Nepal"));
    db.insert("nr", QT_TR_NOOP("Nauru"));
    db.insert("nu", QT_TR_NOOP("Niue"));
    db.insert("nz", QT_TR_NOOP("New Zealand"));
    db.insert("om", QT_TR_NOOP("Oman"));
    db.insert("pa", QT_TR_NOOP("Panama"));
    db.insert("pe", QT_TR_NOOP("Peru"));
    db.insert("pf", QT_TR_NOOP("French Polynesia"));
    db.insert("pg", QT_TR_NOOP("Papua New Guinea"));
    db.insert("ph", QT_TR_NOOP("Philippines"));
    db.insert("pk", QT_TR_NOOP("Pakistan"));
    db.insert("pl", QT_TR_NOOP("Poland"));
    db.insert("pm", QT_TR_NOOP("Saint Pierre and Miquelon"));
    db.insert("pn", QT_TR_NOOP("Pitcairn Islands"));
    db.insert("pr", QT_TR_NOOP("Puerto Rico"));
    db.insert("ps", QT_TR_NOOP("Palestinian Territory, Occupied"));
    db.insert("pt", QT_TR_NOOP("Portugal"));
    db.insert("pw", QT_TR_NOOP("Palau"));
    db.insert("py", QT_TR_NOOP("Paraguay"));
    db.insert("qa", QT_TR_NOOP("Qatar"));
    db.insert("re", QT_TR_NOOP("Reunion"));
    db.insert("ro", QT_TR_NOOP("Romania"));
    db.insert("rs", QT_TR_NOOP("Serbia"));
    db.insert("ru", QT_TR_NOOP("Russian Federation"));
    db.insert("rw", QT_TR_NOOP("Rwanda"));
    db.insert("sa", QT_TR_NOOP("Saudi Arabia"));
    db.insert("sb", QT_TR_NOOP("Solomon Islands"));
    db.insert("sc", QT_TR_NOOP("Seychelles"));
    db.insert("sd", QT_TR_NOOP("Sudan"));
    db.insert("se", QT_TR_NOOP("Sweden"));
    db.insert("sg", QT_TR_NOOP("Singapore"));
    db.insert("sh", QT_TR_NOOP("Saint Helena"));
    db.insert("si", QT_TR_NOOP("Slovenia"));
    db.insert("sj", QT_TR_NOOP("Svalbard and Jan Mayen"));
    db.insert("sk", QT_TR_NOOP("Slovakia"));
    db.insert("sl", QT_TR_NOOP("Sierra Leone"));
    db.insert("sm", QT_TR_NOOP("San Marino"));
    db.insert("sn", QT_TR_NOOP("Senegal"));
    db.insert("so", QT_TR_NOOP("Somalia"));
    db.insert("sr", QT_TR_NOOP("Suriname"));
    db.insert("st", QT_TR_NOOP("Sao Tome and Principe"));
    db.insert("sv", QT_TR_NOOP("El Salvador"));
    db.insert("sy", QT_TR_NOOP("Syrian Arab Republic"));
    db.insert("sz", QT_TR_NOOP("Swaziland"));
    db.insert("tc", QT_TR_NOOP("Turks and Caicos Islands"));
    db.insert("td", QT_TR_NOOP("Chad"));
    db.insert("tf", QT_TR_NOOP("French Southern Territories"));
    db.insert("tg", QT_TR_NOOP("Togo"));
    db.insert("th", QT_TR_NOOP("Thailand"));
    db.insert("tj", QT_TR_NOOP("Tajikistan"));
    db.insert("tk", QT_TR_NOOP("Tokelau"));
    db.insert("tl", QT_TR_NOOP("Timor-Leste"));
    db.insert("tm", QT_TR_NOOP("Turkmenistan"));
    db.insert("tn", QT_TR_NOOP("Tunisia"));
    db.insert("to", QT_TR_NOOP("Tonga"));
    db.insert("tr", QT_TR_NOOP("Turkey"));
    db.insert("tt", QT_TR_NOOP("Trinidad and Tobago"));
    db.insert("tv", QT_TR_NOOP("Tuvalu"));
    db.insert("tw", QT_TR_NOOP("Taiwan"));
    db.insert("tz", QT_TR_NOOP("Tanzania, United Republic of"));
    db.insert("ua", QT_TR_NOOP("Ukraine"));
    db.insert("ug", QT_TR_NOOP("Uganda"));
    db.insert("um", QT_TR_NOOP("United States Minor Outlying Islands"));
    db.insert("us", QT_TR_NOOP("United States"));
    db.insert("uy", QT_TR_NOOP("Uruguay"));
    db.insert("uz", QT_TR_NOOP("Uzbekistan"));
    db.insert("va", QT_TR_NOOP("Holy See (Vatican City State)"));
    db.insert("vc", QT_TR_NOOP("Saint Vincent and the Grenadines"));
    db.insert("ve", QT_TR_NOOP("Venezuela"));
    db.insert("vg", QT_TR_NOOP("Virgin Islands, British"));
    db.insert("vi", QT_TR_NOOP("Virgin Islands, U.S."));
    db.insert("vn", QT_TR_NOOP("Vietnam"));
    db.insert("vu", QT_TR_NOOP("Vanuatu"));
    db.insert("wf", QT_TR_NOOP("Wallis and Futuna"));
    db.insert("ws", QT_TR_NOOP("Samoa"));
    db.insert("ye", QT_TR_NOOP("Yemen"));
    db.insert("yt", QT_TR_NOOP("Mayotte"));
    db.insert("za", QT_TR_NOOP("South Africa"));
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

