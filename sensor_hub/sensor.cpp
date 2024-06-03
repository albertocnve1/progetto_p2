#include "sensor.h"
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <stdexcept>

unsigned int sensor::nextID = 0;
std::unordered_map<unsigned int, sensor*> sensor::sensors;

sensor::sensor(std::string name) : sensorName(name)
{
    while (sensors.find(nextID) != sensors.end())
    {
        ++nextID;
    }
    sensorID = nextID;
    sensors[sensorID] = this;
    ++nextID;

    loadChartDataFromFile(); // Carica i dati del grafico dal file
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

    loadChartDataFromFile(); // Carica i dati del grafico dal file
}

sensor::~sensor()
{
    sensors.erase(sensorID);

    // Delete the sensor file
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
    updateFile(newName);
}

unsigned int sensor::getID() const
{
    return sensorID;
}

std::string sensor::getName() const
{
    return sensorName;
}

std::unordered_map<unsigned int, sensor*>& sensor::getSensors()
{
    return sensors;
}

void sensor::updateSensorData(double time, double value) const
{
    const_cast<sensor*>(this)->addChartData(time, value); // Remove const to update data
    QString currentPath = QDir::currentPath();
    QFile file(currentPath + "/sensors_list/" + QString::number(getID()) + ".txt");
    if (file.open(QIODevice::Append | QIODevice::Text)) // Use Append instead of WriteOnly
    {
        QTextStream out(&file);
        out << time << "," << value << "\n";
    }
}

void sensor::addChartData(double time, double value)
{
    qDebug() << "Adding chart data: time =" << time << ", value =" << value;
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
        while (!in.atEnd())
        {
            QString line = in.readLine();
            if (line.startsWith("Chart Data:"))
            {
                break;
            }
            lines.append(line);
        }
        file.resize(0); // Clear the file content
        for (const QString& line : lines)
        {
            in << line << "\n";
        }
        in << "Chart Data:\n"; // Add the header for chart data
    }
}

void sensor::updateFile(std::string newName)
{
    QString currentPath = QDir::currentPath();
    QFile file(currentPath + "/sensors_list/" + QString::number(getID()) + ".txt");

    if (file.open(QIODevice::ReadWrite | QIODevice::Text))
    {
        QTextStream in(&file);
        QStringList lines;
        while (!in.atEnd())
        {
            QString line = in.readLine();
            if (line.startsWith("Name: "))
            {
                line = "Name: " + QString::fromStdString(newName);
            }
            lines.append(line);
        }
        file.resize(0); // Clear the file content
        for (const QString& line : lines)
        {
            in << line << "\n";
        }
    }
}

void sensor::loadChartDataFromFile()
{
    QString currentPath = QDir::currentPath();
    QFile file(currentPath + "/sensors_list/" + QString::number(getID()) + ".txt");

    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream in(&file);
        bool chartDataSection = false;
        while (!in.atEnd())
        {
            QString line = in.readLine();
            if (line.startsWith("Chart Data:"))
            {
                chartDataSection = true;
                continue;
            }

            if (chartDataSection)
            {
                QStringList parts = line.split(",");
                if (parts.size() == 2)
                {
                    double time = parts[0].toDouble();
                    double value = parts[1].toDouble();
                    chartData.emplace_back(time, value);
                }
            }
        }
    }
}

