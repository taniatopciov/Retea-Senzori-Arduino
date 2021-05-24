#ifndef SIMPLE_TEMP_SENSOR_H
#define SIMPLE_TEMP_SENSOR_H

#include "ISensor.h"

/**
 * Aceasta clasa este folosita pentru un senzor de Temperatura
 * Senzorul citeste temperatura relativa
 * Mai jos este descris procesul de a converti de la temperatura relativa la cea absoluta
*/
class SimpleTempSensor : public ISensor
{
public:
    SimpleTempSensor(int pin);
    virtual ~SimpleTempSensor();
    float ReadValue();

    SensorTypes GetType();
};

#endif // SIMPLE_TEMP_SENSOR_H

/*
    Am calculat 2 punte: A(36,4C; 160) si B(34,7C; 170)
    Facem o dreapta cu aceste doua puncte => y = (x-A.x)*[(B.y - A.y)/(B.x - A.x)] + A.y (folosim punctul A si panta determinata de punctele A si B)
    y = valoarea analogica citita de senzor => trebuie sa aflam x care e temperatura in grade celsius
    x = (y-A.y) * [(B.x - A.x)/(B.y - A.y)] + A.x
    x = y * [(B.x - A.x)/(B.y - A.y)] - A.y * [(B.x - A.x)/(B.y - A.y)] + A.x
    Pentru optimizare precalculam anumite valori:
    [(B.x - A.x)/(B.y - A.y)] = -0.17
    - A.y * [(B.x - A.x)/(B.y - A.y)] + A.x = 63.6
*/