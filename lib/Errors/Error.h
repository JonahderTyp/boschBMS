#ifndef Error_h
#define Error_h
#include "Arduino.h"


enum class Priority {nonCritical = 1, warningLow, warningMedium, warningHigh, critical};

/*priorities:
  1 -  49   non critical
 50 - 99   warning LOW
100 - 149   warning Medium
150 - 199   warning High
200 - 255   critical
*/
class Error
{
public:
    Error(Priority priority, const char *msg)
    {
        this->errorPrio = priority;
        this->errorText = msg;
    }
    const char* getErrorStr() const { return (this->errorText); }
    boolean isActive() const { return this->errorActive; }
    boolean isAck() const {return this->acknowledged;}

    void ack(){
        this->acknowledged = true;
    }

    void clearError()
    {
        //Serial.println("ClearingError");
        this->errorActive = false;
        this->acknowledged = false;
    }

    void throwError(uint64_t timeout = 0)
    {
        //Serial.println("throwing Error");
        this->errorActive = true;
        this->timeout = timeout;
    }

    uint8_t getPriority() const
    {
        return (int) this->errorPrio;
    }

    void handle();

    

private:
    boolean acknowledged = false;
    uint64_t timeout;
    uint8_t errorActive;
    Priority errorPrio;
    const char *errorText;
};
#endif