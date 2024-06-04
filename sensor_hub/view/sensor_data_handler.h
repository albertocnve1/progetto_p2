#ifndef SENSOR_DATA_HANDLER_H
#define SENSOR_DATA_HANDLER_H

#include <QChartView>
#include <QLineSeries>
#include <QMessageBox>
#include <unordered_map>
#include <QLabel>
#include <QValueAxis>
#include "sensors/sensor.h"
#include "sensors/dust_sensor.h"
#include "sensors/temperature_sensor.h"
#include "sensors/humidity_sensor.h"

class SensorDataHandler
{
public:
    static void handleNewSensorData(int sensorId, double time, double value, QChartView *chartView, QLabel *currentValueLabel, QWidget *parent);
};

#endif // SENSOR_DATA_HANDLER_H
