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
 * \file helpbrowser.cpp
 * \version $Id$ 
 */

#include <QDomDocument>
#include <gui/mainwindow.h>

#include "helpbrowser.h"


#define LEFT_PANE_INDEX     0
#define NO_STRETCH          0
#define MINIMUM_PANE_SIZE   1

/* Names of elements and attributes in the XML file */
#define ELEMENT_CONTENTS        "Contents"
#define ELEMENT_TOPIC           "Topic"
#define ATTRIBUTE_TOPIC_ID      "id"
#define ATTRIBUTE_TOPIC_HTML    "html"
#define ATTRIBUTE_TOPIC_NAME    "name"
#define ATTRIBUTE_TOPIC_SECTION "section"

/* Define two roles used to store data associated with a topic item */
#define ROLE_TOPIC_ID        Qt::UserRole
#define ROLE_TOPIC_QRC_PATH (Qt::UserRole+1)


/** Constuctor. This will probably do more later */
HelpBrowser::HelpBrowser(QWidget *parent)
: QMainWindow(parent)
{
  /* Invoke Qt Designer generated QObject setup routine */
  ui.setupUi(this);

  /* Hide Search frame */
  ui.frmSearch->setHidden(true);
 
  /* Set the splitter pane sizes so that only the txtTopic pane expands
   * and set to arbitrary sizes (the minimum sizes will take effect */
  QList<int> sizes;
  sizes.append(MINIMUM_PANE_SIZE); 
  sizes.append(MINIMUM_PANE_SIZE);
  ui.splitter->setSizes(sizes);
  ui.splitter->setStretchFactor(LEFT_PANE_INDEX, NO_STRETCH);

  /* Load the help topics from XML */
  loadContentsFromXml(":/help/contents.xml");
}

/** Destructor */
HelpBrowser::~HelpBrowser()
{
}

/** Load the contents of the help topics tree from the specified XML file. */
void
HelpBrowser::loadContentsFromXml(QString xmlFile)
{
  QString errorString;
  QFile file(xmlFile);
  QDomDocument document;
  
  /* Load the XML contents into the DOM document */
  if (!document.setContent(&file, true, &errorString)) {
    ui.txtTopic->setPlainText(tr("Error Loading Help Contents: ")+errorString);
    return;
  }
  /* Load the DOM document contents into the tree view */
  if (!loadContents(&document, errorString)) {
    ui.txtTopic->setPlainText(tr("Error Loading Help Contents: ")+errorString);
    return;
  }
}

/** Load the contents of the help topics tree from the given DOM document. */
bool
HelpBrowser::loadContents(const QDomDocument *document, QString &errorString)
{
  /* Grab the root document element and make sure it's the right one */
  QDomElement root = document->documentElement();
  if (root.tagName() != ELEMENT_CONTENTS) {
    errorString = "Supplied XML file is not a valid Contents document.";
    return false;
  }
  
  /* Create the home item */
  QTreeWidgetItem *home = new QTreeWidgetItem();
  home->setText(0, root.attribute(ATTRIBUTE_TOPIC_NAME));
  ui.treeContents->addTopLevelItem(home);
  
  /* Process all top-level help topics */
  QDomElement child = root.firstChildElement(ELEMENT_TOPIC);
  while (!child.isNull()) {
    parseHelpTopic(child, home);
    child = child.nextSiblingElement(ELEMENT_TOPIC);
  }
  return true;
}

/** Parse a Topic element and handle all its children recursively. */
void
HelpBrowser::parseHelpTopic(const QDomElement &topicElement, 
                             QTreeWidgetItem *parent)
{
  /* Check that we have a valid help topic */
  if (isValidTopicElement(topicElement)) {
    /* Create and populate the new topic item in the tree */
    QTreeWidgetItem *topic = new QTreeWidgetItem(parent);
    topic->setText(0, topicElement.attribute(ATTRIBUTE_TOPIC_NAME));
    topic->setData(0, ROLE_TOPIC_ID, topicElement.attribute(ATTRIBUTE_TOPIC_ID));
    topic->setData(0, ROLE_TOPIC_QRC_PATH, getResourcePath(topicElement));

    /* Process all its child elements */
    QDomElement child = topicElement.firstChildElement(ELEMENT_TOPIC);
    while (!child.isNull()) {
      parseHelpTopic(child, topic);
      child = child.nextSiblingElement(ELEMENT_TOPIC);
    }
  }
}

/** Returns true if the given Topic element has the necessary attributes. */
bool
HelpBrowser::isValidTopicElement(const QDomElement &topicElement)
{
  return (topicElement.hasAttribute(ATTRIBUTE_TOPIC_ID) &&
          topicElement.hasAttribute(ATTRIBUTE_TOPIC_NAME) &&
          topicElement.hasAttribute(ATTRIBUTE_TOPIC_HTML));
}

/** Builds a resource path to an html file associated with the given help
 * topic. If the help topic needs an achor, the anchor will be formatted and
 * appended. */
QString
HelpBrowser::getResourcePath(const QDomElement &topicElement)
{
  QString link = "qrc:/help/" + topicElement.attribute(ATTRIBUTE_TOPIC_HTML);
  if (topicElement.hasAttribute(ATTRIBUTE_TOPIC_SECTION)) {
    link += "#" + topicElement.attribute(ATTRIBUTE_TOPIC_SECTION);
  }
  return link;
}

/** Overloads the default close() slot, so we can force the parent to become
 * visible. This only matters on Mac, so we can ensure the correct menubar is
 * displayed. */
void
HelpBrowser::close()
{
  MainWindow *p = (MainWindow *)parent();
  if (p) {
    p->show();
  }
  QMainWindow::close();
}

/** Responds to when the user closes the form via the 'X' */
void
HelpBrowser::closeEvent(QCloseEvent *e)
{
  MainWindow *p = (MainWindow *)parent();
  if (p) {
    p->show();
  }
  e->accept();
}

