#include "RainSensor.h"
#include <Arduino.h>

#define SENSOR_OUTPUT_MAX_VALUE 1023

RainSensor::RainSensor(int analogPin, int digitalPin) : ISensor(analogPin)
{
    // se seteaza pinul digital al senzorului ca intrare (INPUT)
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
    // se citeste daca senzorul a detectat picaturi de ploaie
    int isNotWet = digitalRead(digitalPin);

    if (isNotWet)
    {
        // daca nu au fost detectate picaturi de ploaie, atunci se va returna 1023
        return SENSOR_OUTPUT_MAX_VALUE;
    }

    // daca au fost detectate picaturi de ploaie, atunci se va returna cat de umed este senzorul
    return ISensor::ReadValue();
}
