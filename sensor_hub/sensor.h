#ifndef SENSOR_H
#define SENSOR_H

#include <string>
#include <unordered_map>

class sensor
{
private:
    unsigned int sensorID;
    std::string sensorName;
    static unsigned int nextID;
    static std::unordered_map<unsigned int, sensor*> sensors;
protected:
    sensor(std::string name);
    sensor(std::string name, unsigned int ID);
    virtual void createFile() const = 0;
public:
    virtual ~sensor();
    virtual sensor* clone() const = 0;
    void setName(std::string newName);
    unsigned int getID() const;
    std::string getName() const;
    virtual std::string getSensorType() const = 0;
    virtual double getPrecision() const = 0;
    static std::unordered_map<unsigned int, sensor*>& getSensors();
};

#endif // SENSOR_H
