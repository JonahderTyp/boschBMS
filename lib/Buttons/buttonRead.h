#ifndef buttonRead_h_
#define buttonRead_h_
#include "Arduino.h"

class ButtonRead
{
private:
#define buttons 2
#define longpresstime 1000
#define shortpresstime 100

    uint8_t buttonpin[buttons] = {13, 14};

    uint8_t lastbuttonstate[buttons];

    uint32_t presstimebutton[buttons];

    uint32_t releasetimebutton[buttons];

    uint8_t buttonpress[buttons];

public:
    ButtonRead()
    {
        for (int i = 0; i < buttons; i++)
        {
            pinMode(i, INPUT_PULLUP);
        }
    }

    void handle()
    {
        uint32_t currentMillis = millis();
        for (int i = 0; i < buttons; i++)
        {
            uint8_t currentbuttonstate = !digitalRead(buttonpin[i]);
            if (lastbuttonstate[i] && currentbuttonstate)
            {
                presstimebutton[i] = currentMillis;
            }
            else if (!lastbuttonstate[i] && currentbuttonstate)
            {
                releasetimebutton[i] = currentMillis;

                uint32_t pressduration = releasetimebutton[i] - presstimebutton[i];

                if (pressduration >= shortpresstime && buttonpress[i] == 0)
                {
                    if (pressduration >= longpresstime)
                        buttonpress[i] = 2;
                    else
                        buttonpress[i] = 1;
                }
            }
        }
    }

    uint8_t getButtonstate(int i)
    {
        uint8_t state = buttonpress[i];
        buttonpress[i] = 0;
        return state;
    }
};

#endif
