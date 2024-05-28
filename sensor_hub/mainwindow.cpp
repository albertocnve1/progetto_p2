#include "mainwindow.h"
#include "dust_sensor.h"
#include "temperature_sensor.h"
#include "humidity_sensor.h"
#include "sensordialog.h"
#include "sensor_operations.h"
#include "sensor_data_handler.h"
#include "sensor_simulation_manager.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QMenu>
#include <QInputDialog>
#include <QRandomGenerator>
#include <QTime>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QChart>
#include <QtCharts/QValueAxis>
#include <unordered_map>

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent), layout(new QHBoxLayout(this)), detailsLabel(""), chartView(new QChartView), startSimulationButton(new QPushButton("Avvia nuova simulazione")), stopSimulationButton(new QPushButton("Interrompi Simulazione"))
{
    // Creazione di un layout orizzontale per i pulsanti
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    QPushButton *addButton = new QPushButton("+");
    QPushButton *removeButton = new QPushButton("-");
    buttonLayout->addWidget(&searchBox);
    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(removeButton);

    listWidget.setContextMenuPolicy(Qt::CustomContextMenu);
    connect(&listWidget, &QListWidget::customContextMenuRequested, this, &MainWindow::showContextMenu);

    // Creazione del menu per il pulsante "+"
    QMenu *addMenu = new QMenu(this);
    QAction *newSensorAction = new QAction(tr("Nuovo sensore"), this);
    QAction *importFromFileAction = new QAction(tr("Importa da file"), this);
    addMenu->addAction(newSensorAction);
    addMenu->addAction(importFromFileAction);
    addButton->setMenu(addMenu);

    // Connessione del pulsante "+" a uno slot per aggiungere un nuovo sensore da GUI
    connect(addButton, &QPushButton::clicked, this, [this]() {
        SensorOperations::addSensor(&listWidget, this);
    });
    connect(newSensorAction, &QAction::triggered, this, [this]() {
        SensorOperations::newSensor(&listWidget, this);
    });
    // Connessione del pulsante "+" a uno slot per importare un sensore da file
    connect(importFromFileAction, &QAction::triggered, this, [this]() {
        SensorOperations::addSensor(&listWidget, this);
    });
    connect(removeButton, &QPushButton::clicked, this, [this]() {
        SensorOperations::deleteSensor(&listWidget, this);
    });

    // Creazione dello spazio per la lista di sensori a sx
    QVBoxLayout *leftLayout = new QVBoxLayout;
    leftLayout->addLayout(buttonLayout);
    leftLayout->addWidget(&listWidget);

    layout->addLayout(leftLayout);

    rightLayout = new QVBoxLayout;
    rightLayout->addWidget(&detailsLabel);
    rightLayout->addWidget(&currentValueLabel); // Aggiungiamo il currentValueLabel al layout
    rightLayout->addWidget(chartView);

    QHBoxLayout *simulationButtonsLayout = new QHBoxLayout; // Layout orizzontale per i pulsanti di simulazione
    simulationButtonsLayout->addWidget(startSimulationButton);
    simulationButtonsLayout->addWidget(stopSimulationButton);

    rightLayout->addLayout(simulationButtonsLayout); // Aggiungi il layout dei pulsanti di simulazione

    layout->addLayout(rightLayout);

    // Connetti la casella di ricerca al filtro
    connect(&searchBox, &QLineEdit::textChanged, this, &MainWindow::filterSensors);

    resize(1000, 500);

    // Inizializzazione della simulazione
    sensorSimulation = new SensorSimulation(this);
    connect(sensorSimulation, &SensorSimulation::newSensorData, this, &MainWindow::handleNewSensorData);

    // Inizializzazione del gestore della simulazione
    sensorSimulationManager = new SensorSimulationManager(chartView, &detailsLabel, &currentValueLabel, &listWidget, this);
    connect(startSimulationButton, &QPushButton::clicked, sensorSimulationManager, &SensorSimulationManager::startSimulation);
    connect(stopSimulationButton, &QPushButton::clicked, sensorSimulationManager, &SensorSimulationManager::stopSimulation);
    connect(&listWidget, &QListWidget::itemSelectionChanged, sensorSimulationManager, &SensorSimulationManager::displaySensorDetails);

    // Ottenimento dell'elenco dei sensori istanziati
    std::unordered_map<unsigned int, sensor *> &sensors = sensor::getSensors();

    // Aggiungi i sensori istanziati all'elenco laterale sx
    for (const auto &pair : sensors)
    {
        sensor *s = pair.second;
        QString sensorInfo = QString::number(s->getID()) + ": " + QString::fromStdString(s->getName());
        listWidget.addItem(sensorInfo);
    }

    // Impostazione delle proporzioni dei widget
    this->layout->setStretchFactor(leftLayout, 1);
    this->layout->setStretchFactor(rightLayout, 3);
}


MainWindow::~MainWindow()
{
    delete sensorSimulation;
    delete sensorSimulationManager;
}

void MainWindow::filterSensors(const QString &text)
{
    for (int i = 0; i < listWidget.count(); ++i)
    {
        QListWidgetItem *item = listWidget.item(i);
        item->setHidden(!item->text().contains(text, Qt::CaseInsensitive));
    }
}

void MainWindow::showContextMenu(const QPoint &pos)
{
    QListWidgetItem *selectedItem = listWidget.currentItem();
    if (!selectedItem)
    {
        QMessageBox::warning(this, tr("Errore"), tr("Nessun sensore selezionato"));
        return;
    }

    QMenu contextMenu(tr("Context menu"), this);

    QAction actionEdit("Modifica", this);
    connect(&actionEdit, &QAction::triggered, this, [this]() {
        SensorOperations::editSensor(&listWidget, this);
    });
    contextMenu.addAction(&actionEdit);

    QAction actionExport("Esporta", this);
    connect(&actionExport, &QAction::triggered, this, [this]() {
        SensorOperations::exportSensor(&listWidget, this);
    });
    contextMenu.addAction(&actionExport);

    contextMenu.exec(listWidget.mapToGlobal(pos));
}

void MainWindow::handleNewSensorData(int sensorId, double time, double value)
{
    SensorDataHandler::handleNewSensorData(sensorId, time, value, chartView, &currentValueLabel, this);
}
