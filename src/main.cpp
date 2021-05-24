#include <Arduino.h>

#include "NodeManager/NodeManager.h"
#include "NodeManager/SDCardManager.h"
#include "Bluetooth/BTCommunicationProtocol.h"
#include "Sensor/ISensor.h"
#include "Sensor/RainSensor.h"
#include "Sensor/GasSensor.h"
#include "Sensor/LightSensor.h"
#include "Sensor/TempSensor.h"
#include "Sensor/SimpleTempSensor.h"
#include "Sensor/SoilMoistureSensor.h"

//SoilMoistureSensor soilMoistureSensor(A0);
//SimpleTempSensor simpleTempSensor(A1);

RainSensor rainSensor(A0, 8);
GasSensor gasSensor(A1);

// LightSensor lightSensor(A0);
// TempSensor tempSensor(4);

// se va crea controller-ul nodului cu senzorii corespunzatori
NodeManager g_NodeManager(&rainSensor, &gasSensor);
SDCardManager g_SDCardManager;
BTCommunicationProtocol g_BTCommunicationProtocol(3, 2);

void setup()
{
	Serial.begin(9600);

	// se va initializa protocolul de Bluetooth si clasa care controleaza nodul
	g_BTCommunicationProtocol.Begin(9600);
	g_NodeManager.Start();
}

void loop()
{
	// pentru a rula atat state machine-ul protocolului, cat si task-ul pentru scrierea periodica a informatiilor de la senzori
	g_NodeManager.Tick();
	g_BTCommunicationProtocol.Tick();
}