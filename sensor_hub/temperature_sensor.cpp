#include "temperature_sensor.h"

#include <QDir>

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
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << "ID: " << getID() << "\n";
        out << "Type: " << "Temperature Sensor" << "\n";
        out << "Name: " << QString::fromStdString(getName()) << "\n";
        out << "Precision: " << getPrecision() << "\n";
    }
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
