#ifndef BLUETOOTH_MODULE_H
#define BLUETOOTH_MODULE_H

#include "Arduino.h"
#include "SoftwareSerial.h"
#include "Sensor/ISensor.h"
#include "NodeManager/SensorLogData.h"
#include "NodeManager/SDCardErrors.h"
#include "Task/Task.h"

#define TEXT_BUFFER_SIZE 4

enum BTProtocolState
{
    WAITING_FOR_MESSAGE = 0,
    SENDING_SENSOR_COUNT,
    SENDING_SENSOR_TYPES,
    SENDING_DATA
};

class BTCommunicationProtocol : public Task
{
public:
    BTCommunicationProtocol(int BTTransmitPin, int BTReceivePin);
    ~BTCommunicationProtocol();

    void Execute();
    void ResetStateMachine();
    void Begin(long baudrate);
    void Tick();
    void SendSDCardError(SDCardErrors sdCardErrors_en);
    void SendOpenFileMsg();
    void SendCloseFileMsg();

private:
    void StateMachineRun();
    void SendString(char *str);
    void SendBytes(char bytes[], size_t size);
    void SendByte(char byte);

    SoftwareSerial BTSerial;
    BTProtocolState protocolState_en;
    char textBuffer[TEXT_BUFFER_SIZE + 1];
    int currentTextBufferIndex;
};

extern BTCommunicationProtocol g_BTCommunicationProtocol;

#endif // BLUETOOTH_MODULE_H