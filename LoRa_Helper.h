#include <LoRa.h> // https://github.com/sandeepmistry/arduino-LoRa
#include "LoRandom.h" // https://github.com/Kongduino/LoRandom

void stockUpRandom();
unsigned char randomStock[256];
uint8_t randomIndex = 0;

/*
  Note!

  In ~Arduino/libraries/arduino-LoRa/src/LoRa.h, move:

    uint8_t readRegister(uint8_t address);
    void writeRegister(uint8_t address, uint8_t value);

  from `private:` to `public:` or the code won't compile.
  LoRandom needs access to these 2 functions.

*/
void writeRegister(uint8_t reg, uint8_t value) {
  LoRa.writeRegister(reg, value);
}

uint8_t readRegister(uint8_t reg) {
  return LoRa.readRegister(reg);
}

#define REG_OCP 0x0B
#define REG_PA_CONFIG 0x09
#define REG_LNA 0x0c
#define REG_OP_MODE 0x01
#define REG_MODEM_CONFIG_1 0x1d
#define REG_MODEM_CONFIG_2 0x1e
#define REG_MODEM_CONFIG_3 0x26
#define REG_PA_DAC 0x4D
#define PA_DAC_HIGH 0x87
#define RFM_TCXO (40u)
#define RFM_SWITCH (41u)

void loraSetup() {
  SerialUSB.println(" . LoRa begin");
  pinMode(RFM_TCXO, OUTPUT);
  pinMode(RFM_SWITCH, OUTPUT);
  LoRa.setPins(SS, RFM_RST, RFM_DIO0);
  if (!LoRa.begin(868e6)) {
    SerialUSB.println("Starting LoRa failed!\nNow that's disappointing...");
    while (1);
  }

  // build an initial array of 256 bytes of random data
  // this will be replenished automatically when using:
  // uint8_t getRandomByte();
  // uint16_t getRamdom16();
  // void getRandomBytes(uint8_t *, uint8_t);
  stockUpRandom();
}

void stockUpRandom() {
  fillRandom(randomStock, 256);
  randomIndex = 0;
}
