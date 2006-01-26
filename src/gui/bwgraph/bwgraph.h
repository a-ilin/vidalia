
#ifndef _BWGRAPH_H
#define _BWGRAPH_H

#include "ui_bwgraph.h"
#include "../../config/vidaliasettings.h"

class BandwidthGraph : public QDialog
{
  Q_OBJECT

public:
  /** Default constructor **/
  BandwidthGraph(QWidget *parent = 0);
  /** Default destructor **/
  ~BandwidthGraph();

private:
  /** A VidaliaSettings object that handles getting/saving settings **/
  VidaliaSettings* _settings;
  
  /** Qt Designer generated object **/
  Ui::BandwidthGraph ui;
};

#endif
