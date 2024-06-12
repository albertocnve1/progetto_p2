#include "dust_sensor.h"
#include <QDir>

void dust_sensor::createFile() const
{
    QString currentPath = QDir::currentPath();
    QDir dir;
    dir.mkdir(currentPath + "/sensors_list");
    QFile file(currentPath + "/sensors_list/" + QString::number(getID()) + ".txt");

    // se il file esiste già non veiene sovrascritto
    if (file.exists())
    {
        return;
    }

    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream out(&file);
        out << "ID: " << getID() << "\n";
        out << "Type: " << "Dust Sensor" << "\n";
        out << "Name: " << QString::fromStdString(getName()) << "\n";
        out << "Precision: " << getPrecision() << "\n";
        out << "Chart Data:\n";

        for (const auto &point : getChartData())
        {
            out << point.first << "," << point.second << "\n";
        }

        file.close();
    }
}

dust_sensor::dust_sensor(std::string name, double p) : sensor(name, p) {}
dust_sensor::dust_sensor(std::string name, unsigned int ID, double p) : sensor(name, ID, p) {}

dust_sensor *dust_sensor::create(std::string name, double p)
{
    dust_sensor *obj = new dust_sensor(name, p);
    obj->createFile();
    return obj;
}

dust_sensor *dust_sensor::create(std::string name, unsigned int ID, double p)
{
    dust_sensor *obj = new dust_sensor(name, ID, p);
    obj->createFile();
    return obj;
}

double dust_sensor::getDustLevel() const
{
    return dustLevel;
}


dust_sensor *dust_sensor::clone() const
{
    return new dust_sensor(*this);
}

void dust_sensor::setDustLevel(double level)
{
    dustLevel = level;
}
