#include "mainwindow.h"
#include "dust_sensor.h"
#include "temperature_sensor.h"
#include "humidity_sensor.h"
#include "sensordialog.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QMenu>
#include <QInputDialog>

#include <QtCharts>

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent), layout(new QHBoxLayout(this)), detailsLabel("Dettagli del sensore qui")
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
    layout->addWidget(&detailsLabel);

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
    this->layout->setStretchFactor(&detailsLabel, 3);

    // Connetti la casella di ricerca al filtro
    connect(&searchBox, &QLineEdit::textChanged, this, &MainWindow::filterSensors);

    resize(1000, 500);
}

MainWindow::~MainWindow()
{
}

// creazione della barra di ricerca per riceracare tra i sensori
void MainWindow::filterSensors(const QString &text)
{
    for (int i = 0; i < listWidget.count(); ++i)
    {
        QListWidgetItem *item = listWidget.item(i);
        item->setHidden(!item->text().contains(text, Qt::CaseInsensitive));
    }
}

// funzione per aggiungere un nuovo sensore da file di testo
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

// funzione per l'eliminazione di un sensore
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

// creazione di un menu contestuale per la modifica e esportazione di un sensore
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

// funzione che permette di rinominare un sensore
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

// funzione per esportare un sensore sottoforma di file di testo
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
