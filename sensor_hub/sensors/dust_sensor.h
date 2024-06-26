#ifndef DUST_SENSOR_H
#define DUST_SENSOR_H

#include "sensor.h"

class dust_sensor : public sensor
{
private:
    double dustLevel;
    void createFile() const override;
    dust_sensor(std::string, double);
    dust_sensor(std::string, unsigned int, double);

public:
    static dust_sensor *create(std::string, double);
    static dust_sensor *create(std::string, unsigned int, double);
    double getDustLevel() const;
    dust_sensor *clone() const override;
    void setDustLevel(double);
};

#endif // DUST_SENSOR_H
