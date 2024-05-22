#ifndef DUST_SENSOR_H
#define DUST_SENSOR_H

#include "sensor.h"
#include <vector>
#include <utility>

class dust_sensor : public sensor
{
private:
    double dustLevel;
    double precision;
    void createFile() const override;
    dust_sensor(std::string name, double p);
    dust_sensor(std::string name, unsigned int ID, double p);

    std::vector<std::pair<double, double>> chartData; // Aggiungi questo membro

public:
    static dust_sensor* create(std::string name, double p);
    static dust_sensor* create(std::string name, unsigned int ID, double p);
    double getDustLevel() const;
    double getPrecision() const override;
    dust_sensor* clone() const override;
    void setDustLevel(double level); // Metodo per aggiornare il livello di polvere

    void addChartData(double time, double value) override; // Dichiarazione del nuovo metodo
    std::vector<std::pair<double, double>> getChartData() const override; // Dichiarazione del nuovo metodo
    void clearChartData() override; // Dichiarazione del nuovo metodo
};

#endif // DUST_SENSOR_H
