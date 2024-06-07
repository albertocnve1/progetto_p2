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
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include "functions/sensor_simulation.h"
#include "functions/load_sensors.h"
#include "functions/sensor_operations.h"
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
    void updateSensorDetails();


private slots:
    void filterSensors(const QString &text);
    void showContextMenu(const QPoint &);
    void handleNewSensorData(int sensorId, double time, double value);
    void addNewSensor();
    void importSensor();
    void editSensor();
    void deleteSensor();

private:
    QHBoxLayout *layout;
    QLineEdit *searchBox;
    QListWidget *listWidget;
    QLabel *detailsLabel;
    QLabel *currentValueLabel;
    QPushButton *addButton;
    QPushButton *removeButton;
    QPushButton *startSimulationButton;
    QPushButton *stopSimulationButton;
    QChartView *chartView;
    QVBoxLayout *rightLayout;
    QString getAxisLabel(const std::string &sensorType);
    SensorSimulation *sensorSimulation;
    SensorSimulationManager *sensorSimulationManager;
    QMenuBar *menuBar;
    QMenu *fileMenu;
    QAction *newSensorAction;
    QAction *importSensorAction;
    QAction *editSensorAction;
    QAction *deleteSensorAction;
};

#endif // MAINWINDOW_H
