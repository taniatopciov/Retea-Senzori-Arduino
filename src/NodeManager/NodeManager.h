#ifndef NODE_MANAGER_H
#define NODE_MANAGER_H

#include <Arduino.h>

#include "Task/Task.h"
#include "ConfigData.h"
#include "SensorLogData.h"
#include "Sensor/ISensor.h"
#include "Sensor/SensorTypes.h"

/**
 * Aceasta clasa este folosita pentru a controla functionalitatea nodului 
 * 
 * Exista functii pentru a economisi bateria WakeUp si GoToSleep
 * 
 * Exista functii
 * - pentru a seta timpul in log-ul curent
 * - pentru a seta cat de des se salveaza informatiile pe SD Card
 * - pentru a determina numarul de senzori si tipul lor
 * 
 * Salvarea datelor
 * Senzorii citesc valori care vor fi salvate intr-un fisier, denumit "log"
 * Dupa ce se porneste alimentarea, se va scrie in log-ul trecut un mesaj ca "log-ul a fost incheiat"
 * si se va crea un alt log, care va deveni log-ul curent in care vor fi salvate informatiile de la senzori
 * 
 * Citirea datelor
 * Clasa ofera atat posibilitatea de citire a log-ului curent, cat si a tuturor log-urilor salvate pe SD Card
 * Clasa ofera si citirea valorile citite de la senzori
*/

class NodeManager : public Task
{
public:
    NodeManager(ISensor *sensor1, ISensor *sensor2);
    ~NodeManager();

    void Execute();
    void Tick();

    void Start();
    void WakeUp();
    void GoToSleep();

    void SetTime(uint64_t time_u64);
    void SetSaveToLogInterval(uint32_t interval_u32);
    void GetSensorTypes(SensorTypes sensorTypes[2]);
    uint8_t GetSensorCount();
    void StopReplay();
    void StartAllLogsDataReplay();
    void StartCurrentLogDataReplay();

    bool ReadSensorDataFromLog(SensorLogData *data_ptr);
    void ReadLiveSensorData(SensorLogData *data_ptr, bool firstSensor_b);
    void ReplayLogFile(char *logFileName);

private:
    inline uint64_t CalculateCurrentTime();
    void WriteConfigFile();
    void CreateLogFileName(char *str, uint16_t batchIndex_u16);
    void AppendToLogFile(char *logFileName, SensorLogData *logData_ptr);
    void OpenNextLogFileForRead();

    ConfigData configData_st;

    ISensor *sensor1;
    ISensor *sensor2;

    uint16_t currentBatchLogIndex_u16;

    uint64_t currentTimeElapsed_u64;
    unsigned long lastMilliseconds;
};

extern NodeManager g_NodeManager;

#endif // NODE_MANAGER_H