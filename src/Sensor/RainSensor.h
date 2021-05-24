#ifndef RAIN_SENSOR_H
#define RAIN_SENSOR_H

#include "ISensor.h"

/**
 * Aceasta clasa este folosita pentru un senzor de Picaturi de ploaie
 * Acest senzor are 2 pini:
 *  - unul analogic
 *  - unul digital
 * 
 * Pinul digital indica daca senzorul a detectat picaturile de ploaie
 * Pinul analogic indica cat de multe picaturi de ploaie au fost detectate
*/
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