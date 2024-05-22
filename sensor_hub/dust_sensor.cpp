#include "dust_sensor.h"

#include <QDir>

double dust_sensor::getDustLevel() const
{
    return dustLevel;
}

double dust_sensor::getPrecision() const
{
    return precision;
}

dust_sensor* dust_sensor::clone() const
{
    return new dust_sensor(*this);
}

void dust_sensor::setDustLevel(double level)
{
    dustLevel = level;
}

void dust_sensor::createFile() const
{
    QString currentPath = QDir::currentPath();
    QDir dir;
    dir.mkdir(currentPath + "/sensors_list");
    QFile file(currentPath + "/sensors_list/" + QString::number(getID()) + ".txt");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << "ID: " << getID() << "\n";
        out << "Type: " << "Dust Sensor" << "\n";
        out << "Name: " << QString::fromStdString(getName()) << "\n";
        out << "Precision: " << getPrecision() << "\n";
    }
}

dust_sensor::dust_sensor(std::string name, double p) : sensor(name), precision(p) {}
dust_sensor::dust_sensor(std::string name, unsigned int ID ,double p ) : sensor(name, ID), precision(p) {}

dust_sensor* dust_sensor::create(std::string name, double p)
{
    dust_sensor* obj = new dust_sensor(name, p);
    obj->createFile();
    return obj;
}

dust_sensor* dust_sensor::create(std::string name, unsigned int ID, double p)
{
    dust_sensor* obj = new dust_sensor(name, ID, p);
    obj->createFile();
    return obj;
}
