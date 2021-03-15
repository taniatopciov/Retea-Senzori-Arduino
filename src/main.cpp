#include <Arduino.h>

#include "Sensor/SoilMoistureSensor.h"
#include "Sensor/ISensor.h"

#define HUMIDITY_A0_PIN A0

// SoilMoistureSensor soilMoistureSensor(HUMIDITY_A0_PIN);
// SoilMoistureSensor soilMoistureSensor(HUMIDITY_A0_PIN);
SoilMoistureSensor soilMoistureSensor1(HUMIDITY_A0_PIN);
SoilMoistureSensor soilMoistureSensor2(HUMIDITY_A0_PIN);

ISensor *sensor1 = &soilMoistureSensor1;
ISensor *sensor2 = &soilMoistureSensor2;

void setup()
{
	Serial.begin(9600);
}

void loop()
{
	Serial.print(sensor1->ReadValue());
	Serial.print(' ');
	Serial.print(sensor2->ReadValue());
	Serial.println();
}