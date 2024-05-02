#ifndef TEMPERATURE_SENSOR_H
#define TEMPERATURE_SENSOR_H

#include "sensor.h"

class temperature_sensor : public sensor
{
private:
    double temperature;
    double precision;
    void createFile() const override;
    temperature_sensor(std::string name, double p);
    temperature_sensor(std::string name, unsigned int ID, double p);
public:
    static temperature_sensor* create(std::string name, double p);
    static temperature_sensor* create(std::string name, unsigned int ID, double p);
    double getTemperature() const;
    double getPrecision() const;
    temperature_sensor* clone() const override;
    std::string getSensorType() const override;
};

#endif // TEMPERATURE_SENSOR_H
