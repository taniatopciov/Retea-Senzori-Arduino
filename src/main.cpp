#include <Arduino.h>

#include "Bluetooth/BTCommunicationProtocol.h"

#include "Sensor/ISensor.h"
#include "Sensor/SoilMoistureSensor.h"

SoilMoistureSensor soilMoistureSensor1(A0);
SoilMoistureSensor soilMoistureSensor2(A0);

BTCommunicationProtocol BTCommunication(3, 2, &soilMoistureSensor1, &soilMoistureSensor2);

void setup()
{
	Serial.begin(9600);
	BTCommunication.Begin(9600);
	pinMode(13, OUTPUT);
}

void loop()
{
	BTCommunication.Run();
}