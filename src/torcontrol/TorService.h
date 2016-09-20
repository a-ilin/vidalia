/*
**  This file is part of Vidalia, and is subject to the license terms in the
**  LICENSE file, found in the top level directory of this distribution. If
**  you did not receive the LICENSE file with this file, you may obtain it
**  from the Vidalia source package distributed by the Vidalia Project at
**  http://www.torproject.org/projects/vidalia.html. No part of Vidalia,
**  including this file, may be copied, modified, propagated, or distributed
**  except according to the terms described in the LICENSE file.
*/

/*
** \file torservice.h
** \brief Starts, stops, installs, and uninstalls a Tor service (Win32).
*/

#ifndef _TORSERVICE_H
#define _TORSERVICE_H

#include <QObject>
#include <QProcess>
#include <QScopedPointer>

class TorServicePrivate;

class TorService : public QObject
{
  Q_OBJECT

public:
  /** Returns if services are supported. */
  static bool isSupported();

  /** Default ctor. */
  TorService(QObject* parent = 0);
  /** Default dtor. */
  ~TorService();

  /** Returns true if the Tor service is installed. */
  bool isInstalled();
  /** Returns true if the Tor service is running. */
  bool isRunning();
  /** Starts the Tor service. Emits started on success. */
  void start();
  /** Stops the Tor service. Emits finished on success. */
  bool stop();
  /** Returns the exit code of the last Tor service that finished. */
  int exitCode();
  /** Returns the exit status of the last Tor service that finished. */
  QProcess::ExitStatus exitStatus();
  /** Installs the Tor service. */
  bool install(const QString &torPath, const QString &torrc,
               quint16 controlPort);
  /** Removes the Tor service. */
  bool remove();

signals:
  /** Called when the service gets started. */
  void started();
  /** Called when the service gets stopped. */
  void finished(int exitCode, QProcess::ExitStatus);
  /** Called when there is an error in starting the service. */
  void startFailed(QString error);

private:
  /** Pointer to a private class. */
  QScopedPointer<TorServicePrivate> d_ptr;
};

#endif

