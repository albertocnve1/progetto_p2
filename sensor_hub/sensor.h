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
    sensor(std::string);
    sensor(std::string, unsigned int);
    std::vector<std::pair<double, double>> chartData; // Sposta qui per rendere accessibile a tutte le classi derivate

public:
    virtual ~sensor();
    virtual sensor* clone() const = 0;
    virtual void createFile() const = 0;
    void setName(std::string);
    unsigned int getID() const;
    std::string getName() const;
    virtual double getPrecision() const = 0;
    void updateFile(std::string newName);
    static std::unordered_map<unsigned int, sensor*>& getSensors();

    void addChartData(double, double); // Rimuovi const
    std::vector<std::pair<double, double>> getChartData() const;
    void clearChartData();

    // Nuovo metodo per aggiornare i dati del sensore
    void updateSensorData(double, double) const;
};

#endif // SENSOR_H
