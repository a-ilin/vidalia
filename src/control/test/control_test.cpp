
#include <QtDebug>

#include "../torcontrol.cpp"

int
main(int argc, char *argv[])
{
  TorControl control;
  QString errmsg;

  /* Connect the control socket */
  if (!control.connect(&errmsg)) {
    qDebug() << "Could not connect to Tor:" << errmsg;
    return -1;
  } else {
    qDebug( "Connected!");
  }

  /* Disconnect the control socket */
  control.disconnect();
  
  return 0;
}

