#ifndef TEMPERATURE_SENSOR_H
#define TEMPERATURE_SENSOR_H

#include "sensor.h"
#include <vector>
#include <utility>

class temperature_sensor : public sensor
{
private:
    double temperature;
    double precision;
    void createFile() const override;
    temperature_sensor(std::string name, double p);
    temperature_sensor(std::string name, unsigned int ID, double p);

    std::vector<std::pair<double, double>> chartData; // Aggiungi questo membro

public:
    static temperature_sensor* create(std::string name, double p);
    static temperature_sensor* create(std::string name, unsigned int ID, double p);
    double getTemperature() const;
    double getPrecision() const override;
    temperature_sensor* clone() const override;
    void setTemperature(double level); // Metodo per aggiornare la temperatura

    void addChartData(double time, double value); // Dichiarazione del nuovo metodo
    const std::vector<std::pair<double, double>>& getChartData() const; // Dichiarazione del nuovo metodo
    void clearChartData(); // Dichiarazione del nuovo metodo
};

#endif // TEMPERATURE_SENSOR_H
