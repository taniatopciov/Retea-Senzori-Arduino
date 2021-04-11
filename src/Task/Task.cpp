#include "Task.h"

Task::Task()
{
}

Task::~Task()
{
}

void Task::Tick()
{
    milliseconds_u64 = millis();
    if (milliseconds_u64 - elapsedTime_u64 >= intervalTime_u64)
    {
        elapsedTime_u64 = milliseconds_u64;
        Execute();
    }
}

void Task::ResetTaskCounter()
{
    elapsedTime_u64 = milliseconds_u64;
}

void Task::SetIntervalTime(uint64_t intervalTime_u64)
{
    elapsedTime_u64 = millis();

    this->intervalTime_u64 = intervalTime_u64;
}