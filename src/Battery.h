#ifndef battery_h_
#define battery_h_
#include "Arduino.h"

class Battery
{
public:
    /*Status: 0x0: run, 0xffff: charge*/
    uint16_t status;
    /*current in mA*/
    int32_t current;
    /*percent 0 - 100*/
    uint8_t percent;
    /*voltage in mV*/
    uint16_t voltage;
    /*uptime in seconds*/
    uint32_t uptime;
    /*Wattage in 1 watt step*/
    uint16_t power;
    /*temperatur in °C*/
    float temp;

    /*battery connected*/
    uint8_t connected = false;

    /*if true, no critical errors occured*/
    uint8_t enable = false;

    /*battery acivate*/
    uint8_t turnon = true;

    /*  0: No battery connected
     *   1: non-Can Battery
     *   2: Can Battery
     */
    uint8_t type = 0;

    // general
    /*input voltage in mv*/
    uint16_t messuredVoltage;
    uint8_t cells;

    int8_t getBatteryCells(uint16_t voltage)
    {
        for (uint8_t i = 0; i <= 12; i++)
        {
            if (voltage <= uint16_t((i * 4200) + 100) && voltage >= uint16_t((i * 3274) - 100))
                return i;
            else if (voltage <= uint16_t(((i + 1) * 3274) - 100) && voltage >= uint16_t((i * 4200) + 100))
                return -1;
        }
        return 0;
    }

    String dataToString(uint16_t data)
    {
        char str[6];
        if (data / 1000.0 < 10)
        {

            dtostrf(data / 1000.0, 4, 3, str);
        }
        else
        {
            dtostrf(data / 1000.0, 5, 3, str);
        }

        return str;
    }

    void setDefaultValues()
    {
        this->current = 0;
        this->voltage = 0;
        this->percent = 0;
        this->uptime = 0;
        this->power = 0;
        this->temp = 0;
        this->connected = 0;
    }

private:
};

#endif
