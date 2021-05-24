#ifndef TEMP_SENSOR_H
#define TEMP_SENSOR_H

#include "ISensor.h"

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