#ifndef NODE_MANAGER_H
#define NODE_MANAGER_H

#include <Arduino.h>

#include "Task/Task.h"
#include "ConfigData.h"
#include "SensorLogData.h"
#include "Sensor/ISensor.h"
#include "Sensor/SensorTypes.h"

class NodeManager : public Task
{
public:
    NodeManager(ISensor *sensor1, ISensor *sensor2);
    ~NodeManager();

    void Execute();

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
};

extern NodeManager g_NodeManager;

#endif // NODE_MANAGER_H