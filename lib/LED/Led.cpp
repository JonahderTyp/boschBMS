#include "Arduino.h"
#include "Led.h"

Led::Led(uint8_t _pin)
{
    pin = _pin;
}

void Led::setState(uint8_t _state)
{
    if(this -> mode == 2){
        
    }
    this -> state = state;
    this -> mode = 1;
}

void Led::setBlink(uint16_t period)
{
    this->mode = 2;
    this -> interval = period;
}

void Led::setBlink(uint16_t period, uint32_t timeout)
{
    this->mode = 2;
}

void Led::handle()
{
    //uint32_t currntMillis = millis();
    if(mode == 2){
        
    }
}