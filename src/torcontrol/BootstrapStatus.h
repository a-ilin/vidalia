/*
**  This file is part of Vidalia, and is subject to the license terms in the
**  LICENSE file, found in the top level directory of this distribution. If you
**  did not receive the LICENSE file with this file, you may obtain it from the
**  Vidalia source package distributed by the Vidalia Project at
**  http://www.torproject.org/projects/vidalia.html. No part of Vidalia, 
**  including this file, may be copied, modified, propagated, or distributed 
**  except according to the terms described in the LICENSE file.
*/

/* 
** \file BootstrapStatus.h
** \brief Describes the Tor software's current bootstrap status
*/

#ifndef _BOOTSTRAPSTATUS_H
#define _BOOTSTRAPSTATUS_H

#include "tcglobal.h"

#include <QString>
#include <QMetaType>


class BootstrapStatus
{
public:
  /** Actions the Tor software might recommend controllers take in response to
   * a bootstrap status problem event. */
  enum Recommendation {
    UnrecognizedRecommendation,
    RecommendIgnore,
    RecommendWarn
  };

  /** Default constructor. */
  BootstrapStatus();

  /** Constructor. */
  BootstrapStatus(tc::Severity severity,
                  const QString &status, int percentComplete,
                  const QString &description,
                  const QString &warning = QString(),
                  tc::ConnectionStatusReason reason = tc::UnrecognizedReason,
                  Recommendation action = UnrecognizedRecommendation);

  /** Returns the severity of this bootstrap status event. */
  tc::Severity severity() const { return _severity; }

  /** Returns the BootstrapStatus enum value indicated by this bootstrap
   * status event. */
  QString status() const { return _status; }

  /** Returns an integer between 0 and 100 representing an estimate of how
   * much of Tor's bootstrapping process it has completed. */
  int percentComplete() const { return _percentComplete; }

  /** Returns a description of Tor's current bootstrapping status. */
  QString description() const { return _description; }
  
  /** Returns a description of the most recent error Tor encountered while
   * attempting to bootstrap, if this event's severity is 'warn'. Otherwise,
   * this returns a default-constructed QString. */
  QString warning() const { return _warning; }

  /** Returns a ConnectionStatusReason enum value describing the most recent
   * error Tor encountered while attempting to bootstrap, if this event's
   * severity is 'warn'. Otherwise, this simply returns
   * tc::UnrecognizedReason. */
  tc::ConnectionStatusReason reason() const { return _reason; }

  /** Returns the action that the Tor software recommended be taken in
   * response to this bootstrap status event. */ 
  Recommendation recommendedAction() const { return _action; }

  /** Returns true if this object represents a valid bootstrap status
   * phase. */
  bool isValid() const;

  /** Converts a string RECOMMENDATION value to a RecommendAction enum
   * value. */
  static Recommendation actionFromString(const QString &str);

private:
  /** Severity of the current bootstrap status.
   * \sa severity
   */
  tc::Severity _severity;

  /** Current bootstrapping status value.
   * \sa status
   */ 
  QString _status;
  
  /** Approximate percentage of Tor's bootstrapping process that is complete.
   * \sa percentComplete
   */
  int _percentComplete;
  
  /** Description of Tor's current bootstrapping status.
   * \sa description
   */
  QString _description;
  
  /** Description of the most recent error Tor encountered while attempting to
   * bootstrap. 
   * \sa warning
   */
  QString _warning;

  /** ConnectionStatusReason enum value describing the most recent error Tor
   * encountered while attempting to bootstrap.
   * \sa reason
   */
  tc::ConnectionStatusReason _reason;

  /** Recommendation enum value describing Tor's suggested response to this
   * bootstrap status event.
   * \sa recommendedAction
   */
  Recommendation _action;
};

Q_DECLARE_METATYPE(BootstrapStatus);

#endif

