#include "mainwindow.h"
#include "dust_sensor.h"
#include "temperature_sensor.h"
#include "humidity_sensor.h"
#include "sensordialog.h"

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
    : QWidget(parent), layout(new QHBoxLayout(this)), detailsLabel(""), chartView(new QChartView), startSimulationButton(new QPushButton("Avvia Simulazione")), stopSimulationButton(new QPushButton("Interrompi Simulazione"))
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
    connect(&actionEdit, &QAction::triggered, this, &MainWindow::editSensor);
    contextMenu.addAction(&actionEdit);

    QAction actionExport("Esporta", this);
    connect(&actionExport, &QAction::triggered, this, &MainWindow::exportSensor);
    contextMenu.addAction(&actionExport);

    contextMenu.exec(listWidget.mapToGlobal(pos));
}

void MainWindow::editSensor()
{
    QListWidgetItem *selectedItem = listWidget.currentItem();
    if (!selectedItem)
    {
        QMessageBox::warning(this, tr("Errore"), tr("Nessun sensore selezionato"));
        return;
    }

    QString itemText = selectedItem->text();
    unsigned int id = itemText.split(":")[0].toUInt();

    std::unordered_map<unsigned int, sensor *> &sensors = sensor::getSensors();
    auto it = sensors.find(id);
    if (it == sensors.end())
    {
        QMessageBox::warning(this, tr("Errore"), tr("Sensore non trovato"));
        return;
    }

    sensor *sensorToEdit = it->second;

    bool ok;
    QString text = QInputDialog::getText(this, tr("Modifica nome"),
                                         tr("Inserisci il nuovo nome:"), QLineEdit::Normal,
                                         QString::fromStdString(sensorToEdit->getName()), &ok);
    if (ok && !text.isEmpty())
    {
        sensorToEdit->setName(text.toStdString());

        selectedItem->setText(QString::number(sensorToEdit->getID()) + ": " + text);
    }
}

void MainWindow::exportSensor()
{
    QListWidgetItem *selectedItem = listWidget.currentItem();
    if (!selectedItem)
    {
        QMessageBox::warning(this, tr("Errore"), tr("Nessun sensore selezionato"));
        return;
    }

    QString itemText = selectedItem->text();
    unsigned int id = itemText.split(":")[0].toUInt();

    std::unordered_map<unsigned int, sensor *> &sensors = sensor::getSensors();
    auto it = sensors.find(id);
    if (it == sensors.end())
    {
        QMessageBox::warning(this, tr("Errore"), tr("Sensore non trovato"));
        return;
    }

    QString folderPath = QFileDialog::getExistingDirectory(this, tr("Seleziona cartella"));
    if (folderPath.isEmpty())
    {
        return;
    }

    QString currentPath = QDir::currentPath();
    QString sourceFilePath = currentPath + "/sensors_list/" + QString::number(id) + ".txt";
    QString destinationFilePath = folderPath + "/" + QString::number(id) + ".txt";

    if (QFile::exists(sourceFilePath))
    {
        QFile::copy(sourceFilePath, destinationFilePath);
        QMessageBox::information(this, tr("Esportazione riuscita"), tr("File del sensore esportato con successo"));
    }
    else
    {
        QMessageBox::warning(this, tr("Errore"), tr("File del sensore non trovato"));
    }
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
    std::unordered_map<unsigned int, sensor *> &sensors = sensor::getSensors();
    auto it = sensors.find(sensorId);
    if (it != sensors.end())
    {
        sensor *s = it->second;

        // Aggiorna il valore della misurazione attuale
        if (dynamic_cast<dust_sensor *>(s))
        {
            dynamic_cast<dust_sensor *>(s)->setDustLevel(value);
            currentValueLabel.setText(QString("Livello PM10 attuale: %1 μg/m³").arg(value));
        }
        else if (dynamic_cast<temperature_sensor *>(s))
        {
            dynamic_cast<temperature_sensor *>(s)->setTemperature(value);
            currentValueLabel.setText(QString("Temperatura attuale: %1 °C").arg(value));
        }
        else if (dynamic_cast<humidity_sensor *>(s))
        {
            dynamic_cast<humidity_sensor *>(s)->setHumidity(value);
            currentValueLabel.setText(QString("Livello umidità attuale: %1 %").arg(value));
        }

        QLineSeries *series;
        QChart *chart = chartView->chart();
        if (chart && chart->series().size() > 0)
        {
            series = qobject_cast<QLineSeries *>(chart->series().at(0));
        }
        else
        {
            series = new QLineSeries();
            chart = new QChart();
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
        }

        series->append(time, value);

        // Aggiorna gli intervalli dell'asse X per assicurare che i dati siano visibili
        QList<QAbstractAxis*> axesX = chart->axes(Qt::Horizontal, series);
        if (!axesX.isEmpty()) {
            QValueAxis *axisX = qobject_cast<QValueAxis *>(axesX.first());
            if (axisX) {
                double currentMax = axisX->max();
                if (time > currentMax) {
                    axisX->setRange(0, currentMax * 2); // Raddoppia l'intervallo dell'asse X
                }
            }
        }

        // Aggiorna il titolo dell'asse Y
        QList<QAbstractAxis*> axesY = chart->axes(Qt::Vertical, series);
        if (!axesY.isEmpty()) {
            QValueAxis *axisY = qobject_cast<QValueAxis *>(axesY.first());
            if (axisY) {
                // Usare dynamic_cast per determinare il tipo di sensore e impostare il titolo dell'asse Y
                if (dynamic_cast<dust_sensor *>(s))
                {
                    axisY->setTitleText("PM10 (µg/m³)");
                }
                else if (dynamic_cast<temperature_sensor *>(s))
                {
                    axisY->setTitleText("°C");
                }
                else if (dynamic_cast<humidity_sensor *>(s))
                {
                    axisY->setTitleText("% umidità nell'aria");
                }
            }
        }

        // Nascondi la leggenda ogni volta che il grafico viene aggiornato
        if (chart)
        {
            chart->legend()->hide();
        }
    }
    else
    {
        QMessageBox::warning(this, tr("Errore"), tr("Sensore non trovato"));
    }
}




