#include "sensor.h"
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QDebug>

unsigned int sensor::nextID = 0;
std::unordered_map<unsigned int, sensor *> sensor::sensors;

sensor::sensor(std::string name) : sensorName(name)
{
    while (sensors.find(nextID) != sensors.end())
    {
        ++nextID;
    }
    sensorID = nextID;
    sensors[sensorID] = this;
    ++nextID;
}

sensor::sensor(std::string name, unsigned int ID)
{
    if (sensors.find(ID) != sensors.end())
    {
        throw std::runtime_error("Sensor ID already exists");
    }
    sensorID = ID;
    sensorName = name;
    sensors[ID] = this;
}

sensor::~sensor()
{
    sensors.erase(sensorID);

    // Cancella il file relativo al sensore
    QString currentPath = QDir::currentPath();
    QFile file(currentPath + "/sensors_list/" + QString::number(sensorID) + ".txt");
    if (file.exists())
    {
        file.remove();
    }
}

void sensor::setName(std::string newName)
{
    this->sensorName = newName;
    createFile();
}

unsigned int sensor::getID() const
{
    return sensorID;
}

std::string sensor::getName() const
{
    return sensorName;
}

std::unordered_map<unsigned int, sensor *> &sensor::getSensors()
{
    return sensors;
}

void sensor::updateSensorData(double time, double value) const
{
    const_cast<sensor*>(this)->addChartData(time, value); // Rimuovi const per aggiornare i dati
    QString currentPath = QDir::currentPath();
    QFile file(currentPath + "/sensors_list/" + QString::number(getID()) + ".txt");
    if (file.open(QIODevice::Append | QIODevice::Text)) // Usa Append invece di WriteOnly
    {
        QTextStream out(&file);
        out << time << "," << value << "\n";
    }
}

void sensor::addChartData(double time, double value)
{
    qDebug() << "Aggiunta dati al grafico: tempo =" << time << ", valore =" << value;
    chartData.emplace_back(time, value);
}

std::vector<std::pair<double, double>> sensor::getChartData() const
{
    return chartData;
}

void sensor::clearChartData()
{
    chartData.clear();

    QString currentPath = QDir::currentPath();
    QFile file(currentPath + "/sensors_list/" + QString::number(getID()) + ".txt");

    if (file.open(QIODevice::ReadWrite | QIODevice::Text))
    {
        QTextStream in(&file);
        QStringList lines;
        while (!in.atEnd()) {
            QString line = in.readLine();
            if (line.startsWith("Chart Data:")) {
                break;
            }
            lines.append(line);
        }
        file.resize(0); // Pulisce il contenuto del file
        for (const QString& line : lines) {
            in << line << "\n";
        }
        in << "Chart Data:\n"; // Aggiunge l'intestazione per i dati del grafico
    }
}
