#ifndef Led_h
#define Led_h
#include <Arduino.h>

class Led
{
    public:
    Led(uint8_t pin);
    void setState(uint8_t state);
    void setBlink(uint16_t period);
    void setBlink(uint16_t period, uint32_t time);
    void handle();

    private:
    
    uint8_t pin;
    uint8_t state;
    uint8_t mode = 0;
    uint32_t lastMillis;
    uint16_t interval;
};
#endif