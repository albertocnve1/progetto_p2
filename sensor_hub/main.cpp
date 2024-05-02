#include "mainwindow.h"

#include "sensor.h"
#include "temperature_sensor.h"
#include "dust_sensor.h"
#include "humidity_sensor.h"
#include "load_sensors.h"
#include "mainwindow.h"

#include <QApplication>
#include <QVBoxLayout>
#include <QListWidget>
#include <QLabel>



int main(int argc, char *argv[])
{
    loadSensors();
    sensor* s1 = temperature_sensor::create("termometro", 0.3);
    QApplication app(argc, argv);
    MainWindow window;
    window.show();
    return app.exec();
}
