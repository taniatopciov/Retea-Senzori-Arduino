#ifndef CONFIG_DATA_H
#define CONFIG_DATA_H

#include <Arduino.h>

union ConfigData
{
    struct Data
    {
        uint32_t saveToLogIntervalInMillis_u32; // cat de des sa fie salvate informatiile in SD Card
        uint16_t lastBatchIndex_u16;            // folosit pentru a indica care este log-ul curent in care se salveaza informatiile
    } data;
    char rawBytes[sizeof(Data)];
};

#endif // CONFIG_DATA_H