#include "mainwindow.h"
#include "sensor.h"
#include "load_sensors.h"
#include "sensor.h"
#include "dust_sensor.h"
#include "temperature_sensor.h"
#include "humidity_sensor.h"
#include "sensordialog.h"

#include <unordered_map>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QListWidget>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QMenu>

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent), layout(new QHBoxLayout(this)), detailsLabel("Dettagli del sensore qui")
{
    // Crea un layout orizzontale per i pulsanti
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    QPushButton *addButton = new QPushButton("+");
    QPushButton *removeButton = new QPushButton("-");
    buttonLayout->addWidget(&searchBox);
    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(removeButton);

    // Crea il menu per il pulsante "+"
    QMenu *addMenu = new QMenu(this);
    QAction *newSensorAction = new QAction(tr("Nuovo sensore"), this);
    QAction *importFromFileAction = new QAction(tr("Importa da file"), this);
    addMenu->addAction(newSensorAction);
    addMenu->addAction(importFromFileAction);
    addButton->setMenu(addMenu);

    // Connetti il pulsante "addButton" a uno slot per aggiungere un nuovo sensore
    connect(addButton, &QPushButton::clicked, this, &MainWindow::addSensor);
    connect(newSensorAction, &QAction::triggered, this, &MainWindow::newSensor);
    // Connetti l'azione "Importa da file" alla funzione addSensor
    connect(importFromFileAction, &QAction::triggered, this, &MainWindow::addSensor);

    // Crea il layout verticale di sinistra
    QVBoxLayout *leftLayout = new QVBoxLayout;
    leftLayout->addLayout(buttonLayout);  // Aggiungi il layout dei pulsanti
    leftLayout->addWidget(&listWidget);

    layout->addLayout(leftLayout);
    layout->addWidget(&detailsLabel);

    // Ottieni l'elenco dei sensori istanziati
    std::unordered_map<unsigned int, sensor*>& sensors = sensor::getSensors();

    // Aggiungi i sensori alla lista
    for (const auto& pair : sensors) {
        sensor* s = pair.second;
        QString sensorInfo = QString::number(s->getID()) + ": " + QString::fromStdString(s->getName());
        listWidget.addItem(sensorInfo);
    }

    // Imposta il fattore di estensione per i widget
    this->layout->setStretchFactor(leftLayout, 1); // La colonna di sinistra sarà più piccola
    this->layout->setStretchFactor(&detailsLabel, 3); // Lo spazio a destra sarà più grande

    // Connetti la casella di ricerca al filtro
    connect(&searchBox, &QLineEdit::textChanged, this, &MainWindow::filterSensors);


    resize(1000,500);
}

MainWindow::~MainWindow()
{
}

void MainWindow::filterSensors(const QString &text)
{
    for (int i = 0; i < listWidget.count(); ++i) {
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
    else {
        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            unsigned int id = 0;
            std::string type, name;
            double precision = 0.0;

            while (!in.atEnd()) {
                QString line = in.readLine();
                QStringList parts = line.split(": ");
                if (parts[0] == "ID") {
                    id = parts[1].toUInt();
                } else if (parts[0] == "Type") {
                    type = parts[1].toStdString();
                } else if (parts[0] == "Name") {
                    name = parts[1].toStdString();
                } else if (parts[0] == "Precision") {
                    precision = parts[1].toDouble();
                }
            }

            QString sensorInfo = QString::number(id) + ": " + QString::fromStdString(name);

            try {
                if (type == "Dust Sensor") {
                    dust_sensor* sensor = dust_sensor::create(name, id, precision);
                    listWidget.addItem(sensorInfo);
                }
                else if (type == "Temperature Sensor") {
                    temperature_sensor* sensor = temperature_sensor::create(name, id, precision);
                    listWidget.addItem(sensorInfo);
                }
                else if (type == "Humidity Sensor") {
                    humidity_sensor* sensor = humidity_sensor::create(name, id, precision);
                    listWidget.addItem(sensorInfo);
                }
            } catch (const std::runtime_error& e) {
                // Gestisci l'errore qui, ad esempio mostrando un messaggio di errore all'utente
                QMessageBox::warning(this, tr("Errore"), tr("ID del sensore già esistente"));
            }
        }
    }
}

void MainWindow::newSensor()
{
    SensorDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        unsigned int id = dialog.idEdit->text().toUInt();
        QString type = dialog.typeEdit->currentText();
        QString name = dialog.nameEdit->text();
        double precision = dialog.precisionEdit->text().toDouble();

        if (type == "Humidity Sensor") {
            humidity_sensor* sensor = humidity_sensor::create(name.toStdString(), id, precision);
            createSensorFile(sensor);
        } else if (type == "Dust Sensor") {
            dust_sensor* sensor = dust_sensor::create(name.toStdString(), id, precision);
            createSensorFile(sensor);
        } else if (type == "Temperature Sensor") {
            temperature_sensor* sensor = temperature_sensor::create(name.toStdString(), id, precision);
            createSensorFile(sensor);
        }
    }
}

void MainWindow::createSensorFile(sensor* sensor)
{
    QString currentPath = QDir::currentPath();
    QDir dir;
    dir.mkdir(currentPath + "/sensors_list");
    QFile file(currentPath + "/sensors_list/" + QString::number(sensor->getID()) + ".txt");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << "ID: " << sensor->getID() << "\n";
        out << "Type: " << QString::fromStdString(sensor->getSensorType()) << "\n";
        out << "Name: " << QString::fromStdString(sensor->getName()) << "\n";
        out << "Precision: " << sensor->getPrecision() << "\n";
    }
    loadSensors();
}