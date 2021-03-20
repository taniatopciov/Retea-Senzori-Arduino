#include "RainSensor.h"
#include <Arduino.h>

#define SENSOR_OUTPUT_MAX_VALUE 1023

RainSensor::RainSensor(int analogPin, int digitalPin) : ISensor(analogPin)
{
    pinMode(digitalPin, INPUT);
    this->digitalPin = digitalPin;
}

RainSensor::~RainSensor()
{
}

SensorTypes RainSensor::GetType()
{
    return RAIN_SENSOR;
}

float RainSensor::ReadValue()
{
    int isNotWet = digitalRead(digitalPin);

    if (isNotWet)
    {
        return SENSOR_OUTPUT_MAX_VALUE;
    }

    return ISensor::ReadValue();
}
