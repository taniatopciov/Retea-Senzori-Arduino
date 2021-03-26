#ifndef SENSOR_DATA_H
#define SENSOR_DATA_H

#include "SensorTypes.h"

struct SensorData
{
    SensorTypes sensorType_en;
    float sensorValue;
    long time;
};

#endif // SENSOR_DATA_H