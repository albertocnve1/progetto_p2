#ifndef HUMIDITY_SENSOR_H
#define HUMIDITY_SENSOR_H

#include "sensor.h"
#include <vector>
#include <utility>

class humidity_sensor : public sensor
{
private:
    double humidity;
    double precision;
    void createFile() const override;
    humidity_sensor(std::string name, double p);
    humidity_sensor(std::string name, unsigned int ID, double p);

    std::vector<std::pair<double, double>> chartData; // Aggiungi questo membro

public:
    static humidity_sensor* create(std::string name, double p);
    static humidity_sensor* create(std::string name, unsigned int ID, double p);
    double getHumidity() const;
    double getPrecision() const override;
    humidity_sensor* clone() const override;
    void setHumidity(double level); // Metodo per aggiornare il livello di umidità

    void addChartData(double time, double value) override; // Dichiarazione del nuovo metodo
    std::vector<std::pair<double, double>> getChartData() const override; // Dichiarazione del nuovo metodo
    void clearChartData() override; // Dichiarazione del nuovo metodo marcato come const
};

#endif // HUMIDITY_SENSOR_H
