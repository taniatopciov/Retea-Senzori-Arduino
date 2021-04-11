#include "NodeManager.h"
#include "Arduino.h"
#include "SDCardManager.h"
#include "SDCardErrors.h"
#include "Bluetooth/BTCommunicationProtocol.h"
#include "SensorLogData.h"

static char CONFIG_FILE[] = "NODESI/CONFIG.BIN";
static char LOG_FOLDER[] = "NODESI/LOGS";

NodeManager::NodeManager(ISensor *sensor1, ISensor *sensor2)
{
    this->sensor1 = sensor1;
    this->sensor2 = sensor2;

    configData_st.data.saveToLogIntervalInSeconds_u32 = 60;
    configData_st.data.lastBatchIndex_u16 = 0;
}

NodeManager::~NodeManager()
{
}

void NodeManager::Execute()
{
    char logFileName[25] = "";
    CreateLogFileName(logFileName, configData_st.data.lastBatchIndex_u16);
    SensorLogData sensorData;

    g_NodeManager.ReadLiveSensorData(&sensorData, true);
    AppendToLogFile(logFileName, &sensorData);

    g_NodeManager.ReadLiveSensorData(&sensorData, false);
    AppendToLogFile(logFileName, &sensorData);
}

void NodeManager::Start()
{
    Serial.print("Initializing SD Card...");
    if (g_SDCardManager.InitSDCard())
    {
        Serial.println("success");
        if (!g_SDCardManager.CreateFolder(LOG_FOLDER))
        {
            Serial.println("Could not create LOG Folder");
            g_BTCommunicationProtocol.SendSDCardError(SD_CREATE_FOLDER_ERROR);
        }

        if (!g_SDCardManager.ExistsFile(CONFIG_FILE))
        {
            WriteConfigFile();
        }
        else
        {
            if (!g_SDCardManager.OpenFile(CONFIG_FILE))
            {
                g_BTCommunicationProtocol.SendSDCardError(SD_OPEN_FILE_ERROR);
            }
            else
            {
                if (!g_SDCardManager.ReadRawBytes(configData_st.rawBytes, sizeof(configData_st.rawBytes)))
                {
                    g_BTCommunicationProtocol.SendSDCardError(SD_READ_ERROR);
                }
                g_SDCardManager.CloseFile();
            }
        }

        SetIntervalTime(configData_st.data.saveToLogIntervalInSeconds_u32);

        char logFileName[25] = "";
        CreateLogFileName(logFileName, configData_st.data.lastBatchIndex_u16);

        SensorLogData logData;
        memset(&logData, 0, sizeof(SensorLogData));

        if (g_SDCardManager.ExistsFile(logFileName))
        {
            logData.data.batchIndex_u16 = configData_st.data.lastBatchIndex_u16;
            logData.data.logType = LOG_ENDED;

            AppendToLogFile(logFileName, &logData);

            configData_st.data.lastBatchIndex_u16++;
            WriteConfigFile();

            CreateLogFileName(logFileName, configData_st.data.lastBatchIndex_u16);
        }

        logData.data.batchIndex_u16 = configData_st.data.lastBatchIndex_u16;
        logData.data.logType = LOG_STARTED;

        AppendToLogFile(logFileName, &logData);
    }
    else
    {
        Serial.println("Initialization failed!");
        g_BTCommunicationProtocol.SendSDCardError(SD_INIT_ERROR);
    }
}

void NodeManager::WakeUp()
{
}

void NodeManager::GoToSleep()
{
}

void NodeManager::SetTime(uint64_t time_u64)
{

    char logFileName[25] = "";
    CreateLogFileName(logFileName, configData_st.data.lastBatchIndex_u16);

    SensorLogData logData;
    memset(&logData, 0, sizeof(SensorLogData));

    logData.data.batchIndex_u16 = configData_st.data.lastBatchIndex_u16;
    logData.data.logType = LOG_TIME_SET;
    logData.data.time_u64 = time_u64;

    AppendToLogFile(logFileName, &logData);
}

void NodeManager::SetSaveToLogInterval(uint32_t interval_u32)
{
    configData_st.data.saveToLogIntervalInSeconds_u32 = interval_u32;
    SetIntervalTime(configData_st.data.saveToLogIntervalInSeconds_u32);
    WriteConfigFile();
}

void NodeManager::GetSensorTypes(SensorTypes sensorTypes[2])
{
    sensorTypes[0] = NO_TYPE;
    sensorTypes[1] = NO_TYPE;

    if (sensor1 != NULL)
    {
        sensorTypes[0] = sensor1->GetType();
    }

    if (sensor2 != NULL)
    {
        sensorTypes[1] = sensor2->GetType();
    }
}

uint8_t NodeManager::GetSensorCount()
{
    uint8_t count_u8 = 0;

    if (sensor1 != NULL)
    {
        count_u8++;
    }

    if (sensor2 != NULL)
    {
        count_u8++;
    }

    return count_u8;
}

void NodeManager::StopReplay()
{
    if (g_SDCardManager.HasData())
    {
        g_SDCardManager.CloseFile();
        g_BTCommunicationProtocol.SendCloseFileMsg();
    }
    currentBatchLogIndex_u16 = configData_st.data.lastBatchIndex_u16 + 1;
}

void NodeManager::StartAllLogsDataReplay()
{
    currentBatchLogIndex_u16 = 0;

    OpenNextLogFileForRead();
}

void NodeManager::StartCurrentLogDataReplay()
{
    currentBatchLogIndex_u16 = configData_st.data.lastBatchIndex_u16;

    OpenNextLogFileForRead();
}

void NodeManager::ReadLiveSensorData(SensorLogData *data_ptr, bool firstSensor_b)
{
    data_ptr->data.time_u64 = CalculateCurrentTime();
    data_ptr->data.batchIndex_u16 = configData_st.data.lastBatchIndex_u16;
    data_ptr->data.logType = LOG_DATA;

    if (firstSensor_b)
    {
        if (sensor1 != NULL)
        {
            data_ptr->data.sensorType_en = sensor1->GetType();
            data_ptr->data.sensorValue = sensor1->ReadValue();
        }
    }
    else
    {
        if (sensor2 != NULL)
        {
            data_ptr->data.sensorType_en = sensor2->GetType();
            data_ptr->data.sensorValue = sensor2->ReadValue();
        }
    }
}

bool NodeManager::ReadSensorDataFromLog(SensorLogData *data_ptr)
{
    if (currentBatchLogIndex_u16 <= configData_st.data.lastBatchIndex_u16)
    {
        if (g_SDCardManager.HasData())
        {
            g_SDCardManager.ReadRawBytes(data_ptr->rawBytes, sizeof(data_ptr->rawBytes));
            return true;
        }

        g_SDCardManager.CloseFile();

        g_BTCommunicationProtocol.SendCloseFileMsg();

        currentBatchLogIndex_u16++;
        if (currentBatchLogIndex_u16 > configData_st.data.lastBatchIndex_u16)
        {
            return false;
        }

        OpenNextLogFileForRead();

        if (!g_SDCardManager.HasData())
        {
            return false;
        }
        g_SDCardManager.ReadRawBytes(data_ptr->rawBytes, sizeof(data_ptr->rawBytes));

        return true;
    }

    return false;
}

void NodeManager::ReplayLogFile(char *logFileName)
{
    SensorLogData data;

    g_SDCardManager.OpenFile(logFileName);
    while (g_SDCardManager.HasData())
    {
        g_SDCardManager.ReadRawBytes(data.rawBytes, sizeof(data.rawBytes));
    }
    g_SDCardManager.CloseFile();
}

inline uint64_t NodeManager::CalculateCurrentTime()
{
    unsigned long milliseconds = millis();
    return milliseconds / 1000;
}

void NodeManager::WriteConfigFile()
{
    if (g_SDCardManager.OpenFileForWrite(CONFIG_FILE, true))
    {
        if (!g_SDCardManager.WriteRawBytes(configData_st.rawBytes, sizeof(configData_st.rawBytes)))
        {
            g_BTCommunicationProtocol.SendSDCardError(SD_WRITE_ERROR);
        }
        g_SDCardManager.CloseFile();
    }
    else
    {
        g_BTCommunicationProtocol.SendSDCardError(SD_CREATE_FILE_ERROR);
    }
}

void NodeManager::CreateLogFileName(char *str, uint16_t batchIndex_u16)
{
    strcpy(str, LOG_FOLDER);
    strcat(str, "/L");
    char lognumber[7];
    itoa(batchIndex_u16, lognumber, 10);
    strcat(str, lognumber);
    strcat(str, ".BIN");
}

void NodeManager::AppendToLogFile(char *logFileName, SensorLogData *logData_ptr)
{
    if (g_SDCardManager.OpenFileForWrite(logFileName))
    {
        if (!g_SDCardManager.WriteRawBytes(logData_ptr->rawBytes, sizeof(logData_ptr->rawBytes)))
        {
            g_BTCommunicationProtocol.SendSDCardError(SD_WRITE_ERROR);
        }
        g_SDCardManager.CloseFile();
    }
    else
    {
        g_BTCommunicationProtocol.SendSDCardError(SD_CREATE_FILE_ERROR);
    }
}

void NodeManager::OpenNextLogFileForRead()
{
    g_BTCommunicationProtocol.SendOpenFileMsg();

    char logFileName[25] = "";
    CreateLogFileName(logFileName, configData_st.data.lastBatchIndex_u16);
    g_SDCardManager.OpenFile(logFileName);
}