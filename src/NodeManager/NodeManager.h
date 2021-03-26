#ifndef NODE_MANAGER_H
#define NODE_MANAGER_H

#include "Sensor/SensorData.h"
#include "Sensor/ISensor.h"
#include "Sensor/SensorTypes.h"

class NodeManager
{
public:
    NodeManager(ISensor *sensor1, ISensor *sensor2);
    ~NodeManager();

    void WakeUp();
    void GoToSleep();

    void GetSensorTypes(SensorTypes sensorTypes[2]);

    void StartDataReplay();
    bool HasNextLog();
    void ReadSensorData(SensorData *data_ptr);

private:
    ISensor *sensor1;
    ISensor *sensor2;

    int currentValue;
};

extern NodeManager g_NodeManager;

#endif // NODE_MANAGER_H