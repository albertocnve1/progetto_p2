#include "temperature_sensor.h"

#include <QDir>
#include <QFile>
#include <QTextStream>

double temperature_sensor::getTemperature() const
{
    return temperature;
}

double temperature_sensor::getPrecision() const
{
    return precision;
}

temperature_sensor* temperature_sensor::clone() const
{
    return new temperature_sensor(*this);
}

void temperature_sensor::setTemperature(double level)
{
    temperature = level;
}

void temperature_sensor::createFile() const
{
    QString currentPath = QDir::currentPath();
    QDir dir;
    dir.mkdir(currentPath + "/sensors_list");
    QFile file(currentPath + "/sensors_list/" + QString::number(getID()) + ".txt");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)) // Cambiato in Append
    {
        QTextStream out(&file);
        if (file.size() == 0) { // Scrive l'intestazione solo se il file è vuoto
            out << "ID: " << getID() << "\n";
            out << "Type: " << "Temperature Sensor" << "\n";
            out << "Name: " << QString::fromStdString(getName()) << "\n";
            out << "Precision: " << getPrecision() << "\n";
            out << "Chart Data:\n";
        }

        for (const auto& point : chartData)
        {
            out << point.first << "," << point.second << "\n";
        }
    }
}


void temperature_sensor::addChartData(double time, double value) {
    chartData.emplace_back(time, value);
}

std::vector<std::pair<double, double>> temperature_sensor::getChartData() const {
    return chartData;
}

void temperature_sensor::clearChartData() {
    chartData.clear();
}

temperature_sensor::temperature_sensor(std::string name, double p) : sensor(name), precision(p) {}
temperature_sensor::temperature_sensor(std::string name, unsigned int ID, double p) : sensor(name, ID), precision(p) {}

temperature_sensor* temperature_sensor::create(std::string name, double p)
{
    temperature_sensor* obj = new temperature_sensor(name, p);
    obj->createFile();
    return obj;
}

temperature_sensor* temperature_sensor::create(std::string name, unsigned int ID, double p)
{
    temperature_sensor* obj = new temperature_sensor(name, ID, p);
    obj->createFile();
    return obj;
}
