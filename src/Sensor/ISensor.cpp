#include "ISensor.h"
#include <Arduino.h>

ISensor::ISensor(int pin)
{
    this->pin = pin;
    pinMode(pin, INPUT);
}

ISensor::~ISensor()
{
}

float ISensor::ReadValue()
{
    return analogRead(pin);
}