#include "GasSensor.h"

GasSensor::GasSensor(int pin) : ISensor(pin)
{
}

GasSensor::~GasSensor()
{
}

SensorTypes GasSensor::GetType()
{
    return GAS_SENSOR;
}