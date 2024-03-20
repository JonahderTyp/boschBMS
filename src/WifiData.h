#ifndef WifiData_h_
#define WifiData_h_
#include "Arduino.h"

class WifiData
{
public:
    int8_t wifimode = 0;
    String APssid = "BoschBMS";
    String APpasswd = "BoschBMS";
};

#endif
