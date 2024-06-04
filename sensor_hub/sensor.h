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
    static std::unordered_map<unsigned int, sensor *> sensors;

protected:
    sensor(std::string);
    sensor(std::string, unsigned int);
    std::vector<std::pair<double, double>> chartData;

public:
    static std::unordered_map<unsigned int, sensor *> &getSensors();
    virtual ~sensor();
    virtual sensor *clone() const = 0;
    virtual void createFile() const = 0;
    virtual double getPrecision() const = 0;
    unsigned int getID() const;
    std::string getName() const;
    void setName(std::string);
    void updateFile(std::string);
    void addChartData(double, double);
    void clearChartData();
    void updateSensorData(double, double) const;
    void loadChartDataFromFile();
    std::vector<std::pair<double, double>> getChartData() const;
};

#endif // SENSOR_H
