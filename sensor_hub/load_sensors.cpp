#include "load_sensors.h"
#include "dust_sensor.h"
#include "temperature_sensor.h"
#include "humidity_sensor.h"
#include <QDir>

void loadSensors() {
    QString currentPath = QDir::currentPath();
    QDir directory(currentPath + "/sensors_list");
    QStringList files = directory.entryList(QStringList() << "*.txt", QDir::Files);

    foreach(QString filename, files)
    {
        QFile file(directory.absoluteFilePath(filename));
        if (file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QTextStream in(&file);
            unsigned int id = 0;
            std::string type, name;
            double precision = 0.0;
            std::vector<std::pair<double, double>> chartData;

            while (!in.atEnd())
            {
                QString line = in.readLine().trimmed();
                QStringList parts = line.split(": ");
                if (parts[0] == "ID")
                {
                    id = parts[1].toUInt();
                }
                else if (parts[0] == "Type")
                {
                    type = parts[1].toStdString();
                }
                else if (parts[0] == "Name")
                {
                    name = parts[1].toStdString();
                }
                else if (parts[0] == "Precision")
                {
                    precision = parts[1].toDouble();
                }
                else if (parts[0] == "Chart Data")
                {
                    while (!in.atEnd())
                    {
                        QString dataLine = in.readLine().trimmed();
                        if (dataLine.isEmpty()) continue; // Ignora righe vuote
                        QStringList dataParts = dataLine.split(",");
                        if (dataParts.size() == 2)
                        {
                            bool timeOk, valueOk;
                            double time = dataParts[0].toDouble(&timeOk);
                            double value = dataParts[1].toDouble(&valueOk);
                            if (timeOk && valueOk) {
                                chartData.emplace_back(time, value);
                            }
                        }
                    }
                }
            }

            if (type == "Dust Sensor")
            {
                dust_sensor* sensor = dust_sensor::create(name, id, precision);
                for (const auto& data : chartData)
                {
                    sensor->addChartData(data.first, data.second);
                }
            }
            else if (type == "Temperature Sensor")
            {
                temperature_sensor* sensor = temperature_sensor::create(name, id, precision);
                for (const auto& data : chartData)
                {
                    sensor->addChartData(data.first, data.second);
                }
            }
            else if (type == "Humidity Sensor")
            {
                humidity_sensor* sensor = humidity_sensor::create(name, id, precision);
                for (const auto& data : chartData)
                {
                    sensor->addChartData(data.first, data.second);
                }
            }
        }
    }
}
