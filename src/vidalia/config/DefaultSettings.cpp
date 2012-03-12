#include "DefaultSettings.h"
#include "Vidalia.h"

DefaultSettings::DefaultSettings()
{
  _settings = new QSettings(Vidalia::dataDirectory() + "/vidalia.defaults.conf",
                            QSettings::InitFormat);
}

DefaultSettings::~DefaultSettings()
{
  delete _settings;
}

QVariant
DefaultSettings::getDefault(const QString &key,
                            const QVariant &defaultVal = QVariant()) const
{
  return _settings->value(key, defaultVal);
}
