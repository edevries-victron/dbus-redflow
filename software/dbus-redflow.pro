# Application version and revision
VERSION = 0.4.1
REVISION = $$system(git --git-dir $$PWD/../.git --work-tree $$PWD describe --always --dirty --tags)

# Create a include file with VERION / REVISION
version_rule.target = $$OUT_PWD/version.h
version_rule.commands = @echo \"updating file $$revtarget.target\"; \
    printf \"/* generated file (do not edit) */\\n \
    $${LITERAL_HASH}ifndef VERSION_H\\n \
    $${LITERAL_HASH}define VERSION_H\\n \
    $${LITERAL_HASH}define VERSION \\\"$${VERSION}\\\"\\n \
    $${LITERAL_HASH}define REVISION \\\"$${REVISION}\\\"\\n \
    $${LITERAL_HASH}endif\" > $$version_rule.target
version_rule.depends = FORCE
QMAKE_DISTCLEAN += $$version_rule.target

QMAKE_EXTRA_TARGETS += version_rule
PRE_TARGETDEPS += $$OUT_PWD/version.h

# suppress the mangling of va_arg has changed for gcc 4.4
QMAKE_CXXFLAGS += -Wno-psabi

# these warnings appear when compiling with QT4.8.3-debug. Problem appears to be
# solved in newer QT versions.
QMAKE_CXXFLAGS += -Wno-unused-local-typedefs

# Add more folders to ship with the application here
unix {
    bindir = $$(bindir)
    DESTDIR = $$(DESTDIR)
    isEmpty(bindir) {
        bindir = /usr/local/bin
    }
    INSTALLS += target
    target.path = $${DESTDIR}$${bindir}
}

QT += core dbus
QT -= gui

TARGET = dbus-redflow
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

include(ext/qslog/QsLog.pri)

INCLUDEPATH += \
    ext/qslog \
    ext/velib/inc \
    ext/velib/inc/velib/platform \
    src

SOURCES += \
    ext/velib/src/qt/v_busitem.cpp \
    ext/velib/src/qt/v_busitems.cpp \
    ext/velib/src/qt/v_busitem_adaptor.cpp \
    ext/velib/src/qt/v_busitem_private_cons.cpp \
    ext/velib/src/qt/v_busitem_private_prod.cpp \
    ext/velib/src/qt/v_busitem_proxy.cpp \
    ext/velib/src/plt/serial.c \
    ext/velib/src/plt/posix_serial.c \
    ext/velib/src/plt/posix_ctx.c \
    ext/velib/src/types/ve_variant.c \
    src/main.cpp \
    src/dbus_bridge.cpp \
    src/modbus_rtu.cpp \
    src/v_bus_node.cpp \
    src/crc16.cpp \
    src/battery_controller_updater.cpp \
    src/battery_controller_bridge.cpp \
    src/battery_controller.cpp \
    src/dbus_redflow.cpp \
    src/battery_summary.cpp \
    src/abstract_monitor_service.cpp \
    src/device_scanner.cpp \
    src/battery_summary_bridge.cpp

HEADERS += \
    ext/velib/src/qt/v_busitem_adaptor.h \
    ext/velib/src/qt/v_busitem_private_cons.h \
    ext/velib/src/qt/v_busitem_private_prod.h \
    ext/velib/src/qt/v_busitem_private.h \
    ext/velib/src/qt/v_busitem_proxy.h \
    ext/velib/inc/velib/qt/v_busitem.h \
    ext/velib/inc/velib/qt/v_busitems.h \
    ext/velib/inc/velib/platform/serial.h \
    src/dbus_bridge.h \
    src/defines.h \
    src/modbus_rtu.h \
    src/v_bus_node.h \
    src/crc16.h \
    src/velib/velib_config_app.h \
    src/dbus_redflow.h \
    src/battery_controller_bridge.h \
    src/battery_controller.h \
    src/battery_controller_updater.h \
    src/battery_summary.h \
    src/abstract_monitor_service.h \
    src/device_scanner.h \
    src/battery_summary_bridge.h
