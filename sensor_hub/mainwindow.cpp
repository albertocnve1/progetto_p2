#include "mainwindow.h"
#include "sensor.h"
#include "load_sensors.h"

#include <unordered_map>
#include <QLineEdit>
#include <QHBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent), layout(new QHBoxLayout(this)), detailsLabel("Dettagli del sensore qui")
{
    QVBoxLayout *leftLayout = new QVBoxLayout;
    leftLayout->addWidget(&searchBox);
    leftLayout->addWidget(&listWidget);

    layout->addLayout(leftLayout);
    layout->addWidget(&detailsLabel);

    // Ottieni l'elenco dei sensori istanziati
    std::unordered_map<unsigned int, sensor*>& sensors = sensor::getSensors();

    // Aggiungi i sensori alla lista
    for (const auto& pair : sensors) {
        sensor* s = pair.second;
        listWidget.addItem(QString::fromStdString(s->getName()));
    }

    // Imposta il fattore di estensione per i widget
    layout->setStretchFactor(leftLayout, 1); // La colonna di sinistra sarà più piccola
    layout->setStretchFactor(&detailsLabel, 3); // Lo spazio a destra sarà più grande

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