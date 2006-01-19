
#ifndef _FILTERDIALOG_H
#define _FILTERDIALOG_H

#include "ui_filterdialog.h"
#include "../../config/vidaliasettings.h"
#include "../../config/messagetypes.h"

class FilterDialog : public QDialog
{
  Q_OBJECT

public:
  FilterDialog(VidaliaSettings *settings, QWidget *parent = 0);
  void accept();

private:
  Ui::FilterDialog ui;
  VidaliaSettings *_settings;
};

#endif
