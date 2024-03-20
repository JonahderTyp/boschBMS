#ifndef Speaker_h
#define Speaker_h
#include "Arduino.h"

class Speaker
{
    public:

    Speaker(uint8_t pin);


    void setSilence(boolean silence);

    void beep(uint32_t duration);
    void beep(uint32_t ontime, uint32_t pausetime);
    void beep(uint32_t ontime, uint32_t offtime, uint32_t pausetime, uint8_t beeps);
/*
    void beepfor(uint32_t ontime, uint32_t pausetime, uint32_t time);

    void beepfor(uint32_t ontime, uint32_t offtime, uint32_t pausetime, uint32_t time);*/

    void handle();

    private:

    boolean silenced = false;

    
    void tone();
    void notone();

    uint8_t pin;            //pin of speaker
    uint32_t ontime;        //time the speaker will be sounding tone
    uint32_t offtime;       //time the speaker will pause (only for multiple beeps)
    uint32_t pausetime;     //time the speaker will pause after beep(s)
    uint32_t turnofftime;   //time the speaker will beep for | 0 for indefinetly
    uint8_t beeps;          //amount of beeps

    uint32_t enabletime;    //time the speaker was turned on
    
    uint8_t beeperstate;


    int step = 0;
    int beepsmade = 0;
    uint32_t lasttime = 0;

};


#endif