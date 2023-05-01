#define TFT_CS 9
#define TFT_DC 10
#define SD_CHIP_SELECT 5

#include "/Users/dda/Kongduino/Arduino/libraries/SdFatAdafruit/src/SdFat.h"
#include "/Users/dda/Kongduino/Arduino/libraries/SdFatAdafruit/src/sdios.h"
// Too many SdFat.h libraries, not all compatible with each other!
// SdFatAdafruit works for me, so that's the one I am using here...
// Change these includes to match the path to SdFatAdafruit on your computer,
// or remove all other instances of SdFat and you can just do:
// #include "SdFat.h"
// #include "sdios.h"

#include <Adafruit_GFX.h>
#include <SPI.h>
#include "LoRa_Helper.h"
#include <Adafruit_ILI9341.h>
#include "Fonts.h"
#include "TwoB.h"

Adafruit_ILI9341 tft(&SPI1, TFT_DC, TFT_CS, -1);
SdFat sd(&SPI1);

void setup() {
  Serial.begin(115200);
  Serial.flush();
  delay(3000);
  Serial.println("\n\nFeatherWing Hello World...");
  if (!sd.begin(SD_CHIP_SELECT, SD_SCK_MHZ(100))) {
    SerialUSB.println("Ohhh crap. SD card init failed!");
    sd.initErrorHalt();
  }
  tft.begin();
  tft.setRotation(1);
  loraSetup();
}

void loop() {
  displayImage("/SZB.2B", 0, 0, tft);
  delay(2000);
  displayImage("/BEACH.2B", 0, 0, tft);
  uint16_t buff[46 * 46]; // Cheating here, because I know CALM.2B is 46 x 46
  // Ideally I should add a getSize() function that reads width and height into pWidth/pHeight
  // This way I could then dimension the buffer appropriately
  if (!preloadImage("/CALM.2B", buff)) {
    SerialUSB.println("Could not preload CALM.2B!");
    while (1);
  }
  // preloadImage() and displayImage(char *fName, uint16_t XX, uint16_t YY, Adafruit_ILI9341 tft) save the widht and height in pWidth/pHeight
  // displayImage(uint16_t *colors, uint16_t zWidth, uint16_t zHeight, uint16_t XX, uint16_t YY, Adafruit_ILI9341 tft) requires width and height
  // and does not modify pWidth/pHeight
  uint16_t calmWidth, calmHeight;
  calmWidth = pWidth;
  calmHeight = pHeight;
  displayImage(buff, calmWidth, calmHeight, 274, 194, tft);
  delay(2000);
  displayImage("/GREECE1.2B", 0, 0, tft);
  displayImage(buff, calmWidth, calmHeight, ALIGN_RIGHT, ALIGN_BOTTOM, tft);
  delay(2000);
  displayImage("/GREECE2.2B", 0, 0, tft);
  displayImage(buff, calmWidth, calmHeight, ALIGN_RIGHT, ALIGN_BOTTOM, tft);
  delay(2000);
  displayImage("/GREECE3.2B", 0, 0, tft);
  displayImage(buff, calmWidth, calmHeight, ALIGN_RIGHT, ALIGN_BOTTOM, tft);
  delay(2000);
  displayImage("/GREECE4.2B", 0, 0, tft);
  displayImage(buff, calmWidth, calmHeight, ALIGN_RIGHT, ALIGN_BOTTOM, tft);
  delay(2000);
  tft.fillScreen(ILI9341_WHITE);
  displayImage("/MARSUP.2B", 0, ALIGN_MIDDLE, tft);
  displayImage(buff, calmWidth, calmHeight, ALIGN_CENTER, ALIGN_MIDDLE, tft);
  delay(2000);
  tft.fillScreen(ILI9341_WHITE);
  displayImage("/TRUCK.2B", 0, ALIGN_MIDDLE, tft);
  for (uint16_t px = 0; px < 320; px += calmWidth) displayImage(buff, calmWidth, calmHeight, px, ALIGN_BOTTOM, tft);
  delay(2500);

  tft.fillScreen(ILI9341_WHITE);
  uint16_t maxWidth = 320 - calmWidth, maxHeight = 240 - calmHeight, px , py;
  for (uint16_t ix = 0; ix < 64; ix++) {
    px = (randomStock[randomIndex++] + randomStock[randomIndex++] * 256) % maxWidth;
    py = (randomStock[randomIndex++] + randomStock[randomIndex++] * 256) % maxHeight;
    displayImage(buff, calmWidth, calmHeight, px, py, tft);
  }
  stockUpRandom(); // we have used up the whole buffer:
  // 64 times x 2 bytes px x 2 bytes py = 256 bytes
  delay(2500);

  tft.fillScreen(ILI9341_BLACK);
  displayImage("/SZB2.2B", 0, 0, tft);
  tft.setTextColor(ILI9341_WHITE);
  tft.setFont(&FreeMonoBold12pt7b);
  tft.setCursor(10, 235);
  tft.print("Hello #CALM!");
  delay(2000);
}
