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
 * \file helpbrowser.h
 * \version $Id$ 
 */

#ifndef _HELPBROWSER_H
#define _HELPBROWSER_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QDomDocument>
#include <QDomElement>
#include <QTreeWidgetItem>

#include "ui_helpbrowser.h"

class HelpBrowser : public QMainWindow
{
  Q_OBJECT

public:
  /** Default constructor **/
  HelpBrowser(QWidget *parent = 0);
  /** Default destructor **/
  ~HelpBrowser();

public:
  /** Called when the user triggers the close action */
  void close();

protected:
  /** Catches the close event when the user clicks on the X in the titlebar */
  void closeEvent(QCloseEvent *event);

private slots:
  /** Called when the user selects a different item in the topic tree. */
  void currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *prev);
  /** Called when the availability of backward() changes and updates the UI. */
  void backwardAvailable(bool available);
  /** Called when the availability of forward() changes and updates the UI. */
  void forwardAvailable(bool available);
  
private:
  /** Load the contents of the help topics tree from the specified XML file. */
  void loadContentsFromXml(QString xmlFile);
  /** Load the contents of the help topics tree from the given DOM document. */
  bool loadContents(const QDomDocument *document, QString &errorString);
  /** Parse a Topic element and handle all its children. */
  void parseHelpTopic(const QDomElement &element, QTreeWidgetItem *parent);
  /** Returns true if the given Topic element has the necessary attributes. */
  bool isValidTopicElement(const QDomElement &topicElement);
  /** Builds a resource path to an html file associated with a help topic. */
  QString getResourcePath(const QDomElement &topicElement);
  /** Creates a new item to be placed in the topic tree. */
  QTreeWidgetItem* createTopicTreeItem(const QDomElement &topicElement,
                                       QTreeWidgetItem *parent);

  
  /** Qt Designer generated QObject */
  Ui::HelpBrowser ui;
};

#endif
  
