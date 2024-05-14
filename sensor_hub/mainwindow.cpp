#include "mainwindow.h"
#include "sensor.h"
#include "load_sensors.h"

#include <unordered_map>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QListWidget>

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent), layout(new QHBoxLayout(this)), detailsLabel("Dettagli del sensore qui")
{
    // Crea un layout orizzontale per la barra di ricerca e il pulsante
    QHBoxLayout *searchLayout = new QHBoxLayout;
    QPushButton *addButton = new QPushButton("+");
    searchLayout->addWidget(&searchBox);
    searchLayout->addWidget(addButton);

    // Crea il layout verticale di sinistra
    QVBoxLayout *leftLayout = new QVBoxLayout;
    leftLayout->addLayout(searchLayout);  // Aggiungi il layout di ricerca
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

    // Connetti il pulsante "addButton" a uno slot per aggiungere un nuovo sensore
    connect(addButton, &QPushButton::clicked, this, &MainWindow::addSensor);

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
    // Logica per aggiungere un nuovo sensore
    // Questo potrebbe aprire una nuova finestra di dialogo per inserire i dettagli del sensore
    // Oppure potrebbe semplicemente aggiungere un sensore predefinito alla lista
}
