#include <Arduino.h>

#include "Bluetooth/BTCommunicationProtocol.h"

#include "Sensor/ISensor.h"
// #include "Sensor/SoilMoistureSensor.h"
#include "Sensor/RainSensor.h"

RainSensor rainSensor1(A0, 8);
RainSensor rainSensor2(A0, 8);

BTCommunicationProtocol BTCommunication(3, 2, &rainSensor1, &rainSensor2);

void setup()
{
	Serial.begin(9600);
	BTCommunication.Begin(9600);
}

void loop()
{
	BTCommunication.Run();
}