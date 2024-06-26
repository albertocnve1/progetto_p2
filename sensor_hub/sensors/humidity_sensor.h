#ifndef HUMIDITY_SENSOR_H
#define HUMIDITY_SENSOR_H

#include "sensor.h"

class humidity_sensor : public sensor
{
private:
    double humiditypercentage;
    void createFile() const override;
    humidity_sensor(std::string, double);
    humidity_sensor(std::string, unsigned int, double);

public:
    static humidity_sensor *create(std::string, double);
    static humidity_sensor *create(std::string, unsigned int, double);
    double getHumidity() const;
    humidity_sensor *clone() const override;
    void setHumidity(double);
};

#endif // HUMIDITY_SENSOR_H
