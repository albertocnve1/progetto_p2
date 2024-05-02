#include "mainwindow.h"
#include "sensor.h"
#include "load_sensors.h"

#include <unordered_map>

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent), layout(this), detailsLabel("Dettagli del sensore qui")
{


    layout.addWidget(&listWidget);
    layout.addWidget(&detailsLabel);

    // Ottieni l'elenco dei sensori istanziati
    std::unordered_map<unsigned int, sensor*>& sensors = sensor::getSensors();

    // Aggiungi i sensori alla lista
    for (const auto& pair : sensors) {
        sensor* s = pair.second;
        listWidget.addItem(QString::fromStdString(s->getName()));
    }

    // Imposta il fattore di estensione per i widget
    layout.setStretchFactor(&listWidget, 1); // La colonna di sinistra sarà più piccola
    layout.setStretchFactor(&detailsLabel, 3); // Lo spazio a destra sarà più grande

    resize(1000,500);
}

MainWindow::~MainWindow()
{
}
