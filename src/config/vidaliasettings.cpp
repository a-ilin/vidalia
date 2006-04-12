/****************************************************************
 *  Vidalia is distributed under the following license:
 *
 *  Copyright (C) 2006,  Matt Edman, Justin Hipple
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, 
 *  Boston, MA  02110-1301, USA.
 ****************************************************************/

/** 
 * \file vidaliasettings.cpp
 * \version $Id$
 */

#include <QDir>
#include <QCoreApplication>
#include <lang/languagesupport.h>

#include "vidaliasettings.h"

#if defined(Q_WS_WIN)
#include <util/registry.h>
#endif


/* Vidalia's Settings */
#define SETTING_LANGUAGE            "LanguageCode"
#define SETTING_STYLE               "InterfaceStyle"
#define SETTING_RUN_TOR_AT_START    "RunTorAtStart"

#define SETTING_MSG_FILTER          "MessageLog/MessageFilter"
#define SETTING_MAX_MESSAGE         "MessageLog/MaxMsgCount"
#define SETTING_ENABLE_LOG_FILE     "MessageLog/EnableLogFile"
#define SETTING_LOG_FILE            "MessageLog/LogFile"

#define SETTING_BWGRAPH_FILTER      "BandwidthGraph/BWLineFilter"
#define SETTING_BWGRAPH_OPACITY     "BandwidthGraph/Opacity"

/* Default Vidalia Settings */
#if defined(Q_WS_MAC)
#define DEFAULT_STYLE               "macintosh (aqua)"
#else
#define DEFAULT_STYLE               "plastique"
#endif

#define DEFAULT_LANGUAGE            LanguageSupport::defaultLanguageCode()
#define DEFAULT_RUN_TOR_AT_START    false
#define DEFAULT_OPACITY             100

/* Default message log settings */
#define DEFAULT_MSG_FILTER          (TOR_ERROR|TOR_WARN|TOR_NOTICE)
#define DEFAULT_MAX_MESSAGE         500
#define DEFAULT_ENABLE_LOG_FILE     false

#if defined(Q_OS_WIN32)
#define DEFAULT_LOG_FILE       (QDir::rootPath() + "Program Files\\Tor\\tor.log")
#define STARTUP_REG_KEY        "Software\\Microsoft\\Windows\\CurrentVersion\\Run"
#define VIDALIA_REG_KEY        "Vidalia" 
#else
#define DEFAULT_LOG_FILE       (QDir::homePath() + "/.tor/tor.log")
#endif

/* Default bandwidth graph settings */
#define DEFAULT_BWGRAPH_FILTER  (BWGRAPH_SEND|BWGRAPH_REC)

/** Default Constructor
 * We use "Vidalia" for both the company name and the application name.
 */
VidaliaSettings::VidaliaSettings()
  : QSettings("vidalia", "vidalia")
{
}

/** Resets all of Vidalia's settings. */
void
VidaliaSettings::reset()
{
  QSettings settings("vidalia", "vidalia");
  settings.clear();
}

/** Gets the currently preferred language code for Vidalia. */
QString
VidaliaSettings::getLanguageCode()
{
  return value(SETTING_LANGUAGE, DEFAULT_LANGUAGE).toString();
}

/** Sets the preferred language code. */
void
VidaliaSettings::setLanguageCode(QString languageCode)
{
  setValue(SETTING_LANGUAGE, languageCode);
}

/** Gets the interface style key (e.g., "windows", "motif", etc.) */
QString
VidaliaSettings::getInterfaceStyle()
{
  return value(SETTING_STYLE, DEFAULT_STYLE).toString();
}

/** Sets the interface style key. */
void
VidaliaSettings::setInterfaceStyle(QString styleKey)
{
  setValue(SETTING_STYLE, styleKey);
}

/** Returns true if Tor is to be run when Vidalia starts. */
bool
VidaliaSettings::runTorAtStart()
{
  return value(SETTING_RUN_TOR_AT_START, DEFAULT_RUN_TOR_AT_START).toBool();
}

/** If <b>run</b> is set to true, then Tor will be run when Vidalia starts. */
void
VidaliaSettings::setRunTorAtStart(bool run)
{
  setValue(SETTING_RUN_TOR_AT_START, run);
}

/** Returns true if Vidalia is set to run on system boot. */
bool
VidaliaSettings::runVidaliaOnBoot()
{
#if defined(Q_WS_WIN)
  if (!registry_get_key_value(STARTUP_REG_KEY, VIDALIA_REG_KEY).isEmpty()) {
    return true;
  } else {
    return false;
  }
#else
  /* Platforms other than windows aren't supported yet */
  return false;
#endif
}

/** If <b>run</b> is set to true, then Vidalia will run on system boot. */
void
VidaliaSettings::setRunVidaliaOnBoot(bool run)
{
#if defined(Q_WS_WIN)
  if (run) {
    registry_set_key_value(STARTUP_REG_KEY, VIDALIA_REG_KEY,
        QString("\"" +
                QDir::convertSeparators(QCoreApplication::applicationFilePath())) +
                "\"");
  } else {
    registry_remove_key(STARTUP_REG_KEY, VIDALIA_REG_KEY);
  }
#else
  /* Platforms othe rthan windows aren't supported yet */
  Q_UNUSED(run);
  return;
#endif
}

/** Returns the current message filter. */
uint
VidaliaSettings::getMsgFilter()
{
  return value(SETTING_MSG_FILTER, DEFAULT_MSG_FILTER).toUInt(); 
}

/** Saves the setting for whether or not the given message severity will be
 * displayed. */
void
VidaliaSettings::setMsgFilter(LogEvent::Severity severity, bool add)
{
  uint filter = getMsgFilter();
  filter = (add ? (filter | (uint)severity) : (filter & ~((uint)severity)));
  setValue(SETTING_MSG_FILTER, filter);
}

/** Set maximum number of messages to display in log. */
void
VidaliaSettings::setMaxMsgCount(int max)
{
  setValue(SETTING_MAX_MESSAGE, max);
}

/** Return maximum number of messages to display in log. */
int
VidaliaSettings::getMaxMsgCount()
{
  return value(SETTING_MAX_MESSAGE, DEFAULT_MAX_MESSAGE).toInt();
}

/** Returns whether or not to enable automatically saving log messages from
 * the message log to a file on disk. */
bool
VidaliaSettings::isLogFileEnabled()
{
  return value(SETTING_ENABLE_LOG_FILE, 
               DEFAULT_ENABLE_LOG_FILE).toBool();
}

/** Sets whether or not to enable automatically saving log messages from the
 * message log to a file on disk. */
void
VidaliaSettings::enableLogFile(bool enable)
{
  setValue(SETTING_ENABLE_LOG_FILE, enable);
}

/** Gets the destination file on disk to which log messages can be saved. */
QString
VidaliaSettings::getLogFile()
{
  return QDir::convertSeparators(
                  value(SETTING_LOG_FILE, DEFAULT_LOG_FILE).toString());
}

/** Sets the destination file on disk to which log messages can be saved. */
void
VidaliaSettings::setLogFile(QString file)
{
  setValue(SETTING_LOG_FILE, QDir::convertSeparators(file));
}

/** Returns the bandwidth line filter. */
uint
VidaliaSettings::getBWGraphFilter()
{
  return value(SETTING_BWGRAPH_FILTER, DEFAULT_BWGRAPH_FILTER).toUInt(); 
}

/** Saves the setting for whether or not the given line will be graphed */
void
VidaliaSettings::setBWGraphFilter(uint line, bool status)
{
  uint filter = getBWGraphFilter();
  filter = (status ? (filter | line) : (filter & ~(line)));
  setValue(SETTING_BWGRAPH_FILTER, filter);
}

/** Get the level of opacity for the BandwidthGraph window. */
int
VidaliaSettings::getBWGraphOpacity()
{
  return value(SETTING_BWGRAPH_OPACITY, DEFAULT_OPACITY).toInt();
}

/** Set the level of opacity for the BandwidthGraph window. */
void
VidaliaSettings::setBWGraphOpacity(int value)
{
  setValue(SETTING_BWGRAPH_OPACITY, value);
}

