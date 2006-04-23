
#ifndef _HTML_H
#define _HTML_H

#include <QString>

/** Wraps a string in "<p>" tags, converts "\n" to "<br/>" and converts "\n\n"
 * to a new paragraph. */
QString p(QString str);

/** Wraps a string in "<i>" tags. */
QString i(QString str);

/** Wraps a string in "<b>" tags. */
QString b(QString str);

/** Wraps a string in "<tr>" tags. */
QString trow(QString str);

/** Wraps a string in "<td>" tags. */
QString tcol(QString str);

/** Wraps a string in "<th>" tags. */
QString thead(QString str);

/** Escapes "<" and ">" characters in a string, for html. */
QString escape(QString str);

#endif

