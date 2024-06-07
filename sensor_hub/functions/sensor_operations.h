#ifndef SENSOR_OPERATIONS_H
#define SENSOR_OPERATIONS_H

#include <QListWidget>
#include <QInputDialog>
#include <QMessageBox>
#include <QFileDialog>
#include <QDir>
#include <QLineEdit>
#include <QChartView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <unordered_map>
#include "sensors/sensor.h"

class SensorOperations
{
public:
    static void editSensor(QListWidget *, QWidget *);
    static void exportSensor(QListWidget *, QWidget *);
    static void addSensor(QListWidget *, QWidget *);
    static void newSensor(QListWidget *, QWidget *);
    static void deleteSensor(QListWidget *, QWidget *);
private:
    static std::vector<std::pair<double, double>> readChartDataFromFile(QTextStream &);

};

#endif // SENSOR_OPERATIONS_H
