#include "SimpleTempSensor.h"

#define INVERSUL_PANTEI -0.17
#define VALOARE_PRECALCULATA 63.6

SimpleTempSensor::SimpleTempSensor(int pin) : ISensor(pin)
{
}

SimpleTempSensor::~SimpleTempSensor()
{
}

SensorTypes SimpleTempSensor::GetType()
{
    return SIMPLE_TEMP_SENSOR;
}

float SimpleTempSensor::ReadValue()
{
    float y = ISensor::ReadValue();
    return y * INVERSUL_PANTEI + VALOARE_PRECALCULATA;
}

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