/*
    Description: Use COLOR Unit to read C, R, G, B values and serial print output.
    Please install library before compiling:
    Adafruit TCS34725: https://github.com/adafruit/Adafruit_TCS34725
*/

#include <Preferences.h>
#include <iostream>
#include <sstream>

#if defined( ARDUINO_M5STACK_Core2 )
#include <M5Core2.h>
#include <FastLED.h>
#define NUM_LEDS 10
#define DATA_PIN 25
CRGB leds[NUM_LEDS];
#else
#include <M5Stack.h>
#endif

#include "Adafruit_TCS34725.h"

static uint16_t color16(uint16_t r, uint16_t g, uint16_t b) {
  uint16_t _color;
  _color = (uint16_t)(r & 0xF8) << 8;
  _color |= (uint16_t)(g & 0xFC) << 3;
  _color |= (uint16_t)(b & 0xF8) >> 3;
  return _color;
}

Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);
Preferences preferences;

float rScaler = 1;
float gScaler = 1;
float bScaler = 1;

void setup() {
  delay(100);
  //begin(LCDEnable=true, SDEnable=true, SerialEnable=true, I2CEnable=false);
  M5.begin(true, false, false);
#if defined( ARDUINO_M5STACK_Core_ESP32 )
  M5.Power.begin();
#else
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);  // GRB ordering is assumed
#endif
  Serial.begin(115200);
  Serial.println("Color View Test!");
  while (!tcs.begin()) {
    Serial.println("No TCS34725 found ... check your connections");
    M5.Lcd.setTextFont(4);
    M5.Lcd.setTextColor(TFT_WHITE);
    M5.Lcd.drawString("No Found sensor.", 50, 100, 4);
    delay(1000);
  }
  preferences.begin("ColDuo", true); // Read only mode.
  rScaler = preferences.getFloat("rScaler", 1);
  gScaler = preferences.getFloat("gScaler", 1);
  bScaler = preferences.getFloat("bScaler", 1);
  preferences.end();
  tcs.setIntegrationTime(TCS34725_INTEGRATIONTIME_154MS);
  tcs.setGain(TCS34725_GAIN_4X);
  M5.Lcd.setTextColor(TFT_WHITE);
  M5.Lcd.setTextFont(2);
  M5.Lcd.setTextDatum(ML_DATUM);
}

void loop() {
  uint16_t clear, red, green, blue;

  tcs.getRawData(&red, &green, &blue, &clear);

  // Range RGB to be 0 to 255.
  uint32_t sum = clear;
  float r, g, b;
  r = red; r /= sum;
  g = green; g /= sum;
  b = blue; b /= sum;
  r *= 256; g *= 256; b *= 256;

  // Calibrate RGB when pointed at GREY/WHITE paper.
  // Store in preferences.
  M5.update();
  if (M5.BtnC.wasReleasefor(700)) {
    float target = 0; // Middle brightness of R G or B.
    if (((g <= r) && (r < b)) || ((b < r) && (r < g))) target = r;
    if (((r <= g) && (g < b)) || ((b < g) && (g < r))) target = g;
    if (((r <= b) && (b < g)) || ((g < b) && (b < r))) target = b;
    float rChange = (r / target);
    float gChange = (g / target);
    float bChange = (b / target);
    if (rChange == 0) rScaler = 1; else rScaler = 1 / rChange;
    if (gChange == 0) gScaler = 1; else gScaler = 1 / gChange;
    if (bChange == 0) bScaler = 1; else bScaler = 1 / bChange;
    preferences.begin("ColDuo", false); // R/W mode.
    preferences.putFloat("rScaler", rScaler);
    preferences.putFloat("gScaler", gScaler);
    preferences.putFloat("bScaler", bScaler);
    preferences.end();
    M5.Lcd.drawString("CALIBRATED", 120, 180);
  }

  // Adjust RGB by calibration values.
  r *= rScaler; g *= gScaler; b *= bScaler;
  if (r < 0) r = 0;
  if (r > 255) r = 255;
  if (g < 0) g = 0;
  if (g > 255) g = 255;
  if (b < 0) b = 0;
  if (b > 255) b = 255;

  // Update LED's on the Core2
#if defined( ARDUINO_M5STACK_Core2 )
  CRGB colRGB;
  colRGB.r = (int)r >> 2;
  colRGB.g = (int)g >> 2;
  colRGB.b = (int)b >> 2;
  for (int i = 0; i < 10 ; i++) {
    leds[i] = colRGB;
  }
  FastLED.show();
#endif

  // Set the screen color to the RGB color.
  uint16_t _color = color16((int)r, (int)g, (int)b);
  M5.Lcd.clear(_color);

  // Display calibrated RGB values.
  M5.Lcd.drawString("C : " + String(clear), 10, 10);
  M5.Lcd.drawString("R : " + String((int)r), 10, 40);
  M5.Lcd.drawString("G : " + String((int)g), 10, 60);
  M5.Lcd.drawString("B : " + String((int)b), 10, 80);

  M5.Lcd.drawString("x " + String(rScaler), 70, 40);
  M5.Lcd.drawString("x " + String(gScaler), 70, 60);
  M5.Lcd.drawString("x " + String(bScaler), 70, 80);

  long int rgb = ((int)r<<16) | ((int)g << 8) | (int)b;
  char out[14];
  sprintf(out, "Col: # %02X %02X %02X", (int)r, (int)g, (int)b);
  M5.Lcd.drawString(out, 10, 110);

  M5.Lcd.drawString("To calibrate : Point sensor at white paper.", 10, 200);
  M5.Lcd.drawString("Hold button C for 1 second, then let go.", 10, 220);
  
}
