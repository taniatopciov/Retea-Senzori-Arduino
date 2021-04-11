#ifndef CONFIG_DATA_H
#define CONFIG_DATA_H

#include <Arduino.h>

union ConfigData
{
    struct Data
    {
        uint32_t saveToLogIntervalInSeconds_u32;
        uint16_t lastBatchIndex_u16;
    } data;
    char rawBytes[sizeof(Data)];
};

#endif // CONFIG_DATA_H