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

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void filterSensors(const QString &text);
    void addSensor();
    void newSensor();
    void deleteSensor();
    void showContextMenu(const QPoint &);
    void editSensor();
    void exportSensor();
    void displaySensorDetails();
    void startSimulation();
    void stopSimulation();  // Dichiarazione dello slot per interrompere la simulazione
    void handleNewSensorData(int sensorId, double time, double value);

private:
    QHBoxLayout *layout;
    QLineEdit searchBox;
    QListWidget listWidget;
    QLabel detailsLabel;
    QPushButton *addButton;
    QPushButton *removeButton;
    QChartView *chartView;
    QPushButton *startSimulationButton;
    QPushButton *stopSimulationButton;  // Dichiarazione del pulsante per interrompere la simulazione
    QVBoxLayout *rightLayout;
    SensorSimulation *sensorSimulation;
    QString getAxisLabel(const std::string &sensorType);
    QLabel currentValueLabel; // Nuovo QLabel per il valore della misurazione attuale

};

#endif // MAINWINDOW_H
