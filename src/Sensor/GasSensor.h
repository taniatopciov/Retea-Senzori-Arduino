#ifndef GAS_SENSOR_H
#define GAS_SENSOR_H

#include "ISensor.h"

/**
 * Aceasta clasa este folosita pentru un senzor de Gaz
*/
class GasSensor : public ISensor
{
public:
    GasSensor(int pin);
    virtual ~GasSensor();

    SensorTypes GetType();
};

#endif // GAS_SENSOR_H