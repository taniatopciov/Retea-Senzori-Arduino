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

// se fac mai multe citiri si se va returna media acestora
float SoilMoistureSensor::ReadValue()
{
    float result = 0;
    for (int i = 0; i < 10; i++)
    {
        result += ISensor::ReadValue();
    }

    return result / 10.0;
}
