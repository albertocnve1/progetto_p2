#ifndef SENSOR_SIMULATION_H
#define SENSOR_SIMULATION_H

#include <QObject>
#include <QTimer>
#include <unordered_map>
#include "sensors/sensor.h"

class SensorSimulation : public QObject
{
    Q_OBJECT

public:
    explicit SensorSimulation(QObject * = nullptr);
    void simulateSensor(unsigned int);
    void stopSimulation();

signals:
    void newSensorData(int, double, double);

private slots:
    void generateSensorData();

private:
    std::unordered_map<unsigned int, sensor *> sensors;
    QTimer *simulationTimer;
    unsigned int currentSensorId;
    qint64 startTime;
};

#endif // SENSOR_SIMULATION_H
