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
** \file HelpBrowser.h
** \brief Displays a list of help topics and content
*/

#ifndef _HELPBROWSER_H
#define _HELPBROWSER_H

#include "ui_HelpBrowser.h"
#include "VidaliaWindow.h"

#include <QMainWindow>
#include <QCloseEvent>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNodeList>
#include <QTreeWidgetItem>
#include <QTextBrowser>
#include <QTextCursor>


class HelpBrowser : public VidaliaWindow
{
  Q_OBJECT

public:
  /** Default constructor **/
  HelpBrowser(QWidget *parent = 0);

public slots:
  /** Overrides the default QWidget::show() */
  void showWindow(QString topic = QString());

protected:
  /** Called when the user changes the UI translation. */
  virtual void retranslateUi();

private slots:
  /** Called when the user clicks "Find Next" */
  void findNext();
  /** Called when the user clicks "Find Previous" */
  void findPrev();
  /** Called when the user starts a search */
  void search();
  /** Called when the user selects a different item in the contents tree */
  void contentsItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *prev);
  /** Called when the user selects a different item in the search tree */
  void searchItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *prev);

private:
  /** Returns the language in which help topics should appear, or English
   * ("en") if no translated help files exist for the current GUI language. */
  QString language();
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
  /** Searches the current page for the phrase in the Find box */
  void find(bool forward);
  /** Creates a new item to be placed in the topic tree. */
  QTreeWidgetItem* createTopicTreeItem(const QDomElement &topicElement,
                                       QTreeWidgetItem *parent);
  /** Called when the user selects a different item in the tree. */
  void currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *prev);
  /** Finds a topic in the topic tree. */
  QTreeWidgetItem* findTopicItem(QTreeWidgetItem *startItem, QString topic);
  /** Shows the help browser and finds a specific a topic in the browser. */
  void showTopic(QString topic);

  /** List of DOM elements representing topics. */
  QList<QDomElement> _elementList;
  /** Last phrase used for 'Find' */
  QString _lastFind;
  /** Last phrase searched on */
  QString _lastSearch;
  /** Indicates if phrase was previously found on current page */
  bool _foundBefore;

  /** Qt Designer generated QObject */
  Ui::HelpBrowser ui;
};

#endif

