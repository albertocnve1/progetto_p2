#ifndef DUST_SENSOR_H
#define DUST_SENSOR_H

#include "sensor.h"

class dust_sensor : public sensor
{
private:
    double dustLevel;
    double precision;
    void createFile() const override;
    dust_sensor(std::string name, double p);
    dust_sensor(std::string name, unsigned int ID, double p);
public:
    static dust_sensor* create(std::string name, double p);
    static dust_sensor* create(std::string name, unsigned int ID, double p);
    double getDustLevel() const;
    double getPrecision() const;
    dust_sensor* clone() const override;
    std::string getSensorType() const override;
};

#endif // DUST_SENSOR_H
