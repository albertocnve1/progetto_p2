#ifndef TEMPERATURE_SENSOR_H
#define TEMPERATURE_SENSOR_H

#include "sensor.h"

class temperature_sensor : public sensor
{
private:
    double temperature;
    double precision;
    void createFile() const override;
    temperature_sensor(std::string, double);
    temperature_sensor(std::string, unsigned int, double);

public:
    static temperature_sensor* create(std::string, double);
    static temperature_sensor* create(std::string, unsigned int, double);
    double getTemperature() const;
    double getPrecision() const override;
    temperature_sensor* clone() const override;
    void setTemperature(double); // Metodo per aggiornare la temperatura
};

#endif // TEMPERATURE_SENSOR_H
