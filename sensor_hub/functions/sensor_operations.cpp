#include <QRegularExpression>
#include "sensor_operations.h"
#include "sensors/dust_sensor.h"
#include "sensors/temperature_sensor.h"
#include "sensors/humidity_sensor.h"
#include "functions/sensordialog.h"

QRegularExpression re("[A-Za-z]");

void SensorOperations::editSensor(QListWidget *listWidget, QWidget *parent)
{
    QListWidgetItem *selectedItem = listWidget->currentItem();
    if (!selectedItem)
    {
        QMessageBox::warning(parent, QObject::tr("Errore"), QObject::tr("Nessun sensore selezionato"));
        return;
    }

    QString itemText = selectedItem->text();
    unsigned int id = itemText.split(":")[0].toUInt();

    std::unordered_map<unsigned int, sensor *> &sensors = sensor::getSensors();
    auto it = sensors.find(id);
    if (it == sensors.end())
    {
        QMessageBox::warning(parent, QObject::tr("Errore"), QObject::tr("Sensore non trovato"));
        return;
    }

    sensor *sensorToEdit = it->second;

    bool ok;
    QString text = QInputDialog::getText(parent, QObject::tr("Modifica nome"),
                                         QObject::tr("Inserisci il nuovo nome:"), QLineEdit::Normal,
                                         QString::fromStdString(sensorToEdit->getName()), &ok);
    if (ok)
    {
        if (!re.match(text).hasMatch())
        {
            QMessageBox::warning(parent, QObject::tr("Errore"), QObject::tr("Nome inserito non valido"));
            return;
        }
        sensorToEdit->setName(text.toStdString());
        selectedItem->setText(QString::number(sensorToEdit->getID()) + ": " + text);
    }
}

void SensorOperations::exportSensor(QListWidget *listWidget, QWidget *parent)
{
    QListWidgetItem *selectedItem = listWidget->currentItem();
    if (!selectedItem)
    {
        QMessageBox::warning(parent, QObject::tr("Errore"), QObject::tr("Nessun sensore selezionato"));
        return;
    }

    QString itemText = selectedItem->text();
    unsigned int id = itemText.split(":")[0].toUInt();

    std::unordered_map<unsigned int, sensor *> &sensors = sensor::getSensors();
    auto it = sensors.find(id);
    if (it == sensors.end())
    {
        QMessageBox::warning(parent, QObject::tr("Errore"), QObject::tr("Sensore non trovato"));
        return;
    }

    QString folderPath = QFileDialog::getExistingDirectory(parent, QObject::tr("Seleziona cartella"));
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
        QMessageBox::information(parent, QObject::tr("Esportazione riuscita"), QObject::tr("File del sensore esportato con successo"));
    }
    else
    {
        QMessageBox::warning(parent, QObject::tr("Errore"), QObject::tr("File del sensore non trovato"));
    }
}

void SensorOperations::addSensor(QListWidget *listWidget, QWidget *parent)
{
    QString fileName = QFileDialog::getOpenFileName(parent,
                                                    QObject::tr("Seleziona file di testo"), "",
                                                    QObject::tr("Text Files (*.txt);;All Files (*)"));

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
                    for (const auto &data : chartData)
                    {
                        sensor->addChartData(data.first, data.second);
                    }
                    listWidget->addItem(sensorInfo);
                }
                else if (type == "Temperature Sensor")
                {
                    temperature_sensor *sensor = temperature_sensor::create(name, id, precision);
                    for (const auto &data : chartData)
                    {
                        sensor->addChartData(data.first, data.second);
                    }
                    listWidget->addItem(sensorInfo);
                }
                else if (type == "Humidity Sensor")
                {
                    humidity_sensor *sensor = humidity_sensor::create(name, id, precision);
                    for (const auto &data : chartData)
                    {
                        sensor->addChartData(data.first, data.second);
                    }
                    listWidget->addItem(sensorInfo);
                }
            }
            catch (const std::runtime_error &e)
            {
                QMessageBox::warning(parent, QObject::tr("Errore"), QObject::tr("ID del sensore già esistente"));
            }
        }
    }
}

void SensorOperations::newSensor(QListWidget *listWidget, QWidget *parent)
{
    SensorDialog dialog(parent);
    if (dialog.exec() == QDialog::Accepted)
    {
        unsigned int id = dialog.idEdit->text().toUInt();
        QString type = dialog.typeEdit->currentText();
        QString name = dialog.nameEdit->text();
        double precision = dialog.precisionEdit->text().toDouble();
        if (!re.match(name).hasMatch())
        {
            QMessageBox::warning(parent, QObject::tr("Errore"), QObject::tr("Impostare un nome valido"));
            return;
        }

        try
        {
            if (type == "Humidity Sensor")
            {
                humidity_sensor *sensor = humidity_sensor::create(name.toStdString(), id, precision);
                QString sensorInfo = QString::number(sensor->getID()) + ": " + QString::fromStdString(sensor->getName());
                listWidget->addItem(sensorInfo);
            }
            else if (type == "Dust Sensor")
            {
                dust_sensor *sensor = dust_sensor::create(name.toStdString(), id, precision);
                QString sensorInfo = QString::number(sensor->getID()) + ": " + QString::fromStdString(sensor->getName());
                listWidget->addItem(sensorInfo);
            }
            else if (type == "Temperature Sensor")
            {
                temperature_sensor *sensor = temperature_sensor::create(name.toStdString(), id, precision);
                QString sensorInfo = QString::number(sensor->getID()) + ": " + QString::fromStdString(sensor->getName());
                listWidget->addItem(sensorInfo);
            }
        }
        catch (const std::runtime_error &e)
        {
            QMessageBox::warning(parent, QObject::tr("Errore"), QObject::tr("ID del sensore già esistente"));
        }
    }
}

void SensorOperations::deleteSensor(QListWidget *listWidget, QWidget *parent)
{
    QListWidgetItem *item = listWidget->currentItem();
    if (!item)
    {
        QMessageBox::warning(parent, QObject::tr("Errore"), QObject::tr("Nessun sensore selezionato"));
        return;
    }

    unsigned int id = item->text().split(":")[0].toUInt();

    std::unordered_map<unsigned int, sensor *> &sensors = sensor::getSensors();
    auto it = sensors.find(id);

    if (it != sensors.end())
    {
        sensor *sensorToDelete = it->second;

        listWidget->blockSignals(true);

        try
        {
            delete sensorToDelete;
        }
        catch (const std::exception &e)
        {
            listWidget->blockSignals(false);
            return;
        }

        delete listWidget->takeItem(listWidget->row(item));

        listWidget->blockSignals(false);
    }
    else
    {
        QMessageBox::warning(parent, QObject::tr("Errore"), QObject::tr("Sensore non trovato"));
    }
}
