#ifndef ISENSOR_H
#define ISENSOR_H

#include "SensorTypes.h"

class ISensor
{
public:
    ISensor(int pin);
    virtual ~ISensor();
    virtual float ReadValue();

    virtual SensorTypes GetType() = 0;

protected:
    int pin;
};

#endif // ISENSOR_H