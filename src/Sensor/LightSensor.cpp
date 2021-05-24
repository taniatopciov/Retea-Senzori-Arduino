#include "LightSensor.h"
#include <Arduino.h>

LightSensor::LightSensor(int pin) : ISensor(pin)
{
}

LightSensor::~LightSensor()
{
}

SensorTypes LightSensor::GetType()
{
    return LIGHT_SENSOR;
}