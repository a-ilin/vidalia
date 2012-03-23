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
** \file LogFilter.h
** \brief Object used to filter message log history
*/

#ifndef _LOGFILTER_H
#define _LOGFILTER_H

#include "tcglobal.h"
#include <QRegExp>

/** Base class for objects used to filter log history. */
class LogFilter {

public:
  /** Constructor taking severity filter as argument. */
  LogFilter(uint filter);
  virtual ~LogFilter();
  /** Method used to evaluate given log message. */
  virtual bool eval(tc::Severity type, const QString& message) const;

private:
  /** Stores severity filter */
  uint _filter;
};


/** Message filter based on regular expression. */
class LogFilterRegExp : public LogFilter {

public:
  LogFilterRegExp(const QRegExp& exp, uint filter);
  bool eval(tc::Severity type, const QString &message) const;

private:
  /** Stores regular expression used to filter messages. */
  QRegExp _regExp;
};


/** Message filter based on simple search query. */
class LogFilterSearchTerm : public LogFilter {

public:
  LogFilterSearchTerm(const QString& term, uint filter);
  ~LogFilterSearchTerm();
  bool eval(tc::Severity type, const QString &message) const;

private:
  class ExpTree;

  /** Helper method used to create expression tree from given query. */
  void parseString(const QString& input);

  /** Stores the expression tree for filtering messages. */
  ExpTree * _expressionTree;
};

#endif

