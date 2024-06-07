#ifndef SENSOR_SIMULATION_MANAGER_H
#define SENSOR_SIMULATION_MANAGER_H

#include <QChartView>
#include <QListWidget>
#include <QLabel>
#include <QMessageBox>
#include <unordered_map>
#include "functions/sensor_simulation.h"
#include "sensors/sensor.h"

class SensorSimulationManager : public QObject
{
    Q_OBJECT

public:
    explicit SensorSimulationManager(QChartView *, QLabel *, QLabel *, QListWidget *, QWidget * = nullptr);

public slots:
    void displaySensorDetails();
    void startSimulation();
    void stopSimulation();

private:
    QChartView *chartView;
    QLabel *detailsLabel;
    QLabel *currentValueLabel;
    QListWidget *listWidget;
    QWidget *parent;
    SensorSimulation *sensorSimulation;
};

#endif // SENSOR_SIMULATION_MANAGER_H
