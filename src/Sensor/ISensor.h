#ifndef ISENSOR_H
#define ISENSOR_H

class ISensor
{
public:
    ISensor(int pin);
    virtual ~ISensor();
    virtual float ReadValue();

protected:
    int pin;
};

#endif // ISENSOR_H