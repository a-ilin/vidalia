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
** \file TorrcParser.h
** \brief Parser for the torrc file and other data representations for the file
*/

#ifndef TORRCPARSER_H
#define TORRCPARSER_H

#include <QtCore>

/** Rpresents a line in the torrc file that respects the formatting
 *  the user gave it */
class TorrcLine
{
 public:
  /** Constructor */
  TorrcLine(const QString &content = "", const QString &comment = "")
    : _content(content), _comment(comment) {}

  /** Returns the content for this line */
  QString content()
  { return _content; }
  /** Sets the contents for this line */
  void setContent(const QString &content)
  { _content = content; }

  /** Returns the comment for this line */
  QString comment()
  { return _comment; }
  /** Sets the comment for this line */
  void setComment(const QString &comment)
  { _comment = comment; }

  /** Returns true if the line's content is divided into other
   *  subsequent lines */
  bool isMultilined()
  {
    int index = -1;
    index = _content.lastIndexOf("\\") + 1;
    if(index == -1)
      return false;
    for(int i = index; i<_content.size(); i++) {
      if(_content.at(i) != ' ')
        return false;
    }
    return true;
  }

 private:
  /** Strings that hold the content and comment for the line */
  QString _content, _comment;
};

/** Represents an possible option in the torrc file. This is used to
 *  handle the different type of values that may be associated with
 *  each option */
class TorOpt
{
 public:
  /** Possible value types */
  enum OptType {
    TimeInterval = 0,
    TimeMsecInterval,
    String,
    RouterList,
    LineList,
    Float,
    Dependant,
    Virtual,
    Filename,
    CommaList,
    Boolean,
    BooleanAuto,
    Integer,
    DataSize,
    Port,
    NIL
  };

  /** Default constructor */
  TorOpt()
    { TorOpt("", NIL); }
  /** Constructor */
 TorOpt(const QString &name, OptType type, QString defaultValue = "")
   : _name(name), _type(type), _default(defaultValue), _line(0) {}
  /** Copy constructor */
 TorOpt(const TorOpt &opt)
   : _name(opt.name()), _type(opt.type()), _default(opt.defaultValue()), _line(opt.line()) {}

  /** Returns true if this option can be defined with multiple lines */
  bool isMultilined()
  { return _type == LineList or _type == Dependant or _type == Virtual; }

  /** Returns true if this option can be defined multiple times with
   *  different values */
  bool isMultivalued()
  { return _type == LineList; }

  /** Returns the name for this option */
  QString name() const
  { return _name; }

  /** Returns the type for this option*/
  OptType type() const
  { return _type; }

  /** Returns the default value that tor assumes for this option */
  QString defaultValue() const
  { return _default; }

  /** Returns a pointer to the TorrcLine for this option */
  TorrcLine *line() const
  { return _line; }

  /** Sets the line for this option */
  void setLine(TorrcLine *line)
  { _line = line; }

  /** Returns true if it's a null TorOpt */
  bool isNull() const
  { return _type == NIL; }

  /** Creates a null TorOpt*/
  static TorOpt null()
  { return TorOpt("Nil", NIL); }

 private:
  /** Name for this option */
  QString _name;
  /** Type for this option */
  OptType _type;
  /** Default value for this option */
  QString _default;
  /** Pointer to the line that represents this option */
  TorrcLine *_line;
};

/** Parser for the torrc file and any other torrc structured file */
class TorrcParser
{
 public:
  /** Default constructor */
  TorrcParser();

  /** Parses the torrc file provided in the path and returns the
   *  TorrcLine list for the file and the map of key=value */
  QList<TorrcLine *> parse(const QString &path,
                           QMap<QString, QPair<QString, TorOpt> > &map);
  /** Returns the TorOpt for a given option name */
  TorOpt getTorOpt(const QString &name);

 private:
  /** List with all the current valid options for the torrc */
  QList<TorOpt> _torOpts;
  /** Regular expression used in the parsing based on _torOpts */
  QRegExp _regexp;

  /** Strips the torrc contents passed from comments an empty lines */
  QString stripContents(QString contents);
  /** Given the contents of a torrc file, it returns a list of
   *  TorrcLines representing it */
  QList<TorrcLine *> toTorrcLines(QString contents);
  /** Given a TorrcLine and the line list where it belongs, it returns
   *  the set of lines that define the multilined value */
  QList<TorrcLine *> findValueLines(TorrcLine *line,
                                    const QList<TorrcLine *> &lines) const;
  /** Creates the regular expression used in parsing */
  QRegExp createRegExp();
  /** Returns true if there's a keyword in the contents at the given
   *  pos */
  bool isKeyword(const QString &contents, int pos);
};

/** Debug output methods */
QDebug operator<<(QDebug dbg, const TorOpt &opt);
QDebug operator<<(QDebug dbg, TorrcLine *line);

#endif

