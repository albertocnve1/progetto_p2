#include "humidity_sensor.h"

#include <QDir>

double humidity_sensor::getHumidity() const
{
    return humidity;
}

double humidity_sensor::getPrecision() const
{
    return precision;
}

humidity_sensor* humidity_sensor::clone() const
{
    return new humidity_sensor(*this);
}

void humidity_sensor::setHumidity(double level)
{
    humidity = level;
}

void humidity_sensor::createFile() const
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
            out << "Type: " << "Humidity Sensor" << "\n";
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


void humidity_sensor::addChartData(double time, double value) {
    chartData.emplace_back(time, value);
}

const std::vector<std::pair<double, double>>& humidity_sensor::getChartData() const {
    return chartData;
}

void humidity_sensor::clearChartData() {
    chartData.clear();
}

humidity_sensor::humidity_sensor(std::string name, double p) : sensor(name), precision(p) {}
humidity_sensor::humidity_sensor(std::string name, unsigned int ID, double p) : sensor(name, ID), precision(p) {}

humidity_sensor* humidity_sensor::create(std::string name, double p)
{
    humidity_sensor* obj = new humidity_sensor(name, p);
    obj->createFile();
    return obj;
}

humidity_sensor* humidity_sensor::create(std::string name, unsigned int ID, double p)
{
    humidity_sensor* obj = new humidity_sensor(name, ID, p);
    obj->createFile();
    return obj;
}
