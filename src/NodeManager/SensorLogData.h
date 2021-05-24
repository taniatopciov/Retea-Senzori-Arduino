#ifndef SENSOR_DATA_H
#define SENSOR_DATA_H

#include "Sensor/SensorTypes.h"
#include <Arduino.h>

enum LogType : char
{
    LOG_STARTED = 0,
    LOG_DATA,
    LOG_TIME_SET,
    LOG_ENDED,
    LOG_LIVE_DATA,
};

// o inregistrare ce se va scrie in log
union SensorLogData
{
    struct Data
    {
        LogType logType;           // tipul inregistrarii
        SensorTypes sensorType_en; // tipul senzorului
        uint16_t batchIndex_u16;   // index-u log-ului
        float sensorValue;         // valoare citita de senzor
        uint64_t time_u64;         // timpul la care a fost facuta citirea
    } data;
    char rawBytes[sizeof(Data)];
};

#endif // SENSOR_DATA_H