#include "temperature_sensor.h"
#include <QDir>

void temperature_sensor::createFile() const
{
    QString currentPath = QDir::currentPath();
    QDir dir;
    dir.mkdir(currentPath + "/sensors_list");
    QFile file(currentPath + "/sensors_list/" + QString::number(getID()) + ".txt");

    // Se il file esiste già non viene sovrascritto in modo da preservare la persistenza dei dati
    if (file.exists())
    {
        return; //
    }

    // Open the file in write mode (overwrite if exists)
    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream out(&file);
        out << "ID: " << getID() << "\n";
        out << "Type: " << "Temperature Sensor" << "\n";
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

temperature_sensor::temperature_sensor(std::string name, double p) : sensor(name, p) {}
temperature_sensor::temperature_sensor(std::string name, unsigned int ID, double p) : sensor(name, ID, p) {}

temperature_sensor *temperature_sensor::create(std::string name, double p)
{
    temperature_sensor *obj = new temperature_sensor(name, p);
    obj->createFile();
    return obj;
}

temperature_sensor *temperature_sensor::create(std::string name, unsigned int ID, double p)
{
    temperature_sensor *obj = new temperature_sensor(name, ID, p);
    obj->createFile();
    return obj;
}

double temperature_sensor::getTemperature() const
{
    return temperature;
}

temperature_sensor *temperature_sensor::clone() const
{
    return new temperature_sensor(*this);
}

void temperature_sensor::setTemperature(double level)
{
    temperature = level;
}
