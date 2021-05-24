#ifndef TEMP_SENSOR_H
#define TEMP_SENSOR_H

#include "ISensor.h"
/**
 * Aceasta clasa este folosita pentru un senzor de Temperatura DHT
*/
class TempSensor : public ISensor
{
public:
    TempSensor(int pin);
    virtual ~TempSensor();
    float ReadValue();

    SensorTypes GetType();

private:
    float backupTemp;
};

#endif // TEMP_SENSOR_H