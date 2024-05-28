#include "load_sensors.h"
#include "dust_sensor.h"
#include "temperature_sensor.h"
#include "humidity_sensor.h"
#include <QDir>
#include <QDebug>

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

            qDebug() << "Reading file:" << filename;

            while (!in.atEnd())
            {
                QString line = in.readLine().trimmed();
                qDebug() << "Line read:" << line;
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
                    qDebug() << "Inizio lettura dati del grafico per il sensore:" << QString::fromStdString(name);
                    while (!in.atEnd())
                    {
                        QString dataLine = in.readLine().trimmed();
                        qDebug() << "Data line read:" << dataLine;
                        if (dataLine.isEmpty()) continue; // Ignora righe vuote
                        QStringList dataParts = dataLine.split(",");
                        qDebug() << "Parsing chart data line:" << dataLine;
                        if (dataParts.size() == 2)
                        {
                            bool timeOk, valueOk;
                            double time = dataParts[0].toDouble(&timeOk);
                            double value = dataParts[1].toDouble(&valueOk);
                            if (timeOk && valueOk) {
                                chartData.emplace_back(time, value);
                                qDebug() << "Loaded chart data for sensor" << QString::fromStdString(name) << ": time =" << time << ", value =" << value;
                            } else {
                                qDebug() << "Errore nel parsing dei dati del grafico:" << dataLine;
                            }
                        } else {
                            qDebug() << "Formato dati del grafico non valido:" << dataLine;
                        }
                    }
                }
            }

            qDebug() << "Loading Sensor:" << QString::fromStdString(name) << "ID:" << id << "Type:" << QString::fromStdString(type) << "Chart Data Size:" << chartData.size();

            if (type == "Dust Sensor")
            {
                dust_sensor* sensor = dust_sensor::create(name, id, precision);
                for (const auto& data : chartData)
                {
                    sensor->addChartData(data.first, data.second);
                }
                qDebug() << "Loaded Dust Sensor:" << QString::fromStdString(name) << "with chart data size:" << sensor->getChartData().size();
            }
            else if (type == "Temperature Sensor")
            {
                temperature_sensor* sensor = temperature_sensor::create(name, id, precision);
                for (const auto& data : chartData)
                {
                    sensor->addChartData(data.first, data.second);
                }
                qDebug() << "Loaded Temperature Sensor:" << QString::fromStdString(name) << "with chart data size:" << sensor->getChartData().size();
            }
            else if (type == "Humidity Sensor")
            {
                humidity_sensor* sensor = humidity_sensor::create(name, id, precision);
                for (const auto& data : chartData)
                {
                    sensor->addChartData(data.first, data.second);
                }
                qDebug() << "Loaded Humidity Sensor:" << QString::fromStdString(name) << "with chart data size:" << sensor->getChartData().size();
            }
        }
        else
        {
            qDebug() << "Failed to open file:" << filename;
        }
    }
}
