#include "TempSensor.h"
#include "Arduino.h"
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define DHTPIN 4
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
    dht.temperature().getEvent(&event);
    if (!isnan(event.temperature))
    {
        backupTemp = event.temperature;
        return event.temperature;
    }
    else
    {
        return backupTemp;
    }
    //Serial.println(event.temperature);
}
