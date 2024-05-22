#include "sensor_simulation.h"
#include <QRandomGenerator>
#include <QDateTime>

SensorSimulation::SensorSimulation(QObject *parent) : QObject(parent), simulationTimer(new QTimer(this)), startTime(0)
{
    connect(simulationTimer, &QTimer::timeout, this, &SensorSimulation::generateSensorData);
}

void SensorSimulation::simulateSensor(unsigned int sensorId)
{
    currentSensorId = sensorId;
    sensors = sensor::getSensors();
    if (sensors.find(sensorId) != sensors.end())
    {
        startTime = QDateTime::currentMSecsSinceEpoch();
        simulationTimer->start(1000); // Genera dati ogni secondo
    }
}

void SensorSimulation::stopSimulation()
{
    simulationTimer->stop();  // Ferma il timer della simulazione
}

void SensorSimulation::generateSensorData()
{
    if (sensors.find(currentSensorId) != sensors.end())
    {
        sensor *s = sensors[currentSensorId];
        double value = QRandomGenerator::global()->bounded(10.0);
        qint64 timestamp = QDateTime::currentMSecsSinceEpoch();
        emit newSensorData(currentSensorId, (timestamp - startTime) / 1000.0, value);  // Usa il tempo relativo
    }
}
