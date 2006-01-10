TEMPLATE = app
CONFIG += qt console
SOURCES += control_test.cpp

!win32 {
  CONFIG += debug
} else {
  CONFIG += release
}

QT += network

include(../control.pri)
include(../../config/config.pri)
