#ifndef SENSOR_H
#define SENSOR_H

#include <string>

class sensor
{
private:
    unsigned int sensorID;
    std::string sensorName;
    double precision;
    static unsigned int nextID;
    static std::unordered_map<unsigned int, sensor *> sensors;

protected:
    sensor(std::string, double);
    sensor(std::string, unsigned int, double);
    std::vector<std::pair<double, double>> chartData;

public:
    static std::unordered_map<unsigned int, sensor *> &getSensors();
    virtual ~sensor();
    virtual sensor *clone() const = 0;
    virtual void createFile() const = 0;
    unsigned int getID() const;
    std::string getName() const;
    double getPrecision() const;
    void setName(std::string);
    void updateFile(std::string);
    void addChartData(double, double);
    void clearChartData();
    void updateSensorData(double, double) const;
    void loadChartDataFromFile();
    std::vector<std::pair<double, double>> getChartData() const;
};

#endif // SENSOR_H
