#include <Arduino.h>
#include <ESP32CAN.h>
#include <CAN_config.h>
#include <SPIFFS.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include "ESPAsyncWebServer.h"
#include <ArduinoJson.h>
#include <Adafruit_NeoPixel.h>
#include <stdlib.h>
#include <ErrorManager.h>
#include <measurement.h>

#include "config.h"
#include "Battery.h"
#include "advancedDisplay.h"
#include "tools.h"
#include "Speaker.h"
#include "Led.h"
#include "Error.h"

// Internal Temperature in Celsius
Measurement internalTemp("Internal Temp", "°C");
// Current in mA
Measurement current("Current", "mA");
// Battery Temperature
Measurement battTemp("Battery Temperature", "°C");



Battery BatteryData;
//WifiData wifiData;
// AsyncWebServer server(80);
advancedDisplay display();

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

void handleHBled()
{
  if (0)
  {
    digitalWrite(LED_HB_PIN, LOW);
    if (millis() - previousHBMillis >= 100)
    {
      previousHBMillis = millis();
      digitalWrite(LED_STA_PIN, !digitalRead(LED_STA_PIN));
      debugln("---ERROR---");
    }
  }
  else
  {
    digitalWrite(LED_STA_PIN, LOW);
    if (millis() - previousHBMillis >= 1000)
    {
      previousHBMillis = millis();
      digitalWrite(LED_HB_PIN, !digitalRead(LED_HB_PIN));
      debugln("---HB---");
      // buz.beep(10);
    }
  }
}

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

void initWiFi()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin("smarttest", "smarttest");
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED)
  {
    debugln('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
  // WiFi.config
}

// String processor(const String &var)
// {
//   debugln(var);
//   return String();
// }

String getBatteryVoltage()
{
  return String(String(BatteryData.voltage / 1000) + "V");
}

void initalizeWifi(uint8_t mode)
{
  // static boolean initalized = false;
  if (mode == 1)
  {
    initWiFi();
  }
  if (mode == 2)
  {
    WiFi.softAP(SSID, PASS, 1, 0, 4);
    debug("IP address: ");
    debugln(WiFi.softAPIP());

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/index.html"); });

    server.on("/favicon.png", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/favicon.png", "image/png"); });

    server.on("/Chart.js", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/Chart.js"); });

    /*server.on("/getVoltage", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send_P(200, "text/plain", String(battery_voltage).c_str()); });*/

    server.on("/setColors", HTTP_GET, [](AsyncWebServerRequest *request)
              {
    AsyncResponseStream *response = request->beginResponseStream("application/json");
      DynamicJsonDocument json(1024);
      //json["voltage_card"] = "#FFA500";
      json["current_card"] = (BatteryData.current > 5000) ? ((BatteryData.current > 8000) ? "#FF4500" : "#ffa500" ) : "#2E8B57" ;
      //json["watt_card"] = "#FDF5E6";
      json["percent_card"] = (BatteryData.percent < 20) ? ((BatteryData.percent < 10) ? "#FF4500" : "#ffa500" ) : "#2E8B57";
      serializeJson(json, *response);
      request->send(response); });

    server.on("/getData", HTTP_GET, [](AsyncWebServerRequest *request)
              {
    AsyncResponseStream *response = request->beginResponseStream("application/json");
      DynamicJsonDocument json(1024);
      json["Voltage"] = String(BatteryData.voltage/1000.0);
      json["Current"] = String(BatteryData.current/1000.0, 3);
      json["Watt"] = String(BatteryData.power);
      json["Percent"] = String(BatteryData.percent);
      serializeJson(json, *response);
      request->send(response); });

    server.begin();
  }
}

void handle_led()
{
  int8_t percent = -10;
  for (uint8_t i = 6; i >= 2; i--)
  {
    percent += 20;
    if (percent <= BatteryData.percent)
    {
      strip.setPixelColor(i, strip.Color(0, 255, 0));
    }
    else
    {
      strip.setPixelColor(i, strip.Color(0, 0, 0));
    }
  }

  strip.show();
}

void setup()
{
  debugbegin(115200);

  errorManager.addError(overcurrent);
  errorManager.addError(undervoltage);
  errorManager.addError(batteryOvertemp);
  errorManager.addError(batteryDisconect);
  errorManager.addError(internalOvertemp);

  strip.begin();            // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();             // Turn OFF all pixels ASAP
  strip.setBrightness(255); // Set BRIGHTNESS to about 1/5 (max = 255)

  for (uint8_t i = 0; i < strip.numPixels(); i++)
  {
    strip.setPixelColor(i, strip.Color(255, 255, 255));
  }
  strip.show();

  buz.beep(10, 50, 1, 1);

  // display
  display.init(splashscreen);

  // LEDS
  pinMode(LED_STA_PIN, OUTPUT);
  pinMode(LED_HB_PIN, OUTPUT);
  pinMode(LED_EXTC_PIN, OUTPUT);
  pinMode(SPK_PIN, OUTPUT);

  // Initialize SPIFFS
  if (!SPIFFS.begin(true))
  {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // WiFi
  //

  // Fan PWM Setup
  ledcSetup(2, 5000, 8);
  ledcAttachPin(FAN_PIN, 2);
  // ledcWrite(2, 255);

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
  // if (millis() - lastmeasurement >= 200)
  // {
  //   lastmeasurement = millis();
  // }

  readcan();

  if (BatteryData.temp > BATTERY_OVERTEMP)
    batteryOvertemp.throwError(300000); // timeout 5 min

  switch (errorManager.getMaxPrio())
  {
  case (int) Priority::critical:
    buz.beep(1000,500,0,5);
    break;
  case (int) Priority::warningHigh:
    buz.beep(1000,1000,0,4);
    break;
  case (int) Priority::warningMedium:
    buz.beep(100,500,1000,3);
    break;
  case (int) Priority::warningLow:
    //buz.beep(100,500,1000,2);
    break;
  case (int) Priority::nonCritical:
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

  handleHBled();
  handle_fan();
  handle_led();

  buz.handle();
  display.handle();

  yield();
}