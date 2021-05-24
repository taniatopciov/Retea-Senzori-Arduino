#ifndef TASK_H
#define TASK_H

#include <Arduino.h>

/**
 * Aceasta clasa este folosita pentru a executa o functie periodic
 * Functia care trebuie implementata este Execute
 * Functia SetIntervalTime seteaza periodicitatea de executie a functiei
*/
class Task
{
public:
    Task();
    virtual ~Task();

    void SetIntervalTime(uint64_t intervalTime_u64);
    void ResetTaskCounter();

    virtual void Tick();
    virtual void Execute() = 0;

private:
    uint64_t milliseconds_u64;
    uint64_t intervalTime_u64;
    uint64_t elapsedTime_u64;
};

#endif // TASK_H