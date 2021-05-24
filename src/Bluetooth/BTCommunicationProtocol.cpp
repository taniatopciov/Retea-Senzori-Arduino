#include "BTCommunicationProtocol.h"
#include <string.h>
#include "NodeManager/NodeManager.h"
#include "NodeManager/SensorLogData.h"
#include "Sensor/SensorTypes.h"

#define READ_TIMEOUT 10000

static char ConnectString[] = "CONN";
static char KillString[] = "KILL";
static char StopString[] = "STOP";
static char LiveString[] = "LIVE";
static char RequestSensorCountString[] = "RQSC";
static char SendSensorCountString[] = "SCNT";
static char RequestSensorTypesString[] = "RQST";
static char SendSensorTypeString[] = "STYP";
static char LogDataPacketString[] = "DATA";
static char PlayAllDataFromAllLogsString[] = "PLAY";
static char ReplayDataFromCurrentLogString[] = "RQDT";
static char OpenLogFileString[] = "OPEN";
static char CloseLogFileString[] = "CLOS";
static char SetUnixTimeString[] = "TIME";
static char SetSaveToLogTimeIntervalString[] = "SETI";
static char SDCardErrorString[] = "SDER";
static char TimeoutOccuredString[] = "TOUT";
static char LogReplayDoneString[] = "DONE";

BTCommunicationProtocol::BTCommunicationProtocol(int BTTransmitPin, int BTReceivePin) : BTSerial(BTReceivePin, BTTransmitPin, 0)
{
    ResetStateMachine();

    SetIntervalTime(READ_TIMEOUT);
}

BTCommunicationProtocol::~BTCommunicationProtocol()
{
}

void BTCommunicationProtocol::Execute()
{
    SendString(TimeoutOccuredString);
    g_NodeManager.GoToSleep();
}

void BTCommunicationProtocol::ResetStateMachine()
{
    protocolState_en = WAITING_FOR_MESSAGE;
    currentTextBufferIndex = 0;
}

void BTCommunicationProtocol::Begin(long baudrate)
{
    BTSerial.begin(baudrate);
}

void BTCommunicationProtocol::Tick()
{
    Task::Tick();

    StateMachineRun();
}

void BTCommunicationProtocol::SendSDCardError(SDCardErrors sdCardErrors_en)
{
    SendString(SDCardErrorString);
    SendByte(sdCardErrors_en);
}

void BTCommunicationProtocol::SendOpenFileMsg()
{
    SendString(OpenLogFileString);
}

void BTCommunicationProtocol::SendCloseFileMsg()
{
    SendString(CloseLogFileString);
}

void BTCommunicationProtocol::SendLogReplayDoneMsg()
{
    SendString(LogReplayDoneString);
}

void BTCommunicationProtocol::StateMachineRun()
{
    currentTextBufferIndex = 0;

    while (BTSerial.available())
    {
        ResetTaskCounter();

        textBuffer[currentTextBufferIndex++] = (char)BTSerial.read();

        if (currentTextBufferIndex == TEXT_BUFFER_SIZE)
        {
            textBuffer[currentTextBufferIndex] = '\0';
            currentTextBufferIndex = 0;
            Serial.println(textBuffer);
            if (strcmp(textBuffer, ConnectString) == 0)
            {
                g_NodeManager.WakeUp();
                ResetStateMachine();
            }
            else if (strcmp(textBuffer, KillString) == 0)
            {
                ResetStateMachine();
                g_NodeManager.StopReplay();
                g_NodeManager.GoToSleep();
            }
            else if (strcmp(textBuffer, StopString) == 0)
            {
                g_NodeManager.StopReplay();
            }
            else if (strcmp(textBuffer, LiveString) == 0)
            {
                SensorLogData sensorData;

                g_NodeManager.ReadLiveSensorData(&sensorData, true);
                SendString(LogDataPacketString);
                SendBytes(sensorData.rawBytes, sizeof(sensorData.rawBytes));

                g_NodeManager.ReadLiveSensorData(&sensorData, false);
                SendString(LogDataPacketString);
                SendBytes(sensorData.rawBytes, sizeof(sensorData.rawBytes));
            }
            else if (strcmp(textBuffer, RequestSensorCountString) == 0)
            {
                protocolState_en = SENDING_SENSOR_COUNT;
            }
            else if (strcmp(textBuffer, RequestSensorTypesString) == 0)
            {
                protocolState_en = SENDING_SENSOR_TYPES;
            }
            else if (strcmp(textBuffer, PlayAllDataFromAllLogsString) == 0)
            {
                protocolState_en = SENDING_DATA;
                g_NodeManager.StartAllLogsDataReplay();
            }
            else if (strcmp(textBuffer, ReplayDataFromCurrentLogString) == 0)
            {
                protocolState_en = SENDING_DATA;
                g_NodeManager.StartCurrentLogDataReplay();
            }
            else if (strcmp(textBuffer, SetUnixTimeString) == 0)
            {
                uint64_t time_u64 = 0;
                uint8_t i = 0;

                while (BTSerial.available())
                {
                    uint32_t value = (uint32_t)BTSerial.read();
                    time_u64 |= value << i;
                    i += 8;
                }
                g_NodeManager.SetTime(time_u64);
            }
            else if (strcmp(textBuffer, SetSaveToLogTimeIntervalString) == 0)
            {
                uint32_t interval_u32 = 0;
                uint8_t i = 0;

                while (BTSerial.available())
                {
                    uint32_t value = (uint32_t)BTSerial.read();
                    interval_u32 |= value << i;
                    i += 8;
                }
                g_NodeManager.SetSaveToLogInterval(interval_u32);
            }
            else
            {
                protocolState_en = WAITING_FOR_MESSAGE;
            }
        }
    }

    switch (protocolState_en)
    {
    case WAITING_FOR_MESSAGE:
    {
    }
    break;

    case SENDING_SENSOR_COUNT:
    {
        ResetTaskCounter();

        uint8_t sensorCount = g_NodeManager.GetSensorCount();
        SendString(SendSensorCountString);
        SendByte(sensorCount);

        protocolState_en = WAITING_FOR_MESSAGE;
    }
    break;

    case SENDING_SENSOR_TYPES:
    {
        ResetTaskCounter();

        SensorTypes sensorTypes[2];
        g_NodeManager.GetSensorTypes(sensorTypes);

        for (int i = 0; i < 2; i++)
        {
            if (sensorTypes[i] == NO_TYPE)
            {
                continue;
            }
            SendString(SendSensorTypeString);
            SendByte(sensorTypes[i]);
        }

        protocolState_en = WAITING_FOR_MESSAGE;
    }
    break;

    case SENDING_DATA:
    {
        ResetTaskCounter();

        SensorLogData sensorData;

        if (g_NodeManager.ReadSensorDataFromLog(&sensorData))
        {
            SendString(LogDataPacketString);
            SendBytes(sensorData.rawBytes, sizeof(sensorData.rawBytes));
        }
        else
        {
            protocolState_en = WAITING_FOR_MESSAGE;
        }
    }
    break;
    }
}

void BTCommunicationProtocol::SendString(char *str)
{
    int size = strlen(str);
    Serial.println(str);
    SendBytes(str, size);
}

void BTCommunicationProtocol::SendBytes(char bytes[], size_t size)
{
    BTSerial.write(bytes, size);
}

void BTCommunicationProtocol::SendByte(char byte)
{
    BTSerial.write(byte);
}