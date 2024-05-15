#ifndef _blink_h
#define _blink_h
#include <Arduino.h>

class Blink
{
private:
    int num;
    bool active;
    void (*callbackFunction)(int); 
    unsigned long lastmillis;
    unsigned long ONinterval = 50;
    unsigned long OFFinterval = 150;

public:
    Blink(unsigned int ONinterval = 50, unsigned int OFFinterval = 150);
    void setCallback(void (*callback)(int));
    void blink(unsigned int num = 1);
    void handle();
};

#endif