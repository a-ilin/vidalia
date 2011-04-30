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
** \file RouterDescriptorView.cpp
** \brief Formats and displays a router descriptor as HTML
*/

#include "RouterDescriptorView.h"
#include "Vidalia.h"

#include "html.h"
#include "stringutil.h"

#include <QMenu>
#include <QIcon>
#include <QTextCursor>
#include <QClipboard>
#include <QShortcut>
#include <QTextDocumentFragment>

#define IMG_COPY      ":/images/22x22/edit-copy.png"


/** Default constructor. */
RouterDescriptorView::RouterDescriptorView(QWidget *parent)
: QTextEdit(parent)
{
  /* Steal QTextEdit's default "Copy" shortcut, since we want to do some
   * tweaking of the selected text before putting it on the clipboard. */
  QShortcut *shortcut = new QShortcut(QKeySequence::Copy, this,
                                      SLOT(copySelectedText()));
  Q_UNUSED(shortcut);
}

/** Displays a context menu for the user when they right-click on the
 * widget. */
void
RouterDescriptorView::contextMenuEvent(QContextMenuEvent *event)
{
  QMenu *menu = new QMenu();

  QAction *copyAction = new QAction(QIcon(IMG_COPY), tr("Copy"), menu);
  copyAction->setShortcut(QKeySequence::Copy);
  connect(copyAction, SIGNAL(triggered()), this, SLOT(copySelectedText()));

  if (textCursor().selectedText().isEmpty())
    copyAction->setEnabled(false);

  menu->addAction(copyAction);
  menu->exec(event->globalPos());
  delete menu;
}

/** Copies any selected text to the clipboard. */
void
RouterDescriptorView::copySelectedText()
{ 
  QString selectedText = textCursor().selection().toPlainText();
  selectedText.replace(":\n", ": ");
  vApp->clipboard()->setText(selectedText);
}

/** Adjusts the displayed uptime to include time since the router's descriptor
 * was last published. */
quint64
RouterDescriptorView::adjustUptime(quint64 uptime, QDateTime published)
{
  QDateTime now = QDateTime::currentDateTime().toUTC();
  
  if (now < published) {
    return uptime;
  }
  return (uptime + (now.toTime_t() - published.toTime_t()));
}

/** Displays all router descriptors in the given list. */
void
RouterDescriptorView::display(QList<RouterDescriptor> rdlist)
{
  RouterDescriptor rd;
  QString html = "<html><body>";
  
  for (int r = 0; r < rdlist.size(); r++) { 
    rd = rdlist.at(r);
    if (rd.isEmpty())
      continue;
    
    /* Router name and status */
    html.append(p(b(rd.name()) + " (" + i(rd.status()) + ")"));

    /* IP and platform */
    html.append("<table>");
    
    /* If we have location information, show that first. */
    if (!rd.location().isEmpty()) {
      html.append(trow(tcol(b(tr("Location:"))) + tcol(rd.location())));
    }
    
    /* Add the IP address and router platform information */
    html.append(trow(tcol(b(tr("IP Address:"))) + tcol(rd.ip().toString())));
    html.append(trow(tcol(b(tr("Platform:")))   + tcol(rd.platform())));

    /* If the router is online, then show the uptime and bandwidth stats. */
    if (!rd.offline()) {
      html.append(trow(tcol(b(tr("Bandwidth:")))  + 
                       tcol(string_format_bandwidth(rd.observedBandwidth()))));
      html.append(trow(tcol(b(tr("Uptime:")))   + 
                       tcol(string_format_uptime(
                              adjustUptime(rd.uptime(), rd.published())))));
    }

    /* Date the router was published */
    html.append(trow(tcol(b(tr("Last Updated:")))  +
                     tcol(string_format_datetime(rd.published()) + " GMT")));

    html.append("</table>");

    /* If there are multiple descriptors, and this isn't is the last one 
     * then separate them with a short horizontal line. */
    if (r+1 != rdlist.size()) {
      html.append("<center><hr width=\"50%\"/></center>");
    }
  }
  html.append("</body></html>");
  setHtml(html); 
}

/** Displays the given router descriptor. */
void
RouterDescriptorView::display(RouterDescriptor rd)
{
  display(QList<RouterDescriptor>() << rd);
}

