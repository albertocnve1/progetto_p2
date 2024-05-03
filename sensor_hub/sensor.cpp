#include "sensor.h"


unsigned int sensor::nextID = 0;
std::unordered_map<unsigned int, sensor*> sensor::sensors;

sensor::sensor(std::string name) : sensorName(name)
{
    while (sensors.find(nextID) != sensors.end()) {
        ++nextID;
    }
    sensorID = nextID;
    sensors[sensorID] = this;
    ++nextID;
}

sensor::sensor(std::string name, unsigned int ID) {
    if (sensors.find(ID) != sensors.end())
    {
        return; // in caso il sensore con l'id specificato esista giò, non verrà aggiunto
    }
    sensorID = ID;
    sensorName = name;
    sensors[ID] = this;
}

unsigned int sensor::getID() const
{
    return sensorID;
}

std::string sensor::getName() const
{
    return sensorName;
}

std::unordered_map<unsigned int, sensor*>& sensor::getSensors() {
    return sensors;
}


