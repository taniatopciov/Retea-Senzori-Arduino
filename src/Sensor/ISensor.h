#ifndef ISENSOR_H
#define ISENSOR_H

#include "SensorTypes.h"

/**
 * Aceasta clasa este folosita pentru a defini un senzor
 * Fiecare senzor trebuie sa implementeze functia GetType care returneaza tipul senzorului
*/
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