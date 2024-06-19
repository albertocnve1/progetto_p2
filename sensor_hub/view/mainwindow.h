#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QHBoxLayout>
#include <QPushButton>
#include <QMenuBar>

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
    void filterSensors(const QString &);
    void showContextMenu(const QPoint &);
    void handleNewSensorData(int, double, double);
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
    QString getAxisLabel(const std::string &);
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
