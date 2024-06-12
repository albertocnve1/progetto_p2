#ifndef SENSOR_DATA_HANDLER_H
#define SENSOR_DATA_HANDLER_H

#include <QChartView>
#include <QLineSeries>
#include <QMessageBox>
#include <QLabel>
#include <QValueAxis>

#include "sensors/dust_sensor.h"
#include "sensors/temperature_sensor.h"
#include "sensors/humidity_sensor.h"

class SensorDataHandler
{
public:
    static void handleNewSensorData(int, double, double, QChartView *, QLabel *, QWidget *);
};

#endif // SENSOR_DATA_HANDLER_H
