#include "sensor_data_handler.h"

void SensorDataHandler::handleNewSensorData(int sensorId, double time, double value, QChartView *chartView, QLabel *currentValueLabel, QWidget *parent)
{
    std::unordered_map<unsigned int, sensor *> &sensors = sensor::getSensors();
    auto it = sensors.find(sensorId);
    if (it != sensors.end())
    {
        sensor *s = it->second;

        // Aggiorna il valore della misurazione attuale
        if (dynamic_cast<dust_sensor *>(s))
        {
            dynamic_cast<dust_sensor *>(s)->setDustLevel(value);
            currentValueLabel->setText(QString("Livello PM10 attuale: %1 μg/m³").arg(value));
        }
        else if (dynamic_cast<temperature_sensor *>(s))
        {
            dynamic_cast<temperature_sensor *>(s)->setTemperature(value);
            currentValueLabel->setText(QString("Temperatura attuale: %1 °C").arg(value));
        }
        else if (dynamic_cast<humidity_sensor *>(s))
        {
            dynamic_cast<humidity_sensor *>(s)->setHumidity(value);
            currentValueLabel->setText(QString("Livello umidità attuale: %1 %").arg(value));
        }

        QLineSeries *series;
        QChart *chart = chartView->chart();
        if (chart && chart->series().size() > 0)
        {
            series = qobject_cast<QLineSeries *>(chart->series().at(0));
        }
        else
        {
            series = new QLineSeries();
            chart = new QChart();
            chart->addSeries(series);
            chart->setTitle("Simulazione del sensore " + QString::fromStdString(s->getName()));

            QValueAxis *axisX = new QValueAxis;
            axisX->setTitleText("Tempo (s)");
            QValueAxis *axisY = new QValueAxis;

            // Usare dynamic_cast per determinare il tipo di sensore e impostare i limiti dell'asse Y
            if (dynamic_cast<dust_sensor *>(s))
            {
                axisY->setRange(0, 50);
                axisY->setTitleText("PM10 (µg/m³)");
            }
            else if (dynamic_cast<temperature_sensor *>(s))
            {
                axisY->setRange(-20, 100);
                axisY->setTitleText("°C");
            }
            else if (dynamic_cast<humidity_sensor *>(s))
            {
                axisY->setRange(0, 100);
                axisY->setTitleText("% umidità nell'aria");
            }

            chart->addAxis(axisX, Qt::AlignBottom);
            chart->addAxis(axisY, Qt::AlignLeft);
            series->attachAxis(axisX);
            series->attachAxis(axisY);

            chart->legend()->hide(); // Nascondere la leggenda

            // Imposta un intervallo iniziale di 20 secondi per l'asse X

            chartView->setChart(chart);
        }

        series->append(time, value);

        // Aggiorna gli intervalli dell'asse X per assicurare che i dati siano visibili
        QList<QAbstractAxis *> axesX = chart->axes(Qt::Horizontal, series);
        if (!axesX.isEmpty())
        {
            QValueAxis *axisX = qobject_cast<QValueAxis *>(axesX.first());
            if (axisX)
            {
                double currentMax = axisX->max();
                if (time > currentMax)
                {
                    axisX->setRange(0, currentMax * 2); // Raddoppia l'intervallo dell'asse X
                }
            }
        }

        // Aggiorna il titolo dell'asse Y
        QList<QAbstractAxis *> axesY = chart->axes(Qt::Vertical, series);
        if (!axesY.isEmpty())
        {
            QValueAxis *axisY = qobject_cast<QValueAxis *>(axesY.first());
            if (axisY)
            {
                // Usare dynamic_cast per determinare il tipo di sensore e impostare il titolo dell'asse Y
                if (dynamic_cast<dust_sensor *>(s))
                {
                    axisY->setTitleText("PM10 (µg/m³)");
                }
                else if (dynamic_cast<temperature_sensor *>(s))
                {
                    axisY->setTitleText("°C");
                }
                else if (dynamic_cast<humidity_sensor *>(s))
                {
                    axisY->setTitleText("% umidità nell'aria");
                }
            }
        }

        // Nascondi la leggenda ogni volta che il grafico viene aggiornato
        if (chart)
        {
            chart->legend()->hide();
        }
    }
    else
    {
        QMessageBox::warning(parent, QObject::tr("Errore"), QObject::tr("Sensore non trovato"));
    }
}
