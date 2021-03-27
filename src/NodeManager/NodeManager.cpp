#include "NodeManager.h"
#include "Arduino.h"

NodeManager::NodeManager(ISensor *sensor1, ISensor *sensor2)
{
    this->sensor1 = sensor1;
    this->sensor2 = sensor2;
}

NodeManager::~NodeManager()
{
}

void NodeManager::WakeUp()
{
}

void NodeManager::GoToSleep()
{
}

void NodeManager::StartDataReplay()
{
    currentValue = 0;
}
bool NodeManager::HasNextLog()
{
    return currentValue != 2;
}

void NodeManager::ReadSensorDataFromLog(SensorData *data_ptr)
{
    data_ptr->sensorType_en = (SensorTypes)(currentValue + 1);
    data_ptr->sensorValue = 2.0f;
    data_ptr->time = 0;

    currentValue++;
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