#include "load_sensors.h"
#include "sensor.h"
#include "dust_sensor.h"
#include "temperature_sensor.h"
#include "humidity_sensor.h"

#include <QDir>
#include <QFile>
#include <QTextStream>



void loadSensors() {
    QString currentPath = QDir::currentPath();
    QDir directory(currentPath + "/sensors_list");
    QStringList files = directory.entryList(QStringList() << "*.txt", QDir::Files);


    foreach(QString filename, files) {
        QFile file(directory.absoluteFilePath(filename));
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            unsigned int id = 0;
            std::string type, name;
            double precision = 0.0;

            while (!in.atEnd()) {
                QString line = in.readLine();
                QStringList parts = line.split(": ");
                if (parts[0] == "ID") {
                    id = parts[1].toUInt();
                } else if (parts[0] == "Type") {
                    type = parts[1].toStdString();
                } else if (parts[0] == "Name") {
                    name = parts[1].toStdString();
                } else if (parts[0] == "Precision") {
                    precision = parts[1].toDouble();
                }
            }
            if (type == "Dust Sensor") {
                dust_sensor* sensor = dust_sensor::create(name, id, precision);
            }
            else if (type == "Temperature Sensor") {
                temperature_sensor* sensor = temperature_sensor::create(name, id, precision);
            }
            else if (type == "Humidity Sensor") {
                humidity_sensor* sensor = humidity_sensor::create(name, id, precision);
            }
        }
    }
}


