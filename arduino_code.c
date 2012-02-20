#include <pins_arduino.h>

#define CMNDR_START_BYTE 0x01
#define CMNDR_END_BYTE 0x02

byte serInStr[32]; // array that will hold the serial input string

int brightness = 20;
int onOff = 1;

volatile uint8_t *out;

uint8_t clockBit;
uint8_t dataBit;
uint8_t displayBit;

// 320 bytes storage for the display
uint8_t data[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

void setup() // run once, when the sketch starts
{
  Serial.begin(9600);

  serInStr[0] = 0;
  serInStr[1] = 0;
  serInStr[2] = 0;
  serInStr[3] = 0;

  pinMode(2, OUTPUT);
  digitalWrite(2, LOW);
  pinMode(3, OUTPUT);
  digitalWrite(3, LOW);
  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);

  out = portOutputRegister(digitalPinToPort(2));
  clockBit = digitalPinToBitMask(2);
  dataBit = digitalPinToBitMask(3);
  displayBit = digitalPinToBitMask(4);

}

void loop() {

  for (int j = 1; j < 8; j++) {

    *out |= displayBit;

    if (readCommand(serInStr) > 0) {
      if (serInStr[0] == CMNDR_START_BYTE) {
        int address = serInStr[1] * 256 + serInStr[2];
        int length = serInStr[3];
        if (address > -1 && address < 320) {
          for (int k = 0; k < length; k++) {
            data[address + k] = serInStr[4 + k];
          }
        }

        if (address == 321)
          brightness = serInStr[4];
        if (address == 322)
          onOff = serInStr[4];
      }
    }

    if (1 == onOff) {

      // write memory to serial
      for (int i = 0; i < 320; i++) {
        shiftBit(!(data[i] & (1 << (j - 1))));
      }

      // write appropriate row bit
      shiftBit(j & 0x04);
      shiftBit(j & 0x02);
      shiftBit(j & 0x01);

      // start displaying
      *out &= ~displayBit;
      delayMicroseconds(brightness);
    }
  }

}

void shiftBit(int val) {
  if (val == LOW)
    *out &= ~dataBit;
  else
    *out |= dataBit;

  *out |= clockBit;
  *out &= ~clockBit;
}

uint8_t readCommand(byte *str) {
  if (!Serial.available())
    return 0;

  delay(20); // wait a little for serial data

  int c = Serial.read(); // get start byte (hopefully)
  if (c != CMNDR_START_BYTE)
    return 0;
  str[0] = c;

  int i = 1;
  while (Serial.available()) {
    str[i] = Serial.read();
    i++;
    delay(1);
  }
  str[i] = 0;
  return i;
}
