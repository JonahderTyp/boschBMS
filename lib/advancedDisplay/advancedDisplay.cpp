#include <Arduino.h>
#include "advancedDisplay.h"

advancedDisplay::advancedDisplay()
{
}

void advancedDisplay::init(const unsigned char splashscreen[])
{
    display = Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
    debugln("searching for display");
    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
    {
        debugln("SSD1306 allocation failed");
    }
    debugln("SSD1306 consected!");
    display.clearDisplay();
    display.dim(false);
    display.display();

    display.clearDisplay();
    display.drawBitmap(0, 0, splashscreen, 128, 32, WHITE);

    display.display();
}

void advancedDisplay::handle()
{
    static unsigned long lastmillis = 0;
    if (millis() - lastmillis >= 100)
    {
        lastmillis = millis();
        //DataPage(1);
    }

    // button.handle();
}

void advancedDisplay::setMesurments(std::vector<Measurement>* m){
    this->measurements = m;
}




void advancedDisplay::settingsPage()
{
    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextColor(BLACK, WHITE);
    display.print("Settings");

    display.display();
}

void advancedDisplay::DataPage(uint8_t type)
{
    display.clearDisplay();
    switch (type)
    {
    case 1:
        display.setTextColor(WHITE);
        display.setTextSize(1);
        display_battery_text(BatteryData->percent, 0, 0, 30);
        // display_battery(BatteryData->onboardbatterylevel, 118, 5, 22);
        display_amp(BatteryData->current);
        display_pow(BatteryData->power);
        display_temp(BatteryData->temp);
        display_volt(BatteryData->voltage);
        display.display();
        break;

    default:
        display.setCursor(0, 8);
        display.setTextColor(WHITE);
        display.print("No Battery Connected");
        display.setCursor(0, 16);
        display.print(String(String(BatteryData->messuredVoltage) + " V"));
        break;
    }

    display.display();
}



void advancedDisplay::display_battery(uint8_t level, uint8_t x, uint8_t y, uint8_t hight)
{
    display.drawRect(x, y + 2, 10, hight, SSD1306_WHITE);
    display.fillRect(x + 2, y, 6, 2, SSD1306_WHITE);

    if (level > 0 && level <= 100)
    {
        uint8_t val = map(level, 0, 100, 0, hight - 2);
        display.fillRect(x + 1, y + 3 + (hight - 2 - val), 8, val, SSD1306_WHITE);
    }
    else
    {
        display.drawLine(x + 1, y + (hight * 0.36), x + 8, y + (hight * 0.68), SSD1306_WHITE);
        display.drawLine(x + 1, y + (hight * 0.68), x + 8, y + (hight * 0.36), SSD1306_WHITE);
    }
}

void advancedDisplay::display_battery_text(uint8_t level, uint8_t x, uint8_t y, uint8_t hight)
{
    display_battery(level, x, y, hight);

    if (level > 0 && level <= 100)
    {
        display.setRotation(3);
        display.setCursor(5, 1);
        display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
        display.print(String(level));
        display.print("%");
        display.setRotation(0);
        display.setTextColor(SSD1306_WHITE);
    }
}

void advancedDisplay::display_volt(uint32_t _battery_voltage)
{
    char str[4];
    dtostrf((float)_battery_voltage / 1000.0, 3, 1, str);
    display.setCursor(70, 12);
    display.print(str);
    display.println(F(" V"));
}

void advancedDisplay::display_pow(float _battery_power)
{
    char str[4];

    if (_battery_power >= 100)
    {
        dtostrf(_battery_power, 3, 0, str);
    }
    else if (_battery_power >= 10)
    {
        display.print(F(" "));
        dtostrf(_battery_power, 3, 1, str);
    }
    else
    {
        display.print(F("  "));
        dtostrf(_battery_power, 3, 2, str);
    }

    display.setCursor(70, 0);
    display.print(str);
    display.print(F(" W"));
}

void advancedDisplay::display_amp(int16_t _battery_current)
{
    char str[6];
    if (_battery_current / 1000.0 < 10)
    {
        display.print(F(" "));
        dtostrf(_battery_current / 1000.0, 4, 3, str);
    }
    else
    {
        dtostrf(_battery_current / 1000.0, 5, 3, str);
    }

    display.setCursor(15, 0);
    display.print(str);
    display.print(F(" A"));
}

void advancedDisplay::display_temp(float _battery_temp)
{
    char str[4];
    dtostrf(_battery_temp, 3, 1, str);
    display.setCursor(15, 12);
    display.print(str);
    display.drawCircle(display.getCursorX() + 3, display.getCursorY(), 2, WHITE);
    display.setCursor(display.getCursorX() + 8, display.getCursorY());
    display.println(F("C"));
}
