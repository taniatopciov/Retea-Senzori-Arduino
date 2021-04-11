#include <Arduino.h>

#include "NodeManager/NodeManager.h"
#include "NodeManager/SDCardManager.h"
#include "Bluetooth/BTCommunicationProtocol.h"
#include "Sensor/ISensor.h"
#include "Sensor/RainSensor.h"
#include "Sensor/GasSensor.h"
#include "Sensor/LightSensor.h"
#include "Sensor/TempSensor.h"
#include "Sensor/SoilMoistureSensor.h"

// SoilMoistureSensor soilMoistureSensor(A0);
RainSensor rainSensor(A0, 8);
GasSensor gasSensor(A1);
// LightSensor lightSensor(A0);
// TempSensor tempSensor(2);

NodeManager g_NodeManager(&rainSensor, &gasSensor);
SDCardManager g_SDCardManager;
BTCommunicationProtocol g_BTCommunicationProtocol(3, 2);

void setup()
{
	Serial.begin(9600);
	g_BTCommunicationProtocol.Begin(9600);

	g_NodeManager.Start();
}

void loop()
{
	//delay(3000);
	g_NodeManager.Tick();
	g_BTCommunicationProtocol.Tick();
}