#include "mainwindow.h"
#include "sensors/dust_sensor.h"
#include "sensors/temperature_sensor.h"
#include "sensors/humidity_sensor.h"
#include "functions/sensordialog.h"
#include "functions/sensor_operations.h"
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
#include <QLabel>
#include <QListWidgetItem>
#include <QIcon>

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent), layout(new QHBoxLayout), searchBox(new QLineEdit), listWidget(new QListWidget),
    detailsLabel(new QLabel), currentValueLabel(new QLabel), addButton(new QPushButton("+")),
    removeButton(new QPushButton("-")), startSimulationButton(new QPushButton("Avvia nuova simulazione")), stopSimulationButton(new QPushButton("Interrompi Simulazione")),
    chartView(new QChartView), rightLayout(new QVBoxLayout)  // startSimulationButton e stopSimulationButton sono inizializzati prima di chartView
{
    // Impostare il titolo della finestra
    setWindowTitle("Sensor Hub");

    // Impostare le dimensioni minime della finestra
    setMinimumSize(1000, 500);

    // Creazione della barra dei menu
    menuBar = new QMenuBar(this);
    fileMenu = new QMenu(tr("File"), this);
    menuBar->addMenu(fileMenu);

    newSensorAction = new QAction(tr("Nuovo sensore"), this);
    importSensorAction = new QAction(tr("Importa sensore"), this);
    editSensorAction = new QAction(tr("Modifica Sensore"), this);
    deleteSensorAction = new QAction(tr("Rimuovi Sensore"), this);

    fileMenu->addAction(newSensorAction);
    fileMenu->addAction(importSensorAction);
    fileMenu->addAction(editSensorAction);
    fileMenu->addAction(deleteSensorAction);

    connect(newSensorAction, &QAction::triggered, this, &MainWindow::addNewSensor);
    connect(importSensorAction, &QAction::triggered, this, &MainWindow::importSensor);
    connect(editSensorAction, &QAction::triggered, this, &MainWindow::editSensor);
    connect(deleteSensorAction, &QAction::triggered, this, &MainWindow::deleteSensor);

    // Creazione di un layout orizzontale per i pulsanti
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(searchBox);
    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(removeButton);

    listWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(listWidget, &QListWidget::customContextMenuRequested, this, &MainWindow::showContextMenu);

    // Creazione del menu per il pulsante "+"
    QMenu *addMenu = new QMenu(this);
    QAction *newSensorActionBtn = new QAction(tr("Nuovo sensore"), this);
    QAction *importFromFileAction = new QAction(tr("Importa da file"), this);
    addMenu->addAction(newSensorActionBtn);
    addMenu->addAction(importFromFileAction);
    addButton->setMenu(addMenu);

    // Connessione del pulsante "+" a uno slot per aggiungere un nuovo sensore da GUI
    connect(addButton, &QPushButton::clicked, this, [this]()
            { SensorOperations::addSensor(listWidget, this); });
    connect(newSensorActionBtn, &QAction::triggered, this, [this]()
            { SensorOperations::newSensor(listWidget, this); });
    // Connessione del pulsante "+" a uno slot per importare un sensore da file
    connect(importFromFileAction, &QAction::triggered, this, [this]()
            { SensorOperations::addSensor(listWidget, this); });
    connect(removeButton, &QPushButton::clicked, this, [this]()
            { SensorOperations::deleteSensor(listWidget, this); });

    // Creazione dello spazio per la lista di sensori a sx
    QVBoxLayout *leftLayout = new QVBoxLayout;
    leftLayout->addLayout(buttonLayout);
    leftLayout->addWidget(listWidget);

    layout->addLayout(leftLayout); // Assegna leftLayout al layout principale

    // Creazione layout destro
    rightLayout = new QVBoxLayout;
    rightLayout->addWidget(detailsLabel);
    rightLayout->addWidget(currentValueLabel); // Aggiungiamo il currentValueLabel al layout
    rightLayout->addWidget(chartView);

    QHBoxLayout *simulationButtonsLayout = new QHBoxLayout; // Layout orizzontale per i pulsanti di simulazione
    simulationButtonsLayout->addWidget(startSimulationButton);
    simulationButtonsLayout->addWidget(stopSimulationButton);

    rightLayout->addLayout(simulationButtonsLayout); // Aggiungi il layout dei pulsanti di simulazione

    layout->addLayout(rightLayout); // Assegna rightLayout al layout principale

    // Impostazione una dimensione dell'icona personalizzata
    listWidget->setIconSize(QSize(48, 48)); // Imposta la dimensione desiderata delle icone

    // Connessione la casella di ricerca al filtro
    connect(searchBox, &QLineEdit::textChanged, this, &MainWindow::filterSensors);

    // Creazione layout principale
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setMenuBar(menuBar);
    mainLayout->addLayout(layout);
    setLayout(mainLayout);

    resize(1000, 500);

    // Inizializzazione della simulazione
    sensorSimulation = new SensorSimulation(this);
    connect(sensorSimulation, &SensorSimulation::newSensorData, this, &MainWindow::handleNewSensorData);

    // Inizializzazione del gestore della simulazione
    sensorSimulationManager = new SensorSimulationManager(chartView, detailsLabel, currentValueLabel, listWidget, this);
    connect(startSimulationButton, &QPushButton::clicked, sensorSimulationManager, &SensorSimulationManager::startSimulation);
    connect(stopSimulationButton, &QPushButton::clicked, sensorSimulationManager, &SensorSimulationManager::stopSimulation);
    connect(listWidget, &QListWidget::itemSelectionChanged, sensorSimulationManager, &SensorSimulationManager::displaySensorDetails);

    // Ottenimento dell'elenco dei sensori istanziati
    std::unordered_map<unsigned int, sensor *> &sensors = sensor::getSensors();

    // Aggiunta dei sensori istanziati all'elenco laterale sx
    for (const auto &pair : sensors)
    {
        sensor *s = pair.second;
        QString sensorInfo = QString::number(s->getID()) + ": " + QString::fromStdString(s->getName());
        QListWidgetItem *item = new QListWidgetItem(sensorInfo);

        // Aggiunta dell'immagine in base al tipo di sensore
        QString imagePath = ":/assets/default.png"; // Percorso predefinito

        if (dynamic_cast<dust_sensor *>(s))
        {
            imagePath = ":/assets/dust_sensor_icon.png";
        }
        else if (dynamic_cast<temperature_sensor *>(s))
        {
            imagePath = ":/assets/temperature_sensor_icon.png";
        }
        else if (dynamic_cast<humidity_sensor *>(s))
        {
            imagePath = ":/assets/humidity_sensor_icon.png";
        }

        item->setIcon(QIcon(imagePath));
        listWidget->addItem(item);
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

void MainWindow::updateSensorDetails()
{
    QListWidgetItem *currentItem = listWidget->currentItem();
    if (currentItem)
    {
        sensorSimulationManager->displaySensorDetails();
    }
    else
    {
        detailsLabel->clear();
        currentValueLabel->clear();
        chartView->chart()->removeAllSeries();
    }
}

void MainWindow::filterSensors(const QString &text)
{
    for (int i = 0; i < listWidget->count(); ++i)
    {
        QListWidgetItem *item = listWidget->item(i);
        item->setHidden(!item->text().contains(text, Qt::CaseInsensitive));
    }
}

void MainWindow::showContextMenu(const QPoint &pos)
{
    QListWidgetItem *selectedItem = listWidget->currentItem();
    if (!selectedItem)
    {
        QMessageBox::warning(this, tr("Errore"), tr("Nessun sensore selezionato"));
        return;
    }

    QMenu contextMenu(tr("Context menu"), this);

    QAction actionEdit("Modifica", this);
    connect(&actionEdit, &QAction::triggered, this, [this]()
            { SensorOperations::editSensor(listWidget, this); });
    contextMenu.addAction(&actionEdit);

    QAction actionExport("Esporta", this);
    connect(&actionExport, &QAction::triggered, this, [this]()
            { SensorOperations::exportSensor(listWidget, this); });
    contextMenu.addAction(&actionExport);

    contextMenu.exec(listWidget->mapToGlobal(pos));
}

void MainWindow::handleNewSensorData(int sensorId, double time, double value)
{
    SensorDataHandler::handleNewSensorData(sensorId, time, value, chartView, currentValueLabel, this);
}

void MainWindow::addNewSensor()
{
    SensorOperations::newSensor(listWidget, this);
}

void MainWindow::importSensor()
{
    SensorOperations::addSensor(listWidget, this);
}

void MainWindow::editSensor()
{
    SensorOperations::editSensor(listWidget, this);
}

void MainWindow::deleteSensor()
{
    SensorOperations::deleteSensor(listWidget, this);
}


