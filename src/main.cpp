#include <Arduino.h>

#include "NodeManager/NodeManager.h"
#include "Bluetooth/BTCommunicationProtocol.h"

#include "Sensor/ISensor.h"
#include "Sensor/SoilMoistureSensor.h"

SoilMoistureSensor soilMoistureSensor1(A0);
SoilMoistureSensor soilMoistureSensor2(A0);

NodeManager g_NodeManager(&soilMoistureSensor1, &soilMoistureSensor2);
BTCommunicationProtocol BTCommunication(3, 2);

void setup()
{
	Serial.begin(9600);
	BTCommunication.Begin(9600);
}

void loop()
{
	BTCommunication.Run();
}