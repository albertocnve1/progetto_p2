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
        sensor* s = sensors[sensorId];
        if (dynamic_cast<dust_sensor*>(s)) {
            dynamic_cast<dust_sensor*>(s)->clearChartData();
        } else if (dynamic_cast<temperature_sensor*>(s)) {
            dynamic_cast<temperature_sensor*>(s)->clearChartData();
        } else if (dynamic_cast<humidity_sensor*>(s)) {
            dynamic_cast<humidity_sensor*>(s)->clearChartData();
        }
        simulationTimer->start(500); // Genera dati ogni secondo
    }
}

void SensorSimulation::stopSimulation()
{
    simulationTimer->stop();
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
            dynamic_cast<dust_sensor *>(s)->setDustLevel(value);
        }
        else if (dynamic_cast<temperature_sensor *>(s))
        {
            value = QRandomGenerator::global()->bounded(120.0) - 20.0;
            dynamic_cast<temperature_sensor *>(s)->setTemperature(value);
        }
        else if (dynamic_cast<humidity_sensor *>(s))
        {
            value = QRandomGenerator::global()->bounded(100.0);
            dynamic_cast<humidity_sensor *>(s)->setHumidity(value);
        }

        // Limita il valore a due cifre decimali
        value = QString::number(value, 'f', 2).toDouble();

        qint64 timestamp = QDateTime::currentMSecsSinceEpoch();
        double relativeTime = (timestamp - startTime) / 1000.0;

        // Usa il nuovo metodo updateSensorData
        s->updateSensorData(relativeTime, value);

        emit newSensorData(currentSensorId, relativeTime, value);  // Usa il tempo relativo

    }
}
