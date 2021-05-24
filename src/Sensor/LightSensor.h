#ifndef LIGHT_SENSOR_H
#define LIGHT_SENSOR_H

#include "ISensor.h"

/**
 * Aceasta clasa este folosita pentru un senzor de Intensitate Luminoasa
*/
class LightSensor : public ISensor
{
public:
    LightSensor(int pin);
    virtual ~LightSensor();

    SensorTypes GetType();
};

#endif // LIGHT_SENSOR_H