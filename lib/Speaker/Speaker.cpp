#include "Arduino.h"
#include "Speaker.h"

Speaker::Speaker(uint8_t _pin)
{
    pin = _pin;
    ledcSetup(1, 2400, 8);
    ledcAttachPin(_pin, 1);
}

void Speaker::setSilence(boolean silence){
    this->silenced = silence;
}

void Speaker::beep(uint32_t _ontime){
    beep(_ontime,0,0,1);
}

void Speaker::beep(uint32_t _ontime, uint32_t _offtime, uint32_t _pausetime, uint8_t _beeps)
{
    if(step == 4){
        this->beepsmade = 0;
        this->step = 0;
    }
    this->ontime = _ontime;
    this->offtime = _offtime;
    this->pausetime = _pausetime;
    this->beeps = _beeps;
}


void Speaker::handle()
{
    unsigned long currentMillis = millis();

    if(step == 0 && beepsmade < beeps){
        //Serial.print("Step: ");
        //Serial.println(step);
        lasttime = currentMillis;
        tone();
        step++;
        beepsmade++;
    }

    if(step == 1 && currentMillis - lasttime > ontime){
        //Serial.print("Step: ");
        //Serial.println(step);
        lasttime = currentMillis;
        notone();
        step++;
    }

    if(step == 2 && currentMillis - lasttime > offtime){
        //Serial.print("Step: ");
        //Serial.println(step);
        lasttime = currentMillis;
        step++;
    }

    if(step == 3 && currentMillis - lasttime > pausetime){
        //Serial.print("Step: ");
        //Serial.println(step);
        lasttime = currentMillis;
        if(beepsmade < beeps){
            step = 0;
        } else {
            step++;
        }
        
    }

}


void Speaker::tone(){
    //ledcWrite(1,128);
    if(!this->silenced){
        ledcWriteTone(1,2400);
    }
}

void Speaker::notone(){
    ledcWrite(1,0);
}