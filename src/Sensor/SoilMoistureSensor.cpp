#include "SoilMoistureSensor.h"

SoilMoistureSensor::SoilMoistureSensor(int pin) : ISensor(pin)
{
}

SoilMoistureSensor::~SoilMoistureSensor()
{
}

SensorTypes SoilMoistureSensor::GetType()
{
    return SOIL_MOISTURE_SENSOR;
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
