#ifndef advancedDisplay_H_
#define advancedDisplay_H_
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "config.h"
#include "measurement.h"
#include <vector>

class advancedDisplay
{
public:
    advancedDisplay();

    void init(unsigned char splashscreen[]);

    void setMesurments(std::vector<Measurement>* m);

    void gotoPage(uint8_t page);
    void advancepage();
    void handle();

private:
#define SCREEN_WIDTH 128    // OLED display width, in pixels
#define SCREEN_HEIGHT 32    // OLED display height, in pixels
#define OLED_RESET -1       // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32


    std::vector<Measurement>* measurements;
    //ButtonRead button;

    // void settingsPage();
    // void DataPage(uint8_t type);
    // void DataPageCAN();
    // void display_volt(uint32_t _battery_voltage);
    // void display_amp(int16_t _battery_current);
    // void display_pow(float _battery_power);
    // void display_temp(float _battery_temp);
    // void display_battery(uint8_t level, uint8_t x, uint8_t y, uint8_t hight);
    // void display_battery_text(uint8_t level, uint8_t x, uint8_t y, uint8_t hight);

    Adafruit_SSD1306 display;
    //uint8_t buttonpress = 0;

    
};

#endif