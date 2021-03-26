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
    SENDING_SENSOR_TYPES,
    SENDING_DATA
};

class BTCommunicationProtocol
{
public:
    BTCommunicationProtocol(int BTTransmitPin, int BTReceivePin);
    ~BTCommunicationProtocol();

    void Reset();
    void Begin(long baudrate);
    void Run();

private:
    void StateMachineRun();
    bool HasData();
    void SendString(char *str);
    void SendBytes(void *memoryLocation_ptr, size_t memoryLocationSize);

    long currentMillis;
    long lastMillis;

    SoftwareSerial BTSerial;
    BTProtocolState protocolState_en;
    char textBuffer[TEXT_BUFFER_SIZE + 1];
    int currentTextBufferIndex;
};

#endif // BLUETOOTH_MODULE_H