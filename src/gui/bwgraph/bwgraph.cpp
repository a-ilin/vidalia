
#include "bwgraph.h"

/** Default constructor **/
BandwidthGraph::BandwidthGraph(QWidget *parent)
: QDialog(parent)
{
  /* Setup Designer generated form */
  ui.setupUi(this);

  /* Create Bandwidth Graph related QObjects */
  _settings = new VidaliaSettings();
}

/** Default destructor **/
BandwidthGraph::~BandwidthGraph()
{
  if (_settings) {
    delete _settings;
  }
}
