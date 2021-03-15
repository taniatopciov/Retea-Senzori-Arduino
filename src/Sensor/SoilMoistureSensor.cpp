#include "SoilMoistureSensor.h"

SoilMoistureSensor::SoilMoistureSensor(int pin) : ISensor(pin)
{
}

SoilMoistureSensor::~SoilMoistureSensor()
{
}

float SoilMoistureSensor::ReadValue()
{
    float result = 0;
    for (int i = 0; i < 10; i++)
    {
        result += ISensor::ReadValue();
    }

    return result / 10.0;
}