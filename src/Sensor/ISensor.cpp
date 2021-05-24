#include "ISensor.h"
#include <Arduino.h>

ISensor::ISensor(int pin)
{
    // se salveaza pinul la care este conectat senzorul si se marcheaza ca fiind de intrare (INPUT)
    this->pin = pin;
    pinMode(pin, INPUT);
}

ISensor::~ISensor()
{
}

float ISensor::ReadValue()
{
    // implementarea implicita a unui senzor este sa citeasca analogic valoarea de pe pinul la care a fost asociat
    return analogRead(pin);
}