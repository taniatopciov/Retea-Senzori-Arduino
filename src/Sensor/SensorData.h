#ifndef SENSOR_DATA_H
#define SENSOR_DATA_H

#include "SensorTypes.h"

union SensorData
{
    struct Data
    {
        SensorTypes sensorType_en;
        float sensorValue;
        long time;
    } data;
    char rawBytes[sizeof(Data)];
};

#endif // SENSOR_DATA_H