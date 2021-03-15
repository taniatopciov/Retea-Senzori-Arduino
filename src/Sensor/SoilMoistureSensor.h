#ifndef SOIL_MOISTURE_SENSOR_H
#define SOIL_MOISTURE_SENSOR_H

#include "ISensor.h"

class SoilMoistureSensor : public ISensor
{
public:
    SoilMoistureSensor(int pin);
    virtual ~SoilMoistureSensor();
    float ReadValue();
};

#endif // SOIL_MOISTURE_SENSOR_H