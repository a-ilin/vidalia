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
** \file html.cpp
** \brief HTML formatting functions
*/

#include "html.h"


/** Wraps a string in "<p>" tags, converts "\n" to "<br/>" and converts "\n\n"
 * to a new paragraph. */
QString
p(QString str)
{
  str = "<p>" + str + "</p>";
  str.replace("\n\n", "</p><p>");
  str.replace("\n", "<br/>");
  return str;
}

/** Wraps a string in "<i>" tags. */
QString
i(QString str)
{
  return QString("<i>%1</i>").arg(str);
}

/** Wraps a string in "<b>" tags. */
QString
b(QString str)
{
  return QString("<b>%1</b>").arg(str);
}

/** Wraps a string in "<tr>" tags. */
QString
trow(QString str)
{
  return QString("<tr>%1</tr>").arg(str);
}

/** Wraps a string in "<td>" tags. */
QString
tcol(QString str)
{
  return QString("<td>%1</td>").arg(str);
}

/** Wraps a string in "<th>" tags. */
QString
thead(QString str)
{
  return QString("<th>%1</th>").arg(str);
}

/** Escapes "<" and ">" characters in the given string. */
QString
escape(QString str)
{
  str.replace("<", "&lt;");
  str.replace(">", "&gt;");
  return str;
}

