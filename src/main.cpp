#include <Arduino.h>

#include "NodeManager/NodeManager.h"
#include "Bluetooth/BTCommunicationProtocol.h"
#include "Sensor/ISensor.h"
#include "Sensor/RainSensor.h"
#include "Sensor/GasSensor.h"
#include "Sensor/LightSensor.h"
#include "Sensor/TempSensor.h"
#include "Sensor/SimpleTempSensor.h"
#include "Sensor/SoilMoistureSensor.h"

SoilMoistureSensor soilMoistureSensor(A0);
SimpleTempSensor simpleTempSensor(A1);

//RainSensor rainSensor(A0, 8);
//GasSensor gasSensor(A1);

// LightSensor lightSensor(A0);
// TempSensor tempSensor(2);

NodeManager g_NodeManager(&soilMoistureSensor, &simpleTempSensor);
BTCommunicationProtocol BTCommunication(3, 2);

void setup()
{
	Serial.begin(9600);
	BTCommunication.Begin(9600);
}

void loop()
{
	//Serial.println(soilMoistureSensor.ReadValue());
	//Serial.println(tempSensor.ReadValue());
	//delay(3000);
	BTCommunication.Run();
}