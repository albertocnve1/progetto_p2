#ifndef SENSOR_OPERATIONS_H
#define SENSOR_OPERATIONS_H

#include <QListWidget>
#include <QInputDialog>
#include <QMessageBox>
#include <QFileDialog>
#include <QDir>
#include <unordered_map>
#include "sensor.h"

class SensorOperations
{
public:
    static void editSensor(QListWidget *listWidget, QWidget *parent);
    static void exportSensor(QListWidget *listWidget, QWidget *parent);
};

#endif // SENSOR_OPERATIONS_H
