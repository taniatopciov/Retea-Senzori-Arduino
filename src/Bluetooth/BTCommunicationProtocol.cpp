#include "BTCommunicationProtocol.h"
#include <string.h>
#include "NodeManager/NodeManager.h"
#include "NodeManager/SensorLogData.h"
#include "Sensor/SensorTypes.h"

// dupa 10000 milisecunde de inactivitate, state machine-ul este resetat
#define READ_TIMEOUT 10000

// comenzile
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
    // se initializeaza obiectul BTSerial cu pinii corespunzatori pentru comunicatia seriala
    // se reseteaza state machine-ul
    ResetStateMachine();

    // se seteaza intervalul pentru apelul ciclic a functiei Execute
    SetIntervalTime(READ_TIMEOUT);
}

BTCommunicationProtocol::~BTCommunicationProtocol()
{
}

// functia care se va executa ciclic
// daca aceasta functie se apeleaza, atunci au trecut READ_TIMEOUT milisecunde de inactivitate
void BTCommunicationProtocol::Execute()
{
    SendString(TimeoutOccuredString);
    g_NodeManager.GoToSleep();
}

// se reseteaza starea si seteaza indexul cuvantului citit pe 0
void BTCommunicationProtocol::ResetStateMachine()
{
    protocolState_en = WAITING_FOR_MESSAGE;
    currentTextBufferIndex = 0;
}

// se initializeaza obiectul care face comunicatia pe seriala
void BTCommunicationProtocol::Begin(long baudrate)
{
    BTSerial.begin(baudrate);
}

void BTCommunicationProtocol::Tick()
{
    Task::Tick();

    StateMachineRun();
}

// se trimite pe seriala mesajul de eroare al SD Card-ului
void BTCommunicationProtocol::SendSDCardError(SDCardErrors sdCardErrors_en)
{
    SendString(SDCardErrorString);
    SendByte(sdCardErrors_en);
}

// se trimite mesajul ca a fost deschis un fisier pentru citire
void BTCommunicationProtocol::SendOpenFileMsg()
{
    SendString(OpenLogFileString);
}

// se trimite mesajul ca a fost inchis un fisier
void BTCommunicationProtocol::SendCloseFileMsg()
{
    SendString(CloseLogFileString);
}

// se trimite mesajul ca s-a terminat transmiterea informatiilor din log-uri
void BTCommunicationProtocol::SendLogReplayDoneMsg()
{
    SendString(LogReplayDoneString);
}

void BTCommunicationProtocol::StateMachineRun()
{
    currentTextBufferIndex = 0;

    // aceasta bucla se executa cat timp exista date necitite
    while (BTSerial.available())
    {
        // prin aceasta functie se marcheaza faptul ca a aparut o modificare, deci nodul nu este inactiv
        ResetTaskCounter();

        // se creeaza antetul mesajul, citind cate un octet o data
        textBuffer[currentTextBufferIndex++] = (char)BTSerial.read();

        if (currentTextBufferIndex == TEXT_BUFFER_SIZE)
        {
            // se va verifica daca antetul primit este o comanda valida

            textBuffer[currentTextBufferIndex] = '\0';
            currentTextBufferIndex = 0;
            // Serial.println(textBuffer);
            if (strcmp(textBuffer, ConnectString) == 0)
            {
                g_NodeManager.WakeUp();
                ResetStateMachine();
            }
            else if (strcmp(textBuffer, KillString) == 0)
            {
                // se reseteaza state machine-ul, se opreste trimiterea de mesaje daca este cazul
                // si se intra in modul "sleep"
                ResetStateMachine();
                g_NodeManager.StopReplay();
                g_NodeManager.GoToSleep();
            }
            else if (strcmp(textBuffer, StopString) == 0)
            {
                // se opreste trimiterea de mesaje
                g_NodeManager.StopReplay();
            }
            else if (strcmp(textBuffer, LiveString) == 0)
            {
                // se citesc valorile de la senzori si se trimit pe seriala
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
                // se intra in starea de SENDING_SENSOR_COUNT
                protocolState_en = SENDING_SENSOR_COUNT;
            }
            else if (strcmp(textBuffer, RequestSensorTypesString) == 0)
            {
                // se intra in starea de SENDING_SENSOR_TYPES
                protocolState_en = SENDING_SENSOR_TYPES;
            }
            else if (strcmp(textBuffer, PlayAllDataFromAllLogsString) == 0)
            {
                // se intra in starea de SENDING_DATA si se vor trimite toate log-urile
                protocolState_en = SENDING_DATA;
                g_NodeManager.StartAllLogsDataReplay();
            }
            else if (strcmp(textBuffer, ReplayDataFromCurrentLogString) == 0)
            {
                // se intra in starea de SENDING_DATA si se va trimite log-ul curent
                protocolState_en = SENDING_DATA;
                g_NodeManager.StartCurrentLogDataReplay();
            }
            else if (strcmp(textBuffer, SetUnixTimeString) == 0)
            {
                uint64_t time_u64 = 0;
                uint8_t i = 0;

                // se va citi timpul primit
                while (BTSerial.available())
                {
                    uint32_t value = (uint32_t)BTSerial.read();
                    time_u64 |= value << i;
                    i += 8;
                }
                // se va salva timpul primit in log-ul curent
                g_NodeManager.SetTime(time_u64);
            }
            else if (strcmp(textBuffer, SetSaveToLogTimeIntervalString) == 0)
            {
                uint32_t interval_u32 = 0;
                uint8_t i = 0;

                // se va citi intervalul primit
                while (BTSerial.available())
                {
                    uint32_t value = (uint32_t)BTSerial.read();
                    interval_u32 |= value << i;
                    i += 8;
                }
                // se va salva intervalul de salvare a datelor pe SD Card in log-ul curent
                g_NodeManager.SetSaveToLogInterval(interval_u32);
            }
            else
            {
                // daca antetul primit nu este valid, se va trece in starea de asteptarea a unui alt mesaj
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
        // prin aceasta functie se marcheaza faptul ca a aparut o modificare, deci nodul nu este inactiv

        ResetTaskCounter();

        // se preia numarul de senzori, se trimite pe seriala si se reseteaza state-machine-ul
        uint8_t sensorCount = g_NodeManager.GetSensorCount();
        SendString(SendSensorCountString);
        SendByte(sensorCount);

        protocolState_en = WAITING_FOR_MESSAGE;
    }
    break;

    case SENDING_SENSOR_TYPES:
    {
        // prin aceasta functie se marcheaza faptul ca a aparut o modificare, deci nodul nu este inactiv

        ResetTaskCounter();

        // se preia tipul senzorilor, se trimite pe seriala si se reseteaza state-machine-ul
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
        // prin aceasta functie se marcheaza faptul ca a aparut o modificare, deci nodul nu este inactiv

        ResetTaskCounter();

        SensorLogData sensorData;

        // se incearca citirea unei inregistrari din log
        // daca citirea se face cu succes, atunci se va trimite pe seriala
        // daca citirea nu reuseste, inseamna ca s-a ajuns la finalul log-ului si trebuie resetata starea state machine-ului
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

// se trimite un sir de caractere pe seriala
void BTCommunicationProtocol::SendString(char *str)
{
    int size = strlen(str);
    Serial.println(str);
    SendBytes(str, size);
}

// se trimit un sir de octeti pe seriala
void BTCommunicationProtocol::SendBytes(char bytes[], size_t size)
{
    BTSerial.write(bytes, size);
}

// se trimite un octet pe seriala
void BTCommunicationProtocol::SendByte(char byte)
{
    BTSerial.write(byte);
}