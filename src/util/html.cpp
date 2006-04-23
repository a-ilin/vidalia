
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

