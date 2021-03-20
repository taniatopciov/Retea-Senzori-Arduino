#ifndef RAIN_SENSOR_H
#define RAIN_SENSOR_H

#include "ISensor.h"

class RainSensor : public ISensor
{
public:
    RainSensor(int analogPin, int digitalPin);
    virtual ~RainSensor();
    float ReadValue();

    SensorTypes GetType();

private:
    int digitalPin;
};

#endif // RAIN_SENSOR_H