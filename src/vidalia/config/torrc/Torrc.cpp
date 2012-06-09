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
** \file Torrc.cpp
** \brief Handles the interaction with the torrc file
*/

#include "Torrc.h"
#include "Vidalia.h"

#include <QtGlobal>

Torrc::Torrc(const QString &torrcPath, const QString &defaultsPath) :
  QObject(), _torrcPath(torrcPath), _changed(false)
{
  load(torrcPath, defaultsPath);
}

void
Torrc::load(const QString &torrcPath, const QString &defaultsPath)
{
  _torrcMap.clear();
  _defaultsMap.clear();

  _lines = _parser.parse(torrcPath, _torrcMap);

  if(not defaultsPath.isEmpty())
    _parser.parse(defaultsPath, _defaultsMap);
}

bool
Torrc::apply(TorControl *tc, QString *errmsg)
{
  if(not _changed)
    return true;

  QFile torrc(_torrcPath);
  QFileInfo torrc_info(torrc);

  if(!torrc.open(QIODevice::WriteOnly)) {
    *errmsg = "Couldn't open torrc file";
    return false;
  }

  QString torrc_contents = "";
  QString current_line;

  foreach(TorrcLine *line, _lines) {
    torrc_contents += QString("%1 %2")
      .arg(line->content())
      .arg(line->comment()).trimmed();
    torrc_contents += "\n";
  }

  if(torrc_info.isWritable())
    torrc.write(torrc_contents.trimmed().toLatin1());
  else
    vWarn("Torrc is not writable!");

  torrc.close();

  clearAll();
  load(_torrcPath);

  _changed = false;

  bool ret = true;
  if(tc && tc->isConnected()) {
    if(!tc->loadConf(torrc_contents, errmsg)) {
      vWarn(QObject::tr("There were some settings that Vidalia "
                        "wasn't able to apply together"));
      vWarn("Trying to apply them one at a time...");
      QPair<QString, TorOpt> value;
      QString ferrmsg;
      ControlReply reply;
      bool somefailed = false;
      foreach(QString key, _torrcMap.keys()) {
        foreach(value, _torrcMap.values(key)) {
          if(not tc->setConf(key, value.first, &ferrmsg, &reply)) {
            if(reply.getStatus() == "553") {
              vWarn(QObject::tr("Failed to set %1:\nReason: %2")
                    .arg(key).arg(ferrmsg));
              somefailed = true;
            } else {
              *errmsg = ferrmsg;
              return false;
            }
          }
        }
      }
      if(somefailed) {
        if(torrc_info.isWritable())
          vWarn(QObject::tr("The failed options were saved in your torrc "
                            "and will be applied when you restart."));
        else
          vWarn(QObject::tr("The failed options were NOT saved in your torrc "
                            "and will be applied when you restart."));
      }
      if(not torrc_info.isWritable())
        vWarn(QObject::tr("Vidalia was unable to save the options to the torrc file."));
    }
  }
  return true;
}

QStringList
Torrc::value(const QString &key) const
{
  QStringList ret;
  QPair<QString,TorOpt> value;
  if(_torrcMap.contains(key)) {
    foreach(value, _torrcMap.values(key))
      ret << value.first;
    return ret;
  }

  if(_defaultsMap.contains(key)) {
    foreach(value, _defaultsMap.values(key))
      ret << value.first;
    return ret;
  }

  return ret;
}

void
Torrc::setValue(const QString &key, const QString &value, const QString &comment)
{
  // Prevent possible bugs
  if(not key.isEmpty() and value.isEmpty())
    return;

  if(_defaultsMap.contains(key)) {
    QPair<QString,TorOpt> defvalue;
    foreach(defvalue, _defaultsMap.values(key)) {
      if(defvalue.first == value)
        return;
    }
  }

  if(_torrcMap.contains(key)) {
    QPair<QString,TorOpt> val;
    foreach(val, _torrcMap.values(key)) {
      if(not val.second.isMultilined() and val.first != value) {
        val.second.line()->setContent(QString("%1 %2")
                                      .arg(key)
                                      .arg(value));
        if(not comment.isEmpty() and val.second.line()->comment().isEmpty())
          val.second.line()->setComment(comment);
        _changed = true;
        return;
      } else if(not val.second.isMultilined() and val.first == value) {
        // if it's the same, leave it like that
        return;
      } // else it's multilined, in which case we just add a new entry
    }
  }

  _changed = true;

  TorrcLine *line = new TorrcLine(QString("%1 %2")
                                  .arg(key)
                                  .arg(value).trimmed(),
                                  comment.trimmed());
  if(key.isEmpty()) {
    _lines << line;
    return;
  }

  TorOpt opt(_parser.getTorOpt(key));

  if(opt.isMultivalued()) {
    bool found = false;
    QPair<QString,TorOpt> val;
    foreach(val, _torrcMap.values(key)) {
      if(val.first == value) {
        found = true;
        break;
      }
    }

    opt.setLine(line);

    if(!found)
      _torrcMap.insertMulti(key, QPair<QString,TorOpt>(value, opt));
    else
      return;
  } else
    _torrcMap.insert(key, QPair<QString,TorOpt>(value, opt));

  _lines << line;
}

void
Torrc::clear(QStringList &keys)
{
  QPair<QString, TorOpt> value;
  foreach(const QString key, keys) {
    foreach(value, _torrcMap.values(key)) {
      if(value.second.line()) {
        value.second.line()->setContent("");
        if(value.second.line()->comment().isEmpty())
          _lines.removeAll(value.second.line());
      }
    }
    _torrcMap.remove(key);
    _changed = true;
  }
}

bool
Torrc::isValid(const QString &key, const QString &value)
{
  if(not _parser.getTorOpt(key).isNull())
    return true;
  // TODO: check value
  return false;
}

void
Torrc::clearAll()
{
  _torrcMap.clear();
  _lines.clear();
}

bool
Torrc::setRawContents(const QString &contents, QString *errmsg, TorControl *tc)
{
  clearAll();

  QFile torrc(_torrcPath);
  if(!torrc.open(QIODevice::WriteOnly)) {
    *errmsg = "Couldn't open torrc file";
    return false;
  }

  torrc.write(contents.trimmed().toLatin1());
  torrc.close();

  load(_torrcPath);

  return apply(tc, errmsg);
}
