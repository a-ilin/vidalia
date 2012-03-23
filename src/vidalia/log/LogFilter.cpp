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
** \file LogFilter.cpp
** \brief Object used to filter message log history
*/

#include "LogFilter.h"
#include <QStack>

/** Constructor taking severity as argument. */
LogFilter::LogFilter(uint filter)
  : _filter(filter)
{
}

/** Default destructor. */
LogFilter::~LogFilter()
{
}

/** Evaluate if message should be included into log history.
 * \param type Message severity
 * \param message Message
 */
bool
LogFilter::eval(tc::Severity type, const QString &message) const
{
  Q_UNUSED(message);
  return (_filter & (uint)type);
}

/** Constructor regular expression and severity. */
LogFilterRegExp::LogFilterRegExp(const QRegExp &exp, uint filter)
  :LogFilter(filter)
  ,_regExp(exp)
{

}

/** Evaluate if message should be included into log history.
 * \param type Message severity
 * \param message Message
 */
bool
LogFilterRegExp::eval(tc::Severity type, const QString &message) const
{
  if (LogFilter::eval(type, message))
    return _regExp.isValid() ? message.contains(_regExp) : true;
  return false;
}

/** Helper class for search expression tree. */
class LogFilterSearchTerm::ExpTree {
public:
  ExpTree(const QString& value=QString())
  {
    _value = value;
    _left = NULL;
    _right = NULL;
    _invert = false;
  }

  ~ExpTree()
  {
    if (_left)
      delete _left;
    if (_right)
      delete _right;
  }

  bool eval(const QString& input)
  {
    if (_value.isEmpty())
      return true;
    bool result;
    if (_value == "|") {
      if (!_left || !_right)
        return false;
      result = _left->eval(input) || _right->eval(input);
    } else if (_value == "&") {
      if (!_left || !_right)
        return false;
      result = _left->eval(input) && _right->eval(input);
    } else {
      result = input.contains(_value);
    }
    return _invert ? !result : result;
  }

private:
  /** Stores the value of this node, can be a string or operator. */
  QString _value;
  /** Remembers if the result of this node evaluation should be inverted. */
  bool _invert;
  /** Left child node. */
  ExpTree * _left;
  /** Right child node. */
  ExpTree * _right;

  friend class LogFilterSearchTerm;
};

/** Constructor taking search term and severity as arguments. */
LogFilterSearchTerm::LogFilterSearchTerm(const QString &term, uint filter)
  :LogFilter(filter)
  ,_expressionTree(NULL)
{
  parseString(term);
}

/** Default destructor. */
LogFilterSearchTerm::~LogFilterSearchTerm()
{
  if (_expressionTree)
    delete _expressionTree;
}

/** Evaluate if message should be included into log history.
 * \param type Message severity
 * \param msg Message
 */
bool
LogFilterSearchTerm::eval(tc::Severity type, const QString &msg) const
{
  if (LogFilter::eval(type, msg))
    return _expressionTree->eval(msg);
  return false;
}

/** Helper method that inserts string into list and clears it. */
void
addToQueue(QList<QString>& queue, QString& token)
{
  if (!token.isEmpty()) {
    queue.append(token);
    token.clear();
  }
}

/** Helper method used to parse input string and create regular expression.
 * It uses 'Shunting-yard algorithm' to build reverse polish notation of
 * the input expression. This output is then converted to expression tree
 * for evaluation.
 */
void
LogFilterSearchTerm::parseString(const QString& input)
{
  QStack<QString> stack;      /* Stack for operators */
  QList<QString> outputQueue; /* RPN output */
  QString pattern = input;
  pattern.replace("OR","|");
  pattern.replace("AND","&");
  pattern.replace("NOT","!");

  /* Step 1: create RPN of input expression. */
  QString token;
  foreach (const QChar& c, pattern) {
    if (c.isSpace())
      continue;
    if (c=='!' || c=='&' || c=='|') {
      addToQueue(outputQueue, token);
      /* Checking operator precedence with op on top of the stack. */
      while (!stack.isEmpty()) {
        QString op = stack.top();
        if (c=='&' || c=='|') {
          if (op=="!") {
            addToQueue(outputQueue, op);
            stack.pop();
          } else {
            break;
          }
        } else {
          break;
        }
      }
      stack.push(c);
    } else if (c=='(') {
      addToQueue(outputQueue, token);
      stack.push(c);
    } else if (c==')') {
      addToQueue(outputQueue, token);
      while (!stack.isEmpty()) {
        QString tmp = stack.pop();
        if (tmp=="(") {
          break;
        } else {
          addToQueue(outputQueue, tmp);
        }
      }
    } else {
      token += c;
    }
  }
  addToQueue(outputQueue, token);
  while (!stack.isEmpty()) {
    QString top = stack.pop();
    addToQueue(outputQueue, top);
  }

  /* Step 2: convert RPN expression to binary tree. */
  QStack<ExpTree*> tree;
  foreach (QString s, outputQueue) {
    if (s == "!") {
      if (tree.isEmpty()) {
        break;
      }
      ExpTree * top = tree.top();
      top->_invert = !top->_invert;
    } else if (s == "&" || s == "|") {
      if (tree.count() < 2) {
        qDeleteAll(tree);
        tree.clear();
        break;
      }
      ExpTree * node = new ExpTree(s);
      node->_left = tree.pop();
      node->_right = tree.pop();
      tree.push(node);
    } else {
      tree.push(new ExpTree(s));
    }
  }
  if (_expressionTree)
    delete _expressionTree;
  /* If everything is ok, there should be only one item left on stack */
  if (tree.count() == 1) {
    _expressionTree = tree.top();
  } else {
    qDeleteAll(tree);
    _expressionTree = new ExpTree();
  }
}
