

#include <Arduino.h>
#include "PCF8574.h"
#include <Adafruit_BusIO_Register.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ILI9341.h> //for tft spi 2.8" screen
//#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>
#include <OneButton.h>

int measurement_done_flag = 0;
int pound = 0;

#define mosi   12
#define sclk   14
#define miso   13
#define net_ss 24
 
#define TFT_CS        15
#define TFT_RST       -1 // Or set to -1 and connect to Arduino RESET pin
#define TFT_DC        4
#define TFT_BACKLIGHT -1 // Display backlight pin

 //inititialize display with software spi
// Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, mosi, sclk, TFT_RST);
  Adafruit_ILI9341 tft = Adafruit_ILI9341 (TFT_CS, TFT_DC, mosi, sclk, TFT_RST);
  
#define pcf_addr  0x20
PCF8574 pcf(pcf_addr);

//PCF8574 pins
//pcf
#define rtc_int    0
#define sim_pwr    1
#define up         2
#define left       3
#define down       4
#define right      5
#define ok         6
#define set        7

void init_sys()
{     
     Serial.println("starting TFT display");
     
      tft.begin();
      //tft action and test
      tft.fillScreen(ILI9341_BLACK);
      tft.setCursor(0, 0);
      tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(3);
      tft.println("Hello. WELCOME!");
      tft.println("PRESS BUTTON");
     //pcf
     Wire.begin();
     pcf.begin();
     //setting pcf pinmode
//     pcf.pinMode(rtc_int, OUTPUT);
//     pcf.pinMode(sim_pwr, OUTPUT);
//     pcf.pinMode(up,    INPUT);
//     pcf.pinMode(left,  INPUT);
//     pcf.pinMode(down,  INPUT);
//     pcf.pinMode(right, INPUT);
//     pcf.pinMode(ok,    INPUT);
//     pcf.pinMode(set,   INPUT);
        pcf.readButton(set);
     tft.fillScreen(ILI9341_BLACK); //CLEAR SCREEN
  }

  void pcf_read(int x){
    
       Wire.requestFrom(pcf_addr, 1);
  }
