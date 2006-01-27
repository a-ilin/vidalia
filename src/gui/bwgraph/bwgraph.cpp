
#include "bwgraph.h"

/** Default constructor **/
BandwidthGraph::BandwidthGraph(QWidget *parent, Qt::WFlags f)
: QDialog(parent, f)
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
