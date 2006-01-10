#include <QTcpSocket>

class ControlConnection : public QTcpSocket
{
public:
  ControlConnection();
  ~ControlConnection();

  bool connect(QHostAddress addr, quint16 port, QString *errmsg = 0);
  void disconnect();
};
