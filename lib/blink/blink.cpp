#include "blink.h"

Blink::Blink(unsigned int ONinterval, unsigned int OFFinterval)
{
    Blink::ONinterval = ONinterval;
    Blink::OFFinterval = OFFinterval;
    Blink::callbackFunction = nullptr;
}

void Blink::setCallback(void (*callback)(int))
{
    // Set callback function
    Blink::callbackFunction = callback;
}

void Blink::blink(unsigned int num)
{
    if (num == 0)
    {
        return;
    }
    if (Blink::num == 0)
    {
        Blink::lastmillis = millis();
        Blink::active = true;
        callbackFunction(Blink::active);
    }
    Blink::num += num;
}

void Blink::handle()
{
    if (((millis() - Blink::lastmillis >= Blink::ONinterval && Blink::active) || (millis() - Blink::lastmillis >= Blink::OFFinterval && !Blink::active)) && Blink::num != 0)
    {
        Blink::active = !Blink::active;
        Blink::lastmillis = millis();
        callbackFunction(Blink::active);
        if (Blink::active == false)
        {
            Blink::num--;
        }
    }
}