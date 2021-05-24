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

/**
 * Aceasta clasa se ocupa de comunicatia efectiva folosind Bluetooth
 * Comunicatia cu modulul de Bluetooth se face folosind comunicatie seriala
 * 
 * Protocolul consta in transmiterea unor mesaje de forma antet, valoare
 * O descriere mai detaliata a mesajelor se afla in fisierul protocol.txt din directorul res
 * 
 * Pentru a usura aceasta comunicatie, am folosit biblioteca SoftwareSerial
 * SoftwareSerial ofera functionalitatea de scriere si citire a unor octeti pe anumiti pini dati ca parametri
 * 
 * Protocolul de comunicatie este implementat folosind un state machine.
 * Starile: 
 * WAITING_FOR_MESSAGE -> se asteapta un mesaj pe seriala
 * SENDING_SENSOR_COUNT -> se trimit pe seriala numarul de senzori conectati
 * SENDING_SENSOR_TYPES -> se trimit pe seriala tipul de senzori conectati
 * SENDING_DATA -> in aceasta stare se trimit continutul fisierul de log curent
 * 
 * In cadrul citiri, se verifica daca sunt date pe seriala 
 * Daca sunt date, atunci se va crea un sir de caractere cu antetul mesajului trimis
*/

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
    void SendLogReplayDoneMsg();

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