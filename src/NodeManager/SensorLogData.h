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
};

union SensorLogData
{
    struct Data
    {
        LogType logType;
        SensorTypes sensorType_en;
        uint16_t batchIndex_u16;
        float sensorValue;
        uint64_t time_u64;
    } data;
    char rawBytes[sizeof(Data)];
};

#endif // SENSOR_DATA_H