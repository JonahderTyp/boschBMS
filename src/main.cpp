#include <Arduino.h>
#include <ESP32CAN.h>
#include <CAN_config.h>
#include <Adafruit_NeoPixel.h>
#include <stdlib.h>
#include <ErrorManager.h>
#include <measurement.h>

#include "config.h"
#include "Battery.h"
#include "Speaker.h"
#include "Led.h"
#include "Error.h"
#include "blink.h"

Battery BatteryData;

Adafruit_NeoPixel strip(9, 0, NEO_GRB + NEO_KHZ800);

Error overcurrent(Priority::critical, "Battery Overcurrent");
Error undervoltage(Priority::critical, "Undervoltage");
Error batteryOvertemp(Priority::critical, "Battery Overtemp");
Error internalOvertemp(Priority::critical, "Internal Overtemp");
Error batteryDisconect(Priority::warningLow, "Battery Disconected");

ErrorManager errorManager;

Speaker buz(SPK_PIN);

CAN_device_t CAN_cfg; // CAN Config
CAN_frame_t tx_frame;
unsigned long previousMessage = 0; // stores last time a CAN Message was send
const int interval = 100;          // interval at which send CAN Messages (milliseconds)
const int rx_queue_size = 50;      // Receive Queue size
unsigned long lastmessage = 0;
unsigned long lastmeasurement = 0;
unsigned long previousHBMillis = 0;

uint8_t fanspeed;

Blink HBled(100, 1000);

uint64_t readcan()
{

  CAN_frame_t rx_frame;
  // Receive next CAN frame from queue
  while (xQueueReceive(CAN_cfg.rx_queue, &rx_frame, 3 * portTICK_PERIOD_MS) == pdTRUE)
  {
    // debugln("New Can DATA");
    switch (rx_frame.MsgID)
    {
    case 0x101:
      BatteryData.current = (rx_frame.data.u8[2] << 8) | rx_frame.data.u8[3];
      BatteryData.power = ((rx_frame.data.u8[4] << 8) | rx_frame.data.u8[5]) / 10;
      BatteryData.voltage = (rx_frame.data.u8[6] << 8) | rx_frame.data.u8[7];
      break;

    case 0x111:
      BatteryData.percent = rx_frame.data.u8[6];
      break;

    case 0x2AA:
      BatteryData.temp = (((int)(rx_frame.data.u8[0] << 8) | rx_frame.data.u8[1]) / 100.0) - 273.15;
      break;
    case 0x01C:
      BatteryData.uptime = ((int)(rx_frame.data.u8[0] << 24 | rx_frame.data.u8[1] << 16 | rx_frame.data.u8[2] << 8 | rx_frame.data.u8[3])) / 1000;
      break;
    }

    lastmessage = millis();
  }

  return millis() - lastmessage;
}

uint8_t activateBattery()
{
  if (millis() - previousMessage >= interval)
  {
    previousMessage = millis();
    // debugln("creating can frame");
    // debug("writing can frame.....");
    ESP32Can.CANWriteFrame(&tx_frame);
    // debugln("done");
    return 1;
  }
  return 0;
}

void handle_fan()
{
  if (BatteryData.current > 1000)
  {
    fanspeed = 255;
  }
  else if (BatteryData.current > 300)
  {
    fanspeed = 150;
  }
  else
  {
    fanspeed = 0;
  }
  ledcWrite(2, fanspeed);
}

void setup()
{
  debugbegin(115200);

  errorManager.addError(overcurrent);
  errorManager.addError(undervoltage);
  errorManager.addError(batteryOvertemp);
  errorManager.addError(batteryDisconect);
  errorManager.addError(internalOvertemp);

  for (uint8_t i = 0; i < strip.numPixels(); i++)
  {
    strip.setPixelColor(i, strip.Color(255, 255, 255));
  }
  strip.show();

  buz.beep(10, 50, 1, 1);

  // LEDS
  pinMode(LED_STA_PIN, OUTPUT);
  pinMode(LED_HB_PIN, OUTPUT);
  pinMode(LED_EXTC_PIN, OUTPUT);
  pinMode(SPK_PIN, OUTPUT);

  // CAN Setup
  CAN_cfg.speed = CAN_SPEED_500KBPS;
  CAN_cfg.tx_pin_id = GPIO_NUM_5;
  CAN_cfg.rx_pin_id = GPIO_NUM_4;
  CAN_cfg.rx_queue = xQueueCreate(500, sizeof(CAN_frame_t));

  tx_frame.FIR.B.FF = CAN_frame_std;
  tx_frame.MsgID = 0x09A;
  tx_frame.FIR.B.DLC = 4;
  tx_frame.data.u8[0] = 0x04;
  tx_frame.data.u8[1] = 0x00;
  tx_frame.data.u8[2] = 0x00;
  tx_frame.data.u8[3] = 0x00;
  ESP32Can.CANInit(); // Init CAN Module

  debugln("Ready");
}

void loop()
{

  readcan();

  if (BatteryData.temp > BATTERY_OVERTEMP)
    batteryOvertemp.throwError(300000); // timeout 5 min

  switch (errorManager.getMaxPrio())
  {
  case (int)Priority::critical:
    buz.beep(1000, 500, 0, 5);
    break;
  case (int)Priority::warningHigh:
    buz.beep(1000, 1000, 0, 4);
    break;
  case (int)Priority::warningMedium:
    buz.beep(100, 500, 1000, 3);
    break;
  case (int)Priority::warningLow:
    // buz.beep(100,500,1000,2);
    break;
  case (int)Priority::nonCritical:
    break;

  default:
    break;
  }

  if (errorManager.getMaxPrioNotAck() < LMT_SHOUTDOWN_PRIO && BatteryData.enable)
    activateBattery();

  if (internalOvertemp.isActive())
    BatteryData.enable = false;
  else
    BatteryData.enable = true;

  if (millis() - lastmessage >= 50)
  {
    batteryDisconect.throwError();
    BatteryData.setDefaultValues();
  }
  else
  {
    batteryDisconect.clearError();
  }

  handle_fan();
  buz.handle();

  yield();
}