#define ALIGN_LEFT 0
#define ALIGN_TOP 0
#define ALIGN_RIGHT 65535
#define ALIGN_BOTTOM 65534
#define ALIGN_CENTER 65533 // Left <--> Right
#define ALIGN_MIDDLE 65532 // Top <--> Bottom

uint16_t pWidth, pHeight;
// These two variables are global, so you can pick up the values, should you need them, after loading the image.

void displayImage(uint16_t *colors, uint16_t zWidth, uint16_t zHeight, uint16_t XX, uint16_t YY, Adafruit_ILI9341 tft) {
  SerialUSB.print("Image size: ");
  SerialUSB.print(zWidth);
  SerialUSB.print(" x ");
  SerialUSB.println(zHeight);
  uint16_t pXX = XX, pYY = YY;
  if (XX == ALIGN_RIGHT) {
    pXX = 320 - zWidth;
  } else if (XX == ALIGN_CENTER) {
    pXX = (320 - zWidth) / 2;
  }
  if (YY == ALIGN_BOTTOM) {
    pYY = 240 - zHeight;
  } else if (YY == ALIGN_MIDDLE) {
    pYY = (240 - zHeight) / 2;
  }

  uint16_t px, py, ix, color;
  ix = 0;
  for (py = 0; py < zHeight; py++) {
    tft.startWrite();
    tft.setAddrWindow(pXX, py + pYY, zWidth, 1);
    for (px = 0; px < zWidth; px++) {
      color = colors[ix++];
      // SPI1.transfer((uint8_t)(color >> 8));
      // SPI1.transfer((uint8_t)(color & 0xFF));
      tft.spiWrite((uint8_t)(color >> 8));
      tft.spiWrite((uint8_t)(color & 0xFF));
    }
    tft.endWrite();
  }
}

bool preloadImage(char *fName, uint16_t *pColors) {
  SdFile binFile(fName, O_RDONLY);
  // check for open error
  if (!binFile.isOpen()) {
    SerialUSB.print("Couldn't open file ");
    SerialUSB.println(fName);
    return false;
  }
  char header[6];
  if (binFile.read(&header, 6) != 6) {
    SerialUSB.println("Couldn't read header from file.");
    binFile.close();
    return false;
  }
  if (header[0] != '2' || header[1] != 'B') {
    SerialUSB.println("Wrong header!");
    binFile.close();
    return false;
  }
  pWidth = header[2] + header[3] * 256;
  pHeight = header[4] + header[5] * 256;
  SerialUSB.print("Image size: ");
  SerialUSB.print(pWidth);
  SerialUSB.print(" x ");
  SerialUSB.println(pHeight);
  uint16_t px, py, nBytes, ix, nx = 0;
  nBytes = pWidth * 2;
  char mb[nBytes];
  for (py = 0; py < pHeight; py++) {
    if (binFile.read(&mb, nBytes) != nBytes) {
      SerialUSB.println("Failed reading from file!");
      binFile.close();
      return false;
    }
    ix = 0;
    for (px = 0; px < pWidth; px++) {
      pColors[nx++] = mb[ix + 1] * 256 + mb[ix];
      ix += 2;
    }
  }
  binFile.close();
  return true;
}

void displayImage(char *fName, uint16_t XX, uint16_t YY, Adafruit_ILI9341 tft) {
  char mb[640];
  SdFile binFile(fName, O_RDONLY);
  // check for open error
  if (!binFile.isOpen()) {
    SerialUSB.print("Couldn't open file ");
    SerialUSB.println(fName);
    return;
  }
  if (binFile.read(&mb, 6) != 6) {
    SerialUSB.println("Couldn't read header from file.");
    binFile.close();
    return;
  }
  if (mb[0] != '2' || mb[1] != 'B') {
    SerialUSB.println("Wrong header!");
    binFile.close();
    return;
  }
  pWidth = mb[2] + mb[3] * 256;
  pHeight = mb[4] + mb[5] * 256;
  SerialUSB.print("Image size: ");
  SerialUSB.print(pWidth);
  SerialUSB.print(" x ");
  SerialUSB.println(pHeight);
  uint16_t pXX = XX, pYY = YY;
  if (XX == ALIGN_RIGHT) {
    pXX = 320 - pWidth;
  } else if (XX == ALIGN_CENTER) {
    pXX = (320 - pWidth) / 2;
  }
  if (YY == ALIGN_BOTTOM) {
    pYY = 240 - pHeight;
  } else if (YY == ALIGN_MIDDLE) {
    pYY = (240 - pHeight) / 2;
  }

  uint16_t px, py, nBytes, ix;
  nBytes = pWidth * 2;
  for (py = 0; py < pHeight; py++) {
    if (binFile.read(&mb, nBytes) != nBytes) {
      SerialUSB.println("Failed reading from file!");
      binFile.close();
      return;
    }
    ix = 0;
    tft.startWrite();
    tft.setAddrWindow(pXX, py + pYY, pWidth, 1);
    for (px = 0; px < pWidth; px++) {
      // SPI1.transfer(mb[ix + 1]);
      // SPI1.transfer(mb[ix]);
      tft.spiWrite(mb[ix + 1]);
      tft.spiWrite(mb[ix]);
      ix += 2;
    }
    tft.endWrite();
  }
  binFile.close();
}

#define ILI9341_RAMREAD 0x2E // Memory Read
#define ILI9341_NOP 0
void readRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t *pColors, Adafruit_ILI9341 tft) {
  // Does not work...
  uint16_t ix = 0;
  for (uint16_t i = 0; i < h; i++) {
    for (uint16_t n = 0; n < w; n++) {
      tft.startWrite();
      tft.setAddrWindow(x + n, y + i, 1, 1);
      tft.writeCommand(ILI9341_RAMREAD); // read from RAM
      uint8_t red = tft.spiRead(); // dummy read
      red = tft.spiRead();
      uint8_t green = tft.spiRead();
      uint8_t blue = tft.spiRead();
      pColors[ix++] = tft.color565(red, green, blue);
      tft.endWrite();
    }
  }
}
