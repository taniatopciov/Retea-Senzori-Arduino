#include "BTCommunicationProtocol.h"
#include <string.h>

BTCommunicationProtocol::BTCommunicationProtocol(int BTTransmitPin, int BTReceivePin, ISensor *sensor1, ISensor *sensor2)
    : BTSerial(BTReceivePin, BTTransmitPin, 0)
{
    this->sensor1 = sensor1;
    this->sensor2 = sensor2;
    Reset();
}

BTCommunicationProtocol::~BTCommunicationProtocol()
{
}

void BTCommunicationProtocol::Reset()
{
    protocolState_en = WAITING_FOR_MESSAGE;
    currentTextBufferIndex = 0;
}

void BTCommunicationProtocol::Begin(long baudrate)
{
    BTSerial.begin(baudrate);
}

void BTCommunicationProtocol::Run()
{
    currentMillis = millis();

    StateMachineRun();

    if (currentMillis - lastMillis >= READ_TIMEOUT)
    {
        lastMillis = currentMillis;
        Reset();
    }
}

void BTCommunicationProtocol::StateMachineRun()
{
    switch (protocolState_en)
    {
    case WAITING_FOR_MESSAGE:
    {
        while (HasData())
        {
            lastMillis = currentMillis;

            lastMillis = currentMillis;
            textBuffer[currentTextBufferIndex++] = (char)BTSerial.read();

            if (currentTextBufferIndex == TEXT_BUFFER_SIZE)
            {
                textBuffer[currentTextBufferIndex] = '\0';
                currentTextBufferIndex = 0;

                if (strcmp(textBuffer, "CONN") == 0)
                {
                    protocolState_en = SEND_SENSOR_TYPES;
                }
                else if (strcmp(textBuffer, "MORE") == 0)
                {
                    protocolState_en = WAITING_FOR_SAMPLE_COUNT;
                }
                else
                {
                    Reset();
                }
            }
        }
    };
    break;

    case SEND_SENSOR_TYPES:
    {
        lastMillis = currentMillis;

        SendSensorTypes();
        protocolState_en = WAITING_FOR_SAMPLE_COUNT;
    }
    break;

    case WAITING_FOR_SAMPLE_COUNT:
    {
        sampleCountPerSensor = 0;
        while (HasData())
        {
            lastMillis = currentMillis;

            if (sampleCountPerSensor != 0)
            {
                sampleCountPerSensor = BTSerial.read();
            }
            protocolState_en = SEND_SAMPLES;
        }
    }
    break;

    case SEND_SAMPLES:
    {
        while (sampleCountPerSensor > 0)
        {
            lastMillis = currentMillis;

            // convert float to long
            // send bytes starting with LSB

            if (sensor1 != NULL)
            {
                float value = sensor1->ReadValue();
                long copyValue = *(long *)&value;
                for (unsigned int i = 0; i < sizeof(long); i++)
                {
                    BTSerial.write((copyValue >> (i * 8)) & 255);
                }
            }

            if (sensor2 != NULL)
            {
                float value = sensor2->ReadValue();
                long copyValue = *(long *)&value;
                for (unsigned int i = 0; i < sizeof(long); i++)
                {
                    BTSerial.write((copyValue >> (i * 8)) & 255);
                }
            }

            sampleCountPerSensor--;
        }

        protocolState_en = WAITING_FOR_MESSAGE;
    }
    break;
    }
}

bool BTCommunicationProtocol::HasData()
{
    return BTSerial.available();
}

void BTCommunicationProtocol::SendSensorTypes()
{
    if (sensor1 == NULL && sensor2 == NULL)
    {
        Reset();
    }

    if (sensor1 != NULL)
    {
        BTSerial.write(sensor1->GetType());
    }

    if (sensor2 != NULL)
    {
        BTSerial.write(sensor2->GetType());
    }
}