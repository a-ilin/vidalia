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
** \file html.h
** \brief HTML formatting functions
*/

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

