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
    : QWidget(parent), layout(new QHBoxLayout(this)), detailsLabel("Dettagli del sensore qui"), chartView(new QChartView), startSimulationButton(new QPushButton("Avvia Simulazione"))
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
    rightLayout->addWidget(chartView);
    rightLayout->addWidget(startSimulationButton);

    layout->addLayout(rightLayout);

    connect(startSimulationButton, &QPushButton::clicked, this, &MainWindow::startSimulation);

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
}

MainWindow::~MainWindow()
{
}

// Creazione della barra di ricerca per riceracare tra i sensori
void MainWindow::filterSensors(const QString &text)
{
    for (int i = 0; i < listWidget.count(); ++i)
    {
        QListWidgetItem *item = listWidget.item(i);
        item->setHidden(!item->text().contains(text, Qt::CaseInsensitive));
    }
}

// Funzione per aggiungere un nuovo sensore da file di testo
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
            }

            QString sensorInfo = QString::number(id) + ": " + QString::fromStdString(name);

            try
            {
                if (type == "Dust Sensor")
                {
                    dust_sensor *sensor = dust_sensor::create(name, id, precision);
                    listWidget.addItem(sensorInfo);
                }
                else if (type == "Temperature Sensor")
                {
                    temperature_sensor *sensor = temperature_sensor::create(name, id, precision);
                    listWidget.addItem(sensorInfo);
                }
                else if (type == "Humidity Sensor")
                {
                    humidity_sensor *sensor = humidity_sensor::create(name, id, precision);
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

// Funzione per l'eliminazione di un sensore
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
            delete sensorToDelete; // Questo ora elimina anche il file del sensore
        }
        catch (const std::exception &e)
        {
            listWidget.blockSignals(false);
            return;
        }

        // Rimozione del sensore eliminato dalla lista
        delete listWidget.takeItem(listWidget.row(item));

        listWidget.blockSignals(false);
    }
    else
    {
        QMessageBox::warning(this, tr("Errore"), tr("Sensore non trovato"));
    }
}

// Creazione di un menu contestuale per la modifica e esportazione di un sensore
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

// Funzione che permette di rinominare un sensore
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

    // Trova il sensore corrispondente
    std::unordered_map<unsigned int, sensor *> &sensors = sensor::getSensors();
    auto it = sensors.find(id);
    if (it == sensors.end())
    {
        QMessageBox::warning(this, tr("Errore"), tr("Sensore non trovato"));
        return;
    }

    sensor *sensorToEdit = it->second;

    // Mostra una finestra di dialogo per inserire il nuovo nome
    bool ok;
    QString text = QInputDialog::getText(this, tr("Modifica nome"),
                                         tr("Inserisci il nuovo nome:"), QLineEdit::Normal,
                                         QString::fromStdString(sensorToEdit->getName()), &ok);
    if (ok && !text.isEmpty())
    {
        // Imposta il nuovo nome del sensore e aggiorna il file
        sensorToEdit->setName(text.toStdString());

        // Aggiorna il testo dell'elemento della lista
        selectedItem->setText(QString::number(sensorToEdit->getID()) + ": " + text);
    }
}

// Funzione per esportare un sensore sottoforma di file di testo
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

    // Seleziona la cartella di destinazione
    QString folderPath = QFileDialog::getExistingDirectory(this, tr("Seleziona cartella"));
    if (folderPath.isEmpty())
    {
        return;
    }

    QString currentPath = QDir::currentPath();
    QString sourceFilePath = currentPath + "/sensors_list/" + QString::number(id) + ".txt";
    QString destinationFilePath = folderPath + "/" + QString::number(id) + ".txt";

    // Copia il file nella cartella selezionata
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

// Mostra i dettagli del sensore selezionato
void MainWindow::displaySensorDetails()
{
    QListWidgetItem *selectedItem = listWidget.currentItem();
    if (!selectedItem)
    {
        detailsLabel.setText("Dettagli del sensore qui");
        return;
    }

    unsigned int id = selectedItem->text().split(":")[0].toUInt();
    std::unordered_map<unsigned int, sensor *> &sensors = sensor::getSensors();
    auto it = sensors.find(id);
    if (it != sensors.end())
    {
        sensor *s = it->second;
        QString details = QString("ID: %1\nNome: %2\nTipo: %3\nPrecisione: %4")
                              .arg(s->getID())
                              .arg(QString::fromStdString(s->getName()))
                              .arg(QString::fromStdString(s->getSensorType()))
                              .arg(s->getPrecision());
        detailsLabel.setText(details);
    }
    else
    {
        detailsLabel.setText("Dettagli del sensore qui");
    }
}

QString MainWindow::getAxisLabel(const std::string &sensorType)
{
    if (sensorType == "temperature_sensor")
    {
        return "°C";
    }
    else if (sensorType == "humidity_sensor")
    {
        return "% di umidità nell'aria";
    }
    else if (sensorType == "dust_sensor")
    {
        return "PM10";
    }
    else
    {
        return "";
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

        // Genera dati di simulazione
        QLineSeries *series = new QLineSeries();
        QRandomGenerator generator(QRandomGenerator::global()->generate()); // Inizializza un generatore locale con il seed globale
        for (int i = 0; i < 100; ++i) // Simula 100 raccolte di dati
        {
            double value = generator.bounded(10.0); // Simulazione di dati casuali
            series->append(i, value);
        }

        QChart *chart = new QChart();
        chart->addSeries(series);
        chart->setTitle("Simulazione del sensore " + QString::fromStdString(s->getName()));
        chart->createDefaultAxes();

        // Imposta i nomi degli assi
        QValueAxis *axisX = new QValueAxis;
        QValueAxis *axisY = new QValueAxis;
        axisX->setTitleText("Tempo");
        axisY->setTitleText(getAxisLabel(s->getSensorType()));

        chart->setAxisX(axisX, series);
        chart->setAxisY(axisY, series);

        chartView->setChart(chart);
    }
    else
    {
        QMessageBox::warning(this, tr("Errore"), tr("Sensore non trovato"));
    }
}


