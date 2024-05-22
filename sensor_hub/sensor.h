#ifndef SENSOR_H
#define SENSOR_H

#include <string>
#include <unordered_map>
#include <vector>
#include <utility>

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
public:
    virtual ~sensor();
    virtual sensor* clone() const = 0;
    virtual void createFile() const = 0;
    void setName(std::string newName);
    unsigned int getID() const;
    std::string getName() const;
    virtual double getPrecision() const = 0;
    static std::unordered_map<unsigned int, sensor*>& getSensors();

    virtual void addChartData(double time, double value) = 0;
    virtual std::vector<std::pair<double, double>> getChartData() const = 0;
    virtual void clearChartData() = 0;
};

#endif // SENSOR_H
