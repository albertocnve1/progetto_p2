#include "sensor_operations.h"

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
    if (ok && !text.isEmpty())
    {
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
