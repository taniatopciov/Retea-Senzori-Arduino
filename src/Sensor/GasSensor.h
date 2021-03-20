#ifndef GAS_SENSOR_H
#define GAS_SENSOR_H

#include "ISensor.h"

class GasSensor : public ISensor
{
public:
    GasSensor(int pin);
    virtual ~GasSensor();

    SensorTypes GetType();
};

#endif // GAS_SENSOR_H