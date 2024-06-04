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
    static void editSensor(QListWidget *listWidget, QWidget *parent);
    static void exportSensor(QListWidget *listWidget, QWidget *parent);
    static void addSensor(QListWidget *listWidget, QWidget *parent);
    static void newSensor(QListWidget *listWidget, QWidget *parent);
    static void deleteSensor(QListWidget *listWidget, QWidget *parent);
};

#endif // SENSOR_OPERATIONS_H
