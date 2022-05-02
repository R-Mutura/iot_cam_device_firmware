#include "esp_camera.h"
 #include <SPI.h>
 #include <FS.h> 
 #include <SPIFFS.h>
 
#include "soc/soc.h"           // Disable brownour problems
#include "soc/rtc_cntl_reg.h"  // Disable brownour problems
#include "driver/rtc_io.h"
#include <EEPROM.h> 

// define the number of bytes you want to access
#define EEPROM_SIZE 1

   String spiffsFilename = "/image.jpg";     // image name to use when storing in spiffs
   String ImageResDetails = "Unknown";       // image resolution info

     #define PIXFORMAT PIXFORMAT_JPEG;                    // image format, Options =  YUV422, GRAYSCALE, RGB565, JPEG, RGB888
   int cameraImageExposure = 0;                         // Camera exposure (0 - 1200)   If gain and exposure both set to zero then auto adjust is enabled
   int cameraImageGain    = 0;                             // Image gain (0 - 30)
   int cameraImageBrightness = 0;                       // Image brightness (-2 to +2)

   int pictureNumber     = 0;
   bool sRes = 0;
// Pin definition for CAMERA_MODEL_AI_THINKER
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22
/////////////////////////////////////////////////////////////////////
////////////ptrototypess/////////////////////////
bool cameraImageSettings();


bool camera_init(){
    
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG; 
  
  if(psramFound()){
    config.frame_size = FRAMESIZE_UXGA; // FRAMESIZE_ + QVGA|CIF|VGA|SVGA|XGA|SXGA|UXGA
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }
  
   // Init Camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return 0;
  } 
  cameraImageSettings();

  //INITIALIZING SPIFFS - TO STORE THE IMAGE IN
  
   if (!SPIFFS.begin(true)) {
      Serial.println(("An Error has occurred while mounting SPIFFS - restarting"));
     delay(5000);
     ESP.restart();                               // restart and try again
     delay(5000);
   } else {
     // SPIFFS.format();      // wipe spiffs
    
       Serial.print(("SPIFFS mounted successfully: "));
       Serial.printf("total bytes: %d , used: %d \n", SPIFFS.totalBytes(), SPIFFS.usedBytes());
     }
  return 1;
}


   
//  
//// initialize EEPROM with predefined size
//  EEPROM.begin(EEPROM_SIZE);
//  pictureNumber = EEPROM.read(0) + 1;
//
//  // Path where new picture will be saved in SD Card
//  //String path = "/picture" + String(pictureNumber) +".jpg";
//
//
//}

//            interesting info on exposure times here: https://github.com/raduprv/esp32-cam_ov2640-timelapse

bool cameraImageSettings() {

    Serial.println("Applying camera settings");
   sensor_t *s = esp_camera_sensor_get();
  
 // something to try?:     if (s->id.PID == OV3660_PID)
   if (s == NULL) {
      Serial.println("Error: problem reading camera sensor settings");
     return 0;
   }

   // if both set to zero enable auto adjust
   if (cameraImageExposure == 0 && cameraImageGain == 0) {
     // enable auto adjust
       s->set_gain_ctrl(s, 1);                       // auto gain on
       s->set_exposure_ctrl(s, 1);                   // auto exposure on
       s->set_awb_gain(s, 1);                        // Auto White Balance enable (0 or 1)
       s->set_brightness(s, cameraImageBrightness);  // (-2 to 2) - set brightness
   } else {
     // Apply manual settings
       s->set_gain_ctrl(s, 0);                       // auto gain off
       s->set_awb_gain(s, 1);                        // Auto White Balance enable (0 or 1)
       s->set_exposure_ctrl(s, 0);                   // auto exposure off
       s->set_brightness(s, cameraImageBrightness);  // (-2 to 2) - set brightness
       s->set_agc_gain(s, cameraImageGain);          // set gain manually (0 - 30)
       s->set_aec_value(s, cameraImageExposure);     // set exposure manually  (0-1200)
   }

   return 1;
} 

int brightLEDbrightness = 0;
bool flashRequired = 0;


void take_pic()
{
   int currentBrightness = brightLEDbrightness;
   // if (flashRequired) brightLed(255);   // change LED brightness (0 - 255)
  // Take Picture with Camera
 camera_fb_t *fb = esp_camera_fb_get();  
  if(!fb) {
    Serial.println("Camera capture failed");
    return;
  }
       EEPROM.begin(EEPROM_SIZE); //initialize prom size
       pictureNumber = EEPROM.read(0) + 1; //save the number of pictures taken in eeprom
  //SAVE IMAGE TO SPIFFS FOR USE LATER ON SCREEN AND TRANSFER
  
      Serial.println("Storing image to spiffs only");
     SPIFFS.remove(spiffsFilename);                         // if file name already exists delete it
     File file = SPIFFS.open(spiffsFilename, FILE_WRITE);   // create new file
     if (!file) {
        Serial.println("Failed to create file in Spiffs - will format and try again");
       if (!SPIFFS.format()) {                              // format spiffs
          Serial.println("Spiffs format failed");
       } else {
         file = SPIFFS.open(spiffsFilename, FILE_WRITE);    // try again to create new file
         if (!file) {
            Serial.println("Still unable to create file in spiffs");
         }
       }
     }
 /////SAVING TO SPIFFS IF ALL IS OKAY////// PATH = spiffsFilename
       if (file) {       // if file has been created ok write image data to it
           if (file.write(fb->buf, fb->len)) {
             sRes = 1;    // flag as saved ok
           } else {
                 Serial.println("Error: failed to write image data to spiffs file");
         }
     }

       Serial.print("The picture has been saved to Spiffs as " + spiffsFilename);
       Serial.print(" - Size: ");
       Serial.print(file.size());
       Serial.println(" bytes");

       file.close();

        esp_camera_fb_return(fb);   //release buffer memory
        
}
