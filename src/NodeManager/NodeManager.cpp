#include "NodeManager.h"
#include "Arduino.h"
#include "SDCardManager.h"
#include "SDCardErrors.h"
#include "Bluetooth/BTCommunicationProtocol.h"
#include "SensorLogData.h"

// calea catre folder-ul de configurare si de log-uri
static char CONFIG_FILE[] = "NODESI/CONFIG.BIN";
static char LOG_FOLDER[] = "NODESI/LOGS";

NodeManager::NodeManager(ISensor *sensor1, ISensor *sensor2)
{
    // se seteaza senzorii folositi de nod
    this->sensor1 = sensor1;
    this->sensor2 = sensor2;

    // se initializeaza valorile pentru configurarea nodului in cazul in care nu exista un fisier de configurare pentru a le suprascrie
    configData_st.data.saveToLogIntervalInSeconds_u32 = 60;
    configData_st.data.lastBatchIndex_u16 = 0;
}

NodeManager::~NodeManager()
{
}

// aceasta functie se apeleaza periodic pentru a scrie informatiile de la senzori
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
    // se verifica daca se poate citi SD Card-ul
    Serial.print("Initializing SD Card...");
    if (g_SDCardManager.InitSDCard())
    {
        Serial.println("success");
        // Creaza folder-ul de log-uri daca nu exista
        if (!g_SDCardManager.CreateFolder(LOG_FOLDER))
        {
            Serial.println("Could not create LOG Folder");
            g_BTCommunicationProtocol.SendSDCardError(SD_CREATE_FOLDER_ERROR);
        }

        // se scrie fisierul de configuratie a nodului, daca acesta nu exista
        if (!g_SDCardManager.ExistsFile(CONFIG_FILE))
        {
            WriteConfigFile();
        }
        else
        {
            // se incearca citirea fisierului de configurare al nodului
            if (!g_SDCardManager.OpenFile(CONFIG_FILE))
            {
                // se trimite pe seriala mesajul de eroare SD_OPEN_FILE_ERROR
                g_BTCommunicationProtocol.SendSDCardError(SD_OPEN_FILE_ERROR);
            }
            else
            {
                // se citesc informatiile de la fisierul de configurare al nodului
                if (!g_SDCardManager.ReadRawBytes(configData_st.rawBytes, sizeof(configData_st.rawBytes)))
                {
                    // se trimite pe seriala mesajul de eroare SD_READ_ERROR
                    g_BTCommunicationProtocol.SendSDCardError(SD_READ_ERROR);
                }
                g_SDCardManager.CloseFile();
            }
        }

        // dupa ce s-au citit informatiile din fiserul de configurare
        // se seteaza periodicitate de citire si salvare a datelor in log
        SetIntervalTime(configData_st.data.saveToLogIntervalInSeconds_u32);

        // se creeaza un log-ul curent
        char logFileName[25] = "";
        CreateLogFileName(logFileName, configData_st.data.lastBatchIndex_u16);

        SensorLogData logData;
        memset(&logData, 0, sizeof(SensorLogData));

        if (g_SDCardManager.ExistsFile(logFileName))
        {
            // daca exista un log anterior, se va scrie in el mesajul ca aceasta s-a incheiat
            logData.data.batchIndex_u16 = configData_st.data.lastBatchIndex_u16;
            logData.data.logType = LOG_ENDED;

            AppendToLogFile(logFileName, &logData);

            configData_st.data.lastBatchIndex_u16++;
            WriteConfigFile();

            CreateLogFileName(logFileName, configData_st.data.lastBatchIndex_u16);
        }

        // se scrie in log-ul curent mesajul ca acesta a inceput
        logData.data.batchIndex_u16 = configData_st.data.lastBatchIndex_u16;
        logData.data.logType = LOG_STARTED;

        AppendToLogFile(logFileName, &logData);
    }
    else
    {
        // se scrie pe seriala mesajul SD_INIT_ERROR pentru ca nu s-a putut conecta SD Card-ul
        Serial.println("Initialization failed!");
        g_BTCommunicationProtocol.SendSDCardError(SD_INIT_ERROR);
    }
}

void NodeManager::WakeUp()
{
    // neimplementata inca
    // aceasta functie ar trebui sa "trezeasca" nodul din modul "sleep"
}

void NodeManager::GoToSleep()
{
    // neimplementata inca
    // aceasta functie ar trebui sa puna nodul in modul "sleep" pentru a economisi bateria
}

void NodeManager::SetTime(uint64_t time_u64) // timpul este UNIX Time
{
    // se scrie in log-ul curent un mesaj care contine timpul primit ca parametru
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
    // se seteaza cat de des sa se faca citirile de la senzori
    configData_st.data.saveToLogIntervalInSeconds_u32 = interval_u32;
    SetIntervalTime(configData_st.data.saveToLogIntervalInSeconds_u32);
    WriteConfigFile(); // se rescrie fisierul de configurare cu noile date
}

void NodeManager::GetSensorTypes(SensorTypes sensorTypes[2])
{
    // se populeaza vectorul sensorTypes cu tipul de senzori disponibili in nod
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
    // se calculeaza numarul de senzori ai nodului
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
    // se opreste citirea informatiilor din log
    if (g_SDCardManager.HasData())
    {
        g_SDCardManager.CloseFile();
        g_BTCommunicationProtocol.SendCloseFileMsg();
    }
    currentBatchLogIndex_u16 = configData_st.data.lastBatchIndex_u16 + 1;
}

void NodeManager::StartAllLogsDataReplay()
{
    // se incepe citirea informatiilor din log-uri pornind de la primul log
    currentBatchLogIndex_u16 = 0;

    OpenNextLogFileForRead();
}

void NodeManager::StartCurrentLogDataReplay()
{
    // se incepe citirea informatiilor din log-uri pornind de la log-ul curent
    currentBatchLogIndex_u16 = configData_st.data.lastBatchIndex_u16;

    OpenNextLogFileForRead();
}

void NodeManager::ReadLiveSensorData(SensorLogData *data_ptr, bool firstSensor_b)
{
    data_ptr->data.time_u64 = CalculateCurrentTime();
    data_ptr->data.batchIndex_u16 = configData_st.data.lastBatchIndex_u16;
    data_ptr->data.logType = LOG_LIVE_DATA;

    // se citesc datele de la senzori

    if (firstSensor_b)
    {
        // se citesc informatiile de la primul senzor
        if (sensor1 != NULL)
        {
            data_ptr->data.sensorType_en = sensor1->GetType();
            data_ptr->data.sensorValue = sensor1->ReadValue();
        }
        else
        {
            data_ptr->data.sensorType_en = NO_TYPE;
        }
    }
    else
    {
        // se citesc informatiile de la al doilea senzor
        if (sensor2 != NULL)
        {
            data_ptr->data.sensorType_en = sensor2->GetType();
            data_ptr->data.sensorValue = sensor2->ReadValue();
        }
        else
        {
            data_ptr->data.sensorType_en = NO_TYPE;
        }
    }
}

bool NodeManager::ReadSensorDataFromLog(SensorLogData *data_ptr)
{
    // se verifica daca s-a ajuns la ultimul fisier
    if (currentBatchLogIndex_u16 <= configData_st.data.lastBatchIndex_u16)
    {
        // se citeste o inregistrare din fisier
        if (g_SDCardManager.HasData())
        {
            g_SDCardManager.ReadRawBytes(data_ptr->rawBytes, sizeof(data_ptr->rawBytes));
            return true;
        }

        // daca citirea nu s-a putut efectua, atunci s-au terminat datele din fisierul deschis
        g_SDCardManager.CloseFile();

        // se trimite mesajul pe seriala ca s-a terminat citirea unui fisier
        g_BTCommunicationProtocol.SendCloseFileMsg();

        // se incerca sa se treaca la fisierul urmator
        currentBatchLogIndex_u16++;
        if (currentBatchLogIndex_u16 > configData_st.data.lastBatchIndex_u16)
        {
            // daca s-a ajuns aici, inseamna ca s-au terminat de citit toate informatiile din toate log-urile
            // se trimite mesajul corespunzator acestei situatii pe seriala
            g_BTCommunicationProtocol.SendLogReplayDoneMsg();
            return false;
        }

        // se deschide urmator fisier
        OpenNextLogFileForRead();

        // daca acesta are date, se citeste o inregistrare din fisier
        if (!g_SDCardManager.HasData())
        {
            return false;
        }
        g_SDCardManager.ReadRawBytes(data_ptr->rawBytes, sizeof(data_ptr->rawBytes));

        return true;
    }

    // daca s-a ajuns aici, inseamna ca nu mai exista fisiere de citit
    return false;
}

// nefolosita inca
void NodeManager::ReplayLogFile(char *logFileName)
{
    SensorLogData data;

    // se citesc toate datele dintr-un fisier
    g_SDCardManager.OpenFile(logFileName);
    while (g_SDCardManager.HasData())
    {
        g_SDCardManager.ReadRawBytes(data.rawBytes, sizeof(data.rawBytes));
    }
    g_SDCardManager.CloseFile();
}

inline uint64_t NodeManager::CalculateCurrentTime()
{
    // se calculeaza numarul de secunde care au trecut de cand ruleaza nodul
    unsigned long milliseconds = millis();
    return milliseconds / 1000;
}

void NodeManager::WriteConfigFile()
{
    // se incearca scrierea de informatii in fisierul de configurare al nodului
    // daca acest lucru nu este posibil, atunci se va trimite pe seriala un mesaj corespunzator
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
    // aceasta functie creeaza un sir de caractere corespunzator fisier-ului de log care are index-ul batchIndex_u16
    strcpy(str, LOG_FOLDER);
    strcat(str, "/L");
    char lognumber[7];
    itoa(batchIndex_u16, lognumber, 10);
    strcat(str, lognumber);
    strcat(str, ".BIN");
}

void NodeManager::AppendToLogFile(char *logFileName, SensorLogData *logData_ptr)
{
    // se incearca scrierea in fisierul de calea logFileName a unei informatii citite de la senzor
    // daca acest lucru nu reuseste, atunci se va scrie pe seriala un mesaj corespunzator
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
    // se trimite mesajul pe seriala ca un fisier a fost deschis
    g_BTCommunicationProtocol.SendOpenFileMsg();

    // se creeaza calea catre fisierul de log din care trebuie citite informatiile
    char logFileName[25] = "";
    CreateLogFileName(logFileName, configData_st.data.lastBatchIndex_u16);
    // se deschide fisierul de log pentru a citi informatiile
    g_SDCardManager.OpenFile(logFileName);
}