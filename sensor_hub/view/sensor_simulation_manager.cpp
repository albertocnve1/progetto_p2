#include "sensor_simulation_manager.h"
#include "sensors/sensor.h"
#include "sensors/dust_sensor.h"
#include "sensors/temperature_sensor.h"
#include "sensors/humidity_sensor.h"
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QDateTime>

SensorSimulationManager::SensorSimulationManager(QChartView *chartView, QLabel *detailsLabel, QLabel *currentValueLabel, QListWidget *listWidget, QWidget *parent)
    : QObject(parent), chartView(chartView), detailsLabel(detailsLabel), currentValueLabel(currentValueLabel), listWidget(listWidget), parent(parent)
{
    sensorSimulation = new SensorSimulation(this);
    connect(sensorSimulation, &SensorSimulation::newSensorData, this, [this](int sensorId, double time, double value)
            {
        Q_UNUSED(sensorId);
        Q_UNUSED(time);
        Q_UNUSED(value);
        displaySensorDetails(); });
}

void SensorSimulationManager::displaySensorDetails()
{
    QListWidgetItem *selectedItem = listWidget->currentItem();
    if (!selectedItem)
    {
        detailsLabel->setText("Dettagli del sensore qui");
        currentValueLabel->setText("");
        chartView->setChart(nullptr);
        return;
    }

    unsigned int id = selectedItem->text().split(":")[0].toUInt();
    std::unordered_map<unsigned int, sensor *> &sensors = sensor::getSensors();
    auto it = sensors.find(id);
    if (it != sensors.end())
    {
        sensor *s = it->second;

        QString details = QString("ID: %1\nNome: %2\nPrecisione: %3").arg(s->getID()).arg(QString::fromStdString(s->getName())).arg(s->getPrecision());

        // Creazione di un nuovo grafico
        QChart *chart = new QChart();
        QLineSeries *series = new QLineSeries();

        if (dynamic_cast<dust_sensor *>(s))
        {
            details += "\nTipo: Dust Sensor";
            currentValueLabel->setText(QString("Livello PM10 attuale: %1 μg/m³").arg(dynamic_cast<dust_sensor *>(s)->getDustLevel()));

            for (const auto &data : dynamic_cast<dust_sensor *>(s)->getChartData())
            {
                series->append(data.first, data.second);
            }
            chart->addSeries(series);
            chart->setTitle("Simulazione del sensore " + QString::fromStdString(s->getName()));

            QValueAxis *axisX = new QValueAxis;
            axisX->setTitleText("Tempo (s)");
            QValueAxis *axisY = new QValueAxis;
            axisY->setRange(0, 50);
            axisY->setTitleText("PM10 (µg/m³)");

            chart->addAxis(axisX, Qt::AlignBottom);
            chart->addAxis(axisY, Qt::AlignLeft);
            series->attachAxis(axisX);
            series->attachAxis(axisY);
        }
        else if (dynamic_cast<temperature_sensor *>(s))
        {
            details += "\nTipo: Temperature Sensor";
            currentValueLabel->setText(QString("Temperatura attuale: %1 °C").arg(dynamic_cast<temperature_sensor *>(s)->getTemperature()));

            for (const auto &data : dynamic_cast<temperature_sensor *>(s)->getChartData())
            {
                series->append(data.first, data.second);
            }
            chart->addSeries(series);
            chart->setTitle("Simulazione del sensore " + QString::fromStdString(s->getName()));

            QValueAxis *axisX = new QValueAxis;
            axisX->setTitleText("Tempo (s)");
            QValueAxis *axisY = new QValueAxis;
            axisY->setRange(-20, 100);
            axisY->setTitleText("°C");

            chart->addAxis(axisX, Qt::AlignBottom);
            chart->addAxis(axisY, Qt::AlignLeft);
            series->attachAxis(axisX);
            series->attachAxis(axisY);
        }
        else if (dynamic_cast<humidity_sensor *>(s))
        {
            details += "\nTipo: Humidity Sensor";
            currentValueLabel->setText(QString("Livello umidità attuale: %1 %").arg(dynamic_cast<humidity_sensor *>(s)->getHumidity()));

            for (const auto &data : dynamic_cast<humidity_sensor *>(s)->getChartData())
            {
                series->append(data.first, data.second);
            }
            chart->addSeries(series);
            chart->setTitle("Simulazione del sensore " + QString::fromStdString(s->getName()));

            QValueAxis *axisX = new QValueAxis;
            axisX->setTitleText("Tempo (s)");
            QValueAxis *axisY = new QValueAxis;
            axisY->setRange(0, 100);
            axisY->setTitleText("% umidità nell'aria");

            chart->addAxis(axisX, Qt::AlignBottom);
            chart->addAxis(axisY, Qt::AlignLeft);
            series->attachAxis(axisX);
            series->attachAxis(axisY);
        }

        chart->legend()->hide();

        if (series->count() > 0)
        {
            chart->axes(Qt::Horizontal).first()->setRange(0, series->at(series->count() - 1).x());
        }
        else
        {
            chart->axes(Qt::Horizontal).first()->setRange(0, 20);
        }

        chartView->setChart(chart);
        detailsLabel->setText(details);
    }
}

void SensorSimulationManager::startSimulation()
{
    QListWidgetItem *selectedItem = listWidget->currentItem();
    if (!selectedItem)
    {
        QMessageBox::warning(parent, tr("Errore"), tr("Nessun sensore selezionato"));
        return;
    }

    unsigned int id = selectedItem->text().split(":")[0].toUInt();
    std::unordered_map<unsigned int, sensor *> &sensors = sensor::getSensors();
    auto it = sensors.find(id);
    if (it != sensors.end())
    {
        sensor *s = it->second;

        // Verifica se ci sono dati relativi a una simulazione passata
        if (!s->getChartData().empty())
        {
            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(parent, tr("Conferma"),
                                          tr("Avviando una nuova simulazione i dati di simulazioni passate verranno sovrascritti, continuare?"),
                                          QMessageBox::Yes | QMessageBox::No);
            if (reply == QMessageBox::No)
            {
                return;
            }
        }

        // Cancella i dati del grafico precedente sia dal grafico che dal file
        s->clearChartData();

        QChart *chart = new QChart();
        QLineSeries *series = new QLineSeries();
        chart->addSeries(series);
        chart->setTitle("Simulazione del sensore " + QString::fromStdString(s->getName()));

        QValueAxis *axisX = new QValueAxis;
        axisX->setTitleText("Tempo (s)");
        QValueAxis *axisY = new QValueAxis;

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

        chart->legend()->hide();

        axisX->setRange(0, 20);

        chartView->setChart(chart);

        sensorSimulation->simulateSensor(id);
    }
    else
    {
        QMessageBox::warning(parent, tr("Errore"), tr("Sensore non trovato"));
    }
}

void SensorSimulationManager::stopSimulation()
{
    sensorSimulation->stopSimulation();
}
