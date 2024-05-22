#include "sensor_simulation.h"
#include "dust_sensor.h"
#include "temperature_sensor.h"
#include "humidity_sensor.h"
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
        double value = 0.0;

        // Usare dynamic_cast per determinare il tipo di sensore e generare valori appropriati
        if (dynamic_cast<dust_sensor *>(s))
        {
            value = QRandomGenerator::global()->bounded(50.0);
        }
        else if (dynamic_cast<temperature_sensor *>(s))
        {
            value = QRandomGenerator::global()->bounded(120.0) - 20.0;
        }
        else if (dynamic_cast<humidity_sensor *>(s))
        {
            value = QRandomGenerator::global()->bounded(100.0);
        }

        qint64 timestamp = QDateTime::currentMSecsSinceEpoch();
        emit newSensorData(currentSensorId, (timestamp - startTime) / 1000.0, value);  // Usa il tempo relativo

        // Debug: stampa del valore generato
        qDebug() << "Generated value for sensor " << currentSensorId << ": " << value;
    }
}
