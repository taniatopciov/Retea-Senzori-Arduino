#include "Task.h"

Task::Task()
{
}

Task::~Task()
{
}

// se verifica numarul de milisecunde care au trecut, iar daca diferenta este mai mare decat perioada, atunci se va executa functia
void Task::Tick()
{
    milliseconds_u64 = millis();
    if (milliseconds_u64 - elapsedTime_u64 >= intervalTime_u64)
    {
        elapsedTime_u64 = milliseconds_u64;
        Execute();
    }
}

// reseteaza numaratoarea
void Task::ResetTaskCounter()
{
    elapsedTime_u64 = milliseconds_u64;
}

void Task::SetIntervalTime(uint64_t intervalTime_u64)
{
    elapsedTime_u64 = millis();

    this->intervalTime_u64 = intervalTime_u64;
}