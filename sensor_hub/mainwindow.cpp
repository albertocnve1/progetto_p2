#include "mainwindow.h"
#include "dust_sensor.h"
#include "temperature_sensor.h"
#include "humidity_sensor.h"
#include "sensordialog.h"
#include "sensor_operations.h" // Aggiungi questa linea
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
    connect(&listWidget, &QListWidget::itemSelectionChanged, this, &MainWindow::displaySensorDetails);

    // Creazione del menu per il pulsante "+"
    QMenu *addMenu = new QMenu(this);
    QAction *newSensorAction = new QAction(tr("Nuovo sensore"), this);
    QAction *importFromFileAction = new QAction(tr("Importa da file"), this);
    addMenu->addAction(newSensorAction);
    addMenu->addAction(importFromFileAction);
    addButton->setMenu(addMenu);

    // Connessione del pulsante "+" a uno slot per aggiungere un nuovo sensore da GUI
    connect(addButton, &QPushButton::clicked, this, &MainWindow::addSensor);
    connect(newSensorAction, &QAction::triggered, this, &MainWindow::newSensor);
    // Connessione del pulsante "+" a uno slot per importare un sensore da file
    connect(importFromFileAction, &QAction::triggered, this, &MainWindow::addSensor);
    connect(removeButton, SIGNAL(clicked()), this, SLOT(deleteSensor()));

    // Crezione dello spazio per la lista di sensori a sx
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

    connect(startSimulationButton, &QPushButton::clicked, this, &MainWindow::startSimulation);
    connect(stopSimulationButton, &QPushButton::clicked, this, &MainWindow::stopSimulation);  // Connessione del pulsante allo slot

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

    // Connetti la casella di ricerca al filtro
    connect(&searchBox, &QLineEdit::textChanged, this, &MainWindow::filterSensors);

    resize(1000, 500);

    // Inizializzazione della simulazione
    sensorSimulation = new SensorSimulation(this);
    connect(sensorSimulation, &SensorSimulation::newSensorData, this, &MainWindow::handleNewSensorData);
}


MainWindow::~MainWindow()
{
    delete sensorSimulation;
}

void MainWindow::filterSensors(const QString &text)
{
    for (int i = 0; i < listWidget.count(); ++i)
    {
        QListWidgetItem *item = listWidget.item(i);
        item->setHidden(!item->text().contains(text, Qt::CaseInsensitive));
    }
}

void MainWindow::addSensor()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Seleziona file di testo"), "",
                                                    tr("Text Files (*.txt);;All Files (*)"));

    if (fileName.isEmpty())
        return;
    else
    {
        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QTextStream in(&file);
            unsigned int id = 0;
            std::string type, name;
            double precision = 0.0;
            std::vector<std::pair<double, double>> chartData;

            while (!in.atEnd())
            {
                QString line = in.readLine();
                QStringList parts = line.split(": ");
                if (parts[0] == "ID")
                {
                    id = parts[1].toUInt();
                }
                else if (parts[0] == "Type")
                {
                    type = parts[1].toStdString();
                }
                else if (parts[0] == "Name")
                {
                    name = parts[1].toStdString();
                }
                else if (parts[0] == "Precision")
                {
                    precision = parts[1].toDouble();
                }
                else if (parts[0] == "Chart Data")
                {
                    while (!in.atEnd())
                    {
                        QString dataLine = in.readLine();
                        QStringList dataParts = dataLine.split(",");
                        if (dataParts.size() == 2)
                        {
                            double time = dataParts[0].toDouble();
                            double value = dataParts[1].toDouble();
                            chartData.emplace_back(time, value);
                        }
                    }
                }
            }

            QString sensorInfo = QString::number(id) + ": " + QString::fromStdString(name);

            try
            {
                if (type == "Dust Sensor")
                {
                    dust_sensor *sensor = dust_sensor::create(name, id, precision);
                    for (const auto& data : chartData)
                    {
                        sensor->addChartData(data.first, data.second);
                    }
                    listWidget.addItem(sensorInfo);
                }
                else if (type == "Temperature Sensor")
                {
                    temperature_sensor *sensor = temperature_sensor::create(name, id, precision);
                    for (const auto& data : chartData)
                    {
                        sensor->addChartData(data.first, data.second);
                    }
                    listWidget.addItem(sensorInfo);
                }
                else if (type == "Humidity Sensor")
                {
                    humidity_sensor *sensor = humidity_sensor::create(name, id, precision);
                    for (const auto& data : chartData)
                    {
                        sensor->addChartData(data.first, data.second);
                    }
                    listWidget.addItem(sensorInfo);
                }
            }
            catch (const std::runtime_error &e)
            {
                // Gestisci l'errore qui, ad esempio mostrando un messaggio di errore all'utente
                QMessageBox::warning(this, tr("Errore"), tr("ID del sensore già esistente"));
            }
        }
    }
}





void MainWindow::newSensor()
{
    SensorDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted)
    {
        unsigned int id = dialog.idEdit->text().toUInt();
        QString type = dialog.typeEdit->currentText();
        QString name = dialog.nameEdit->text();
        double precision = dialog.precisionEdit->text().toDouble();

        if (type == "Humidity Sensor")
        {
            humidity_sensor *sensor = humidity_sensor::create(name.toStdString(), id, precision);
            QString sensorInfo = QString::number(sensor->getID()) + ": " + QString::fromStdString(sensor->getName());
            listWidget.addItem(sensorInfo);
        }
        else if (type == "Dust Sensor")
        {
            dust_sensor *sensor = dust_sensor::create(name.toStdString(), id, precision);
            QString sensorInfo = QString::number(sensor->getID()) + ": " + QString::fromStdString(sensor->getName());
            listWidget.addItem(sensorInfo);
        }
        else if (type == "Temperature Sensor")
        {
            temperature_sensor *sensor = temperature_sensor::create(name.toStdString(), id, precision);
            QString sensorInfo = QString::number(sensor->getID()) + ": " + QString::fromStdString(sensor->getName());
            listWidget.addItem(sensorInfo);
        }
    }
}

void MainWindow::deleteSensor()
{
    QListWidgetItem *item = listWidget.currentItem();
    if (!item)
    {
        QMessageBox::warning(this, tr("Errore"), tr("Nessun sensore selezionato"));
        return;
    }

    unsigned int id = item->text().split(":")[0].toUInt();

    std::unordered_map<unsigned int, sensor *> &sensors = sensor::getSensors();
    auto it = sensors.find(id);

    if (it != sensors.end())
    {
        sensor *sensorToDelete = it->second;

        listWidget.blockSignals(true);

        try
        {
            delete sensorToDelete;
        }
        catch (const std::exception &e)
        {
            listWidget.blockSignals(false);
            return;
        }

        delete listWidget.takeItem(listWidget.row(item));

        listWidget.blockSignals(false);
    }
    else
    {
        QMessageBox::warning(this, tr("Errore"), tr("Sensore non trovato"));
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

void MainWindow::displaySensorDetails()
{
    QListWidgetItem *selectedItem = listWidget.currentItem();
    if (!selectedItem)
    {
        detailsLabel.setText("Dettagli del sensore qui");
        currentValueLabel.setText(""); // Nascondi il valore della misurazione attuale
        chartView->setChart(nullptr); // Rimuovi il grafico
        return;
    }

    unsigned int id = selectedItem->text().split(":")[0].toUInt();
    std::unordered_map<unsigned int, sensor *> &sensors = sensor::getSensors();
    auto it = sensors.find(id);
    if (it != sensors.end())
    {
        sensor *s = it->second;

        QString details = QString("ID: %1\nNome: %2\nPrecisione: %3").arg(s->getID()).arg(QString::fromStdString(s->getName())).arg(s->getPrecision());

        // Crea un nuovo grafico
        QChart *chart = new QChart();
        QLineSeries *series = new QLineSeries();

        qDebug() << "Sensor type:" << QString::fromStdString(s->getName());
        qDebug() << "Chart Data:" << s->getChartData().size();

        // Usare dynamic_cast per determinare il tipo di sensore e impostare i limiti dell'asse Y
        if (dynamic_cast<dust_sensor *>(s))
        {
            details += "\nTipo: Dust Sensor";
            currentValueLabel.setText(QString("Livello PM10 attuale: %1 μg/m³").arg(dynamic_cast<dust_sensor *>(s)->getDustLevel()));

            for (const auto& data : dynamic_cast<dust_sensor *>(s)->getChartData())
            {
                series->append(data.first, data.second);
            }
            chart->addSeries(series);
            chart->setTitle("Simulazione del sensore " + QString::fromStdString(s->getName()));

            QValueAxis *axisX = new QValueAxis;
            axisX->setTitleText("Tempo (s)");
            QValueAxis *axisY = new QValueAxis;
            axisY->setRange(0, 50);
            axisY->setTitleText("PM10 (µg/m³)");

            chart->addAxis(axisX, Qt::AlignBottom);
            chart->addAxis(axisY, Qt::AlignLeft);
            series->attachAxis(axisX);
            series->attachAxis(axisY);
        }
        else if (dynamic_cast<temperature_sensor *>(s))
        {
            details += "\nTipo: Temperature Sensor";
            currentValueLabel.setText(QString("Temperatura attuale: %1 °C").arg(dynamic_cast<temperature_sensor *>(s)->getTemperature()));

            for (const auto& data : dynamic_cast<temperature_sensor *>(s)->getChartData())
            {
                series->append(data.first, data.second);
            }
            chart->addSeries(series);
            chart->setTitle("Simulazione del sensore " + QString::fromStdString(s->getName()));

            QValueAxis *axisX = new QValueAxis;
            axisX->setTitleText("Tempo (s)");
            QValueAxis *axisY = new QValueAxis;
            axisY->setRange(-20, 100);
            axisY->setTitleText("°C");

            chart->addAxis(axisX, Qt::AlignBottom);
            chart->addAxis(axisY, Qt::AlignLeft);
            series->attachAxis(axisX);
            series->attachAxis(axisY);
        }
        else if (dynamic_cast<humidity_sensor *>(s))
        {
            details += "\nTipo: Humidity Sensor";
            currentValueLabel.setText(QString("Livello umidità attuale: %1 %").arg(dynamic_cast<humidity_sensor *>(s)->getHumidity()));

            for (const auto& data : dynamic_cast<humidity_sensor *>(s)->getChartData())
            {
                series->append(data.first, data.second);
            }
            chart->addSeries(series);
            chart->setTitle("Simulazione del sensore " + QString::fromStdString(s->getName()));

            QValueAxis *axisX = new QValueAxis;
            axisX->setTitleText("Tempo (s)");
            QValueAxis *axisY = new QValueAxis;
            axisY->setRange(0, 100);
            axisY->setTitleText("% umidità nell'aria");

            chart->addAxis(axisX, Qt::AlignBottom);
            chart->addAxis(axisY, Qt::AlignLeft);
            series->attachAxis(axisX);
            series->attachAxis(axisY);
        }

        chart->legend()->hide(); // Nascondi la leggenda

        // Imposta un intervallo iniziale di 20 secondi per l'asse X
        if (series->count() > 0)
        {
            chart->axes(Qt::Horizontal).first()->setRange(0, series->at(series->count() - 1).x());
        }
        else
        {
            chart->axes(Qt::Horizontal).first()->setRange(0, 20);
        }

        chartView->setChart(chart);
        detailsLabel.setText(details);
    }
    else
    {
        detailsLabel.setText("Seleziona un sensore per mostrare i dettagli");
        currentValueLabel.setText(""); // Nascondi il valore della misurazione attuale
        chartView->setChart(nullptr); // Rimuovi il grafico
    }
}


void MainWindow::startSimulation()
{
    QListWidgetItem *selectedItem = listWidget.currentItem();
    if (!selectedItem)
    {
        QMessageBox::warning(this, tr("Errore"), tr("Nessun sensore selezionato"));
        return;
    }

    unsigned int id = selectedItem->text().split(":")[0].toUInt();
    std::unordered_map<unsigned int, sensor *> &sensors = sensor::getSensors();
    auto it = sensors.find(id);
    if (it != sensors.end())
    {
        sensor *s = it->second;

        // Cancella i dati del grafico precedente sia dal grafico che dal file
        s->clearChartData();

        QChart *chart = new QChart();
        QLineSeries *series = new QLineSeries();
        chart->addSeries(series);
        chart->setTitle("Simulazione del sensore " + QString::fromStdString(s->getName()));

        QValueAxis *axisX = new QValueAxis;
        axisX->setTitleText("Tempo (s)");
        QValueAxis *axisY = new QValueAxis;

        // Usare dynamic_cast per determinare il tipo di sensore e impostare i limiti dell'asse Y
        if (dynamic_cast<dust_sensor *>(s))
        {
            axisY->setRange(0, 50);
            axisY->setTitleText("PM10 (µg/m³)");
        }
        else if (dynamic_cast<temperature_sensor *>(s))
        {
            axisY->setRange(-20, 100);
            axisY->setTitleText("°C");
        }
        else if (dynamic_cast<humidity_sensor *>(s))
        {
            axisY->setRange(0, 100);
            axisY->setTitleText("% umidità nell'aria");
        }

        chart->addAxis(axisX, Qt::AlignBottom);
        chart->addAxis(axisY, Qt::AlignLeft);
        series->attachAxis(axisX);
        series->attachAxis(axisY);

        chart->legend()->hide(); // Nascondere la leggenda

        // Imposta un intervallo iniziale di 20 secondi per l'asse X
        axisX->setRange(0, 20);

        chartView->setChart(chart);

        sensorSimulation->simulateSensor(id);
    }
    else
    {
        QMessageBox::warning(this, tr("Errore"), tr("Sensore non trovato"));
    }
}


void MainWindow::stopSimulation()
{
    sensorSimulation->stopSimulation();
}


void MainWindow::handleNewSensorData(int sensorId, double time, double value)
{
    SensorDataHandler::handleNewSensorData(sensorId, time, value, chartView, &currentValueLabel, this);
}




