#ifndef HUMIDITY_SENSOR_H
#define HUMIDITY_SENSOR_H

#include "sensor.h"

class humidity_sensor : public sensor
{
private:
    double humidity;
    double precision;
    void createFile() const override;
    humidity_sensor(std::string name, double p);
    humidity_sensor(std::string name, unsigned int ID, double p);
public:
    static humidity_sensor* create(std::string name, double p);
    static humidity_sensor* create(std::string name, unsigned int ID, double p);
    double getHumidity() const;
    double getPrecision() const;
    humidity_sensor* clone() const override;
    std::string getSensorType() const override;
};

#endif // HUMIDITY_SENSOR_H
