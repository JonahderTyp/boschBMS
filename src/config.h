#pragma once

#define enabledebug

// Pin mapping
#define LED_HB_PIN 2
#define LED_EXTC_PIN 15
#define LED_STA_PIN 0
#define SPK_PIN 25
#define FAN_PIN 12
#define LCD_SDA 21
#define LCD_SCL 22

// Maximal temperature for battery
#define BATTERY_OVERTEMP 60 // 60°C

// Maximal current for battery
#define MAX_CURRENT 5

#define LMT_SHOUTDOWN_PRIO 150

#define SSID "BoschBMS"
#define PASS "BoschBMS"

#ifdef enabledebug
#define debugbegin(a) Serial.begin(a)
#define debug(a) Serial.print(a)
#define debugln(a) Serial.println(a)
#define debugf1(a) Serial.printf(a)

#define debugf2(a, b) Serial.printf(a, b)

#define debugf3(a, b, c) Serial.printf(a, b, c)

#else
#define debugbegin(a)
#define debug(a)
#define debugln(a)
#define debugf(a, b)
#define debugfln(a, b)
#endif
