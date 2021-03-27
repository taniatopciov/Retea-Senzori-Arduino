#include "BTCommunicationProtocol.h"
#include <string.h>
#include "NodeManager/NodeManager.h"
#include "Sensor/SensorTypes.h"
#include "Sensor/SensorData.h"

static char ConnectString[] = "CONN";
static char DisconnectString[] = "STOP";
static char RequestDataString[] = "RQDT";
static char RequestSensorTypesString[] = "RQST";
static char SendSensorTypeString[] = "STYP";
static char DataPacketString[] = "DATA";
static char StartSendingDataString[] = "STSD";
static char StopSendingDataString[] = "SPSD";

BTCommunicationProtocol::BTCommunicationProtocol(int BTTransmitPin, int BTReceivePin) : BTSerial(BTReceivePin, BTTransmitPin, 0)
{
    Reset();
}

BTCommunicationProtocol::~BTCommunicationProtocol()
{
}

void BTCommunicationProtocol::Reset()
{
    protocolState_en = WAITING_FOR_MESSAGE;
    currentTextBufferIndex = 0;
    g_NodeManager.GoToSleep();
}

void BTCommunicationProtocol::Begin(long baudrate)
{
    BTSerial.begin(baudrate);
}

void BTCommunicationProtocol::Run()
{
    currentMillis = millis();

    StateMachineRun();

    if (currentMillis - lastMillis >= READ_TIMEOUT)
    {
        lastMillis = currentMillis;
        Reset();
    }
}

void BTCommunicationProtocol::StateMachineRun()
{
    switch (protocolState_en)
    {
    case WAITING_FOR_MESSAGE:
    {
        currentTextBufferIndex = 0;
        while (HasData())
        {
            lastMillis = currentMillis;
            textBuffer[currentTextBufferIndex++] = (char)BTSerial.read();

            if (currentTextBufferIndex == TEXT_BUFFER_SIZE)
            {
                textBuffer[currentTextBufferIndex] = '\0';
                currentTextBufferIndex = 0;

                if (strcmp(textBuffer, ConnectString) == 0)
                {
                    g_NodeManager.WakeUp();
                }
                else if (strcmp(textBuffer, DisconnectString) == 0)
                {
                    Reset();
                }
                else if (strcmp(textBuffer, RequestDataString) == 0)
                {
                    protocolState_en = SENDING_DATA;
                }
                else if (strcmp(textBuffer, RequestSensorTypesString) == 0)
                {
                    protocolState_en = SENDING_SENSOR_TYPES;
                }
                else
                {
                    protocolState_en = WAITING_FOR_MESSAGE;
                }
            }

            // Remove End Of Line
            // while (HasData())
            // {
            //     BTSerial.read();
            // }
        }
    };
    break;

    case SENDING_SENSOR_TYPES:
    {
        lastMillis = currentMillis;

        SensorTypes sensorTypes[2];
        g_NodeManager.GetSensorTypes(sensorTypes);

        char type[] = "0";
        for (int i = 0; i < 2; i++)
        {
            if (sensorTypes[i] == NO_TYPE)
            {
                continue;
            }
            SendString(SendSensorTypeString);
            type[0] += sensorTypes[i];

            SendString(type);
            type[0] = '0';
        }

        protocolState_en = WAITING_FOR_MESSAGE;
    }
    break;

    case SENDING_DATA:
    {
        lastMillis = currentMillis;

        SendString(StartSendingDataString);

        g_NodeManager.StartDataReplay();

        SensorData sensorData;

        while (g_NodeManager.HasNextLog())
        {
            g_NodeManager.ReadSensorDataFromLog(&sensorData);
            SendString(DataPacketString);
            SendBytes(&sensorData, sizeof(SensorData));
        }

        SendString(StopSendingDataString);
        protocolState_en = WAITING_FOR_MESSAGE;
    }
    break;
    }
}

bool BTCommunicationProtocol::HasData()
{
    return BTSerial.available();
}

void BTCommunicationProtocol::SendString(char *str)
{
    int size = strlen(str);
    for (int i = 0; i < size; i++)
    {
        BTSerial.write(str[i]);
    }
}

void BTCommunicationProtocol::SendBytes(void *memoryLocation_ptr, size_t memoryLocationSize)
{
    void *memoryLocationCurrent_ptr = memoryLocation_ptr;
    char *currentByte = NULL;

    for (size_t i = 0; i < memoryLocationSize; i++)
    {
        currentByte = (char *)memoryLocationCurrent_ptr + i;
        BTSerial.write(*currentByte);
    }
}