
#ifndef _VIDALIAWINDOW_H
#define _VIDALIAWINDOW_H

#include <QString>
#include <QWidget>
#include <QVariant>
#include <QMainWindow>
#include <config/vidaliasettings.h>


class VidaliaWindow : public QMainWindow
{
  Q_OBJECT

public:
  /** Default constructor. */
  VidaliaWindow(QString name, QWidget *parent = 0, Qt::WFlags flags = 0);
  /** Destructor. */
  ~VidaliaWindow();

  /** Saves the size and location of the window. */
  void saveWindowState();
  /** Restores the last size and location of the window. */
  void restoreWindowState();

  /** Gets the saved value of a property associated with this window object.
   * If no property value was saved, the default value is returned. */
  QVariant getWindowProperty(QString name, QVariant defaultValue);
  /** Saves a value associated with a property name for this window object. */
  void saveWindowProperty(QString name, QVariant value);

public slots:
  /** Overloaded QWidget::close() method. Saves the window state and closes
   * the window. Returns true if the window was closed. */
  bool close();
  /** Overloaded QWidget::show(). */
  void show();

private:
  QString _name;  /**< Name associated with this window. */
  VidaliaSettings* _settings; /**< Object used to store window properties */
};

#endif

