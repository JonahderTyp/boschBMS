#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "config.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

#define OLED_RESET -1       // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

uint8_t display_begin()
{
  debugln("searching for display");
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
  {
    debugln("SSD1306 allocation failed");
    return 0;
  }
  debugln("SSD1306 connected!");
  display.clearDisplay();
  display.dim(true);
  display.display();
  return 1;
}

void display_battery(uint8_t level, uint8_t x, uint8_t y, uint8_t hight)
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

void display_battery_text(uint8_t level, uint8_t x, uint8_t y, uint8_t hight)
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

void display_temp(float _battery_temp)
{
  char str[4];
  dtostrf(_battery_temp, 3, 1, str);
  display.setCursor(15, 12);
  display.print(str);
  display.drawCircle(display.getCursorX() + 3, display.getCursorY(), 2, WHITE);
  display.setCursor(display.getCursorX() + 8, display.getCursorY());
  display.println(F("C"));
}

void display_pow(float _battery_power)
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
  display.print(F("W"));
}

void display_amp(int16_t _battery_current)
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
  display.print(F("A"));
}

void display_volt(int32_t _battery_voltage)
{
  char str[4];
  dtostrf((float)_battery_voltage / 1000.0, 3, 1, str);
  display.setCursor(70, 12);
  display.print(str);
  display.println(F("V"));
}

void display_status()
{

  display.setCursor(16, 24);
  display.setTextColor(BLACK, WHITE);
  display.print("1234567891011121");
}