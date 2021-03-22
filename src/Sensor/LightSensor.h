#ifndef LIGHT_SENSOR_H
#define LIGHT_SENSOR_H

#include "ISensor.h"

class LightSensor : public ISensor
{
public:
    LightSensor(int pin);
    virtual ~LightSensor();
    //float ReadValue();

    SensorTypes GetType();
};

#endif // LIGHT_SENSOR_H