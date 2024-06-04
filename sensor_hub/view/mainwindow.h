#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QListWidget>
#include <QLineEdit>
#include <QLabel>
#include <QPoint>
#include <QChartView>
#include <QLineSeries>
#include "sensor_simulation.h"
#include "load_sensors.h"
#include "sensor_operations.h"
#include "sensor_data_handler.h"
#include "sensor_simulation_manager.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void filterSensors(const QString &text);
    void showContextMenu(const QPoint &);
    void handleNewSensorData(int sensorId, double time, double value);

private:
    QHBoxLayout *layout;
    QLineEdit searchBox;
    QListWidget listWidget;
    QLabel detailsLabel;
    QLabel currentValueLabel;
    QPushButton *addButton;
    QPushButton *removeButton;
    QPushButton *startSimulationButton;
    QPushButton *stopSimulationButton;
    QChartView *chartView;
    QVBoxLayout *rightLayout;
    QString getAxisLabel(const std::string &sensorType);
    SensorSimulation *sensorSimulation;
    SensorSimulationManager *sensorSimulationManager;
};

#endif // MAINWINDOW_H
