
#include "controlconnection.h"
#include "torprocess.h"

class TorControl
{
private:
  ControlConnection _controlConn;
  TorProcess _torProcess;
  
public:
  /** Default constructor */
  TorControl();
  
  /** Default destructor */
  ~TorControl();

  /** Start the Tor process */
  bool startTor(QString *errmsg = 0);

  /** Stop the Tor process */
  void stopTor();

  /** Connect to Tor's control socket */
  bool connect(QString *errmsg = 0);

  /** Disconnect from Tor's control socket */
  void disconnect();
};
