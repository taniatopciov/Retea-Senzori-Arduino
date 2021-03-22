#include "TempSensor.h"
#include "Arduino.h"
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define DHTPIN 2
#define DHTTYPE DHT11

DHT_Unified dht(DHTPIN, DHTTYPE);

TempSensor::TempSensor(int pin) : ISensor(pin)
{
}

TempSensor::~TempSensor()
{
}

SensorTypes TempSensor::GetType()
{
    return TEMP_SENSOR;
}

float TempSensor::ReadValue()
{
    dht.begin();
    sensors_event_t event;
    dht.humidity().getEvent(&event);
    if (event.temperature > 0 && event.temperature < 50)
        return event.temperature;
}
