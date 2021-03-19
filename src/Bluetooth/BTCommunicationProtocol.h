#ifndef BLUETOOTH_MODULE_H
#define BLUETOOTH_MODULE_H

#include "Arduino.h"
#include "SoftwareSerial.h"
#include "Sensor/ISensor.h"

#define TEXT_BUFFER_SIZE 4
#define READ_TIMEOUT 8000

enum BTProtocolState
{
    WAITING_FOR_MESSAGE = 0,
    SEND_SENSOR_TYPES,
    WAITING_FOR_SAMPLE_COUNT,
    SEND_SAMPLES
};

class BTCommunicationProtocol
{
public:
    BTCommunicationProtocol(int BTTransmitPin, int BTReceivePin, ISensor *sensor1, ISensor *sensor2);
    ~BTCommunicationProtocol();

    void Reset();
    void Begin(long baudrate);
    void Run();

private:
    void StateMachineRun();
    void SendSensorTypes();
    bool HasData();

    int sampleCountPerSensor;

    long currentMillis;
    long lastMillis;

    SoftwareSerial BTSerial;
    BTProtocolState protocolState_en;
    ISensor *sensor1;
    ISensor *sensor2;
    char textBuffer[TEXT_BUFFER_SIZE + 1];
    int currentTextBufferIndex;
};

#endif // BLUETOOTH_MODULE_H