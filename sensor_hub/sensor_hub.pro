QT       += core gui
QT       += charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    sensors/dust_sensor.cpp \
    sensors/humidity_sensor.cpp \
    functions/load_sensors.cpp \
    main.cpp \
    view/mainwindow.cpp \
    sensors/sensor.cpp \
    view/sensor_data_handler.cpp \
    functions/sensor_operations.cpp \
    functions/sensor_simulation.cpp \
    view/sensor_simulation_manager.cpp \
    functions/sensordialog.cpp \
    sensors/temperature_sensor.cpp

HEADERS += \
    sensors/dust_sensor.h \
    sensors/humidity_sensor.h \
    functions/load_sensors.h \
    view/mainwindow.h \
    sensors/sensor.h \
    view/sensor_data_handler.h \
    functions/sensor_operations.h \
    functions/sensor_simulation.h \
    view/sensor_simulation_manager.h \
    functions/sensordialog.h \
    sensors/temperature_sensor.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc
