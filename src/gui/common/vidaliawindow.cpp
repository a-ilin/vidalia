
#include <QPoint>
#include <QSize>
#include "vidaliawindow.h"


/** Default constructor. */
VidaliaWindow::VidaliaWindow(QString name, QWidget *parent, Qt::WFlags flags)
 : QMainWindow(parent, flags)
{
  _name     = name;
  _settings = new VidaliaSettings();
}

/** Destructor. */
VidaliaWindow::~VidaliaWindow()
{
  saveWindowState();
  delete _settings;
}

/** Saves the size and location of the window. */
void
VidaliaWindow::saveWindowState()
{
  saveWindowProperty("Size", size());
  saveWindowProperty("Position", pos());
}

/** Restores the last size and location of the window. */
void
VidaliaWindow::restoreWindowState()
{
  /* Restore the window size. */
  QSize size = getWindowProperty("Size", QSize()).toSize();
  if (!size.isEmpty()) {
    resize(size);
  }

  /* Restore the window position. */
  QPoint pos = getWindowProperty("Position", QPoint()).toPoint();
  if (!pos.isNull()) {
    move(pos);
  }
}

/** Gets the saved value of a property associated with this window object.
 * If no property value was saved, the default value is returned. */
QVariant
VidaliaWindow::getWindowProperty(QString prop, QVariant defaultValue)
{
  QString key = _name + "/" + prop;
  return _settings->value(key, defaultValue);
}

/** Saves a value associated with a property name for this window object. */
void
VidaliaWindow::saveWindowProperty(QString prop, QVariant value)
{
  QString key = _name + "/" + prop;
  _settings->setValue(key, value);
}

/** Overloaded QWidget::close() method. Saves the window state and closes the
 * window. Returns true if the window was closed. */
bool
VidaliaWindow::close()
{
  saveWindowState();
  return QMainWindow::close();
}

/** Overloaded QWidget::show() */
void
VidaliaWindow::show()
{
  static bool previouslyShown = false;
  
  /* If this is the first time this window is shown, restore its window
   * position and size. */
  if (!previouslyShown) {
    restoreWindowState();
    previouslyShown = true;
  }

  /* Bring the window to the top, if it's already open. Otherwise, make the
   * window visible. */
  if (!this->isVisible()) {
    QMainWindow::show();
  } else {
    activateWindow();
    setWindowState(windowState() & ~Qt::WindowMinimized | Qt::WindowActive);
    raise();
  }
}

