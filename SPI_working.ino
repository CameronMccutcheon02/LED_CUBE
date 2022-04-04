#include <SPI.h>
#include <digitalWriteFast.h>
#define LATCH_PIN 10
#define LEDS_PER_LEVEL 64
uint8_t cube0[8][8][3];
uint8_t cube1[8][8][3];
uint8_t cube2[8][8][3];
uint8_t cube3[8][8][3];

byte red0[LEDS_PER_LEVEL], red1[LEDS_PER_LEVEL], red2[LEDS_PER_LEVEL], red3[LEDS_PER_LEVEL];
byte blue0[LEDS_PER_LEVEL], blue1[LEDS_PER_LEVEL], blue2[LEDS_PER_LEVEL], blue3[LEDS_PER_LEVEL];
byte green0[LEDS_PER_LEVEL], green1[LEDS_PER_LEVEL], green2[LEDS_PER_LEVEL], green3[LEDS_PER_LEVEL];


int BAM_BIT = 0, BAM_Counter = 0;
uint8_t anode_level = 0;

void setup() {
  clearCube();
  pinMode(LATCH_PIN, OUTPUT); //Pin 10 is Storage Clock pin

  SPI.setBitOrder(MSBFIRST);//Most Significant Bit First
  SPI.setDataMode(SPI_MODE0);// Mode 0 Rising edge of data, keep clock low
  SPI.setClockDivider(SPI_CLOCK_DIV2);//Run the data in at 16MHz/2 - 8MHz
  SPI.begin();
  Serial.begin(9600);

  //Timer Setup ********
  TCCR1A = B00000000;//Register A all 0's since we're not toggling any pins
  TCCR1B = B00001011;//bit 3 set to place in CTC mode, will call an interrupt on a counter match
  //bits 0 and 1 are set to divide the clock by 64, so 16MHz/64=250kHz
  TIMSK1 = B00000010;//bit 1 set to call the interrupt on an OCR1A match
  OCR1A = 30;
  interrupts();               //Enable interrupt
  //TIMER SETUP END

  //setFirstLayer();
}

void loop () {
  int mode = 0;
  switch (mode) {
    case 1: setFirstLayer(); break;
    case 0: setColor(4, 1, 3); break;
    default: colorchange(); break;
  }
}

ISR(TIMER1_COMPA_vect) {
  uint8_t k;
  int i;
  SPI.transfer(0x01 << anode_level);
  int start_byte = anode_level * 8;
  int max_byte = (anode_level + 1) * 8;
  switch (BAM_BIT) {
    case 0:
      for (i = 2; i > -1; i--) {
        for (k = 0; k < 8; k++) {
          SPI.transfer(cube0[anode_level][k][i]);
        }
      }
      break;

    case 1:
      for (i = 2; i > -1; i--) {
        for (k = 0; k < 8; k++) {
          SPI.transfer(cube1[anode_level][k][i]);
        }
      }
      break;

    case 2:
      for (i = 2; i > -1; i--) {
        for (k = 0; k < 8; k++) {
          SPI.transfer(cube2[anode_level][k][i]);
        }
      }
      break;

    case 3:
      for (i = 2; i > -1; i--) {
        for (k = 0; k < 8; k++) {
          SPI.transfer(cube3[anode_level][k][i]);
        }
      }
      break;
  }
  digitalWriteFast(LATCH_PIN, HIGH);

  BAM_BIT++;
  if (BAM_BIT == 4) {
    BAM_BIT = 0;
    anode_level++;
  }
  if (anode_level == 8) {
    anode_level = 0;
  }
  digitalWriteFast(LATCH_PIN, LOW); //Pull our latch pin high to store the data
}

void colorchange() {
  int i, j, k;
  int del = 250;
  //for (int i = 0; i < 5; i++) {
  for (j = 0; j < 5; j++) {
      setColor(0, j, 0);
      delay(del);
  }
  for (j = 0; j < 5; j++) {
      setColor(0, 0, j);
      delay(del);
  }
  for (i = 4; i >= 0; i--) {
      setColor(0,4,i);
      delay(del);
  }
  for (i = 0; i < 5; i++) {
    setColor(i,4,0);
    delay(del);
  }
  for (i = 4; i >= 0; i--) {
    setColor(4,i,0);
    delay(del);
  }
  for (i = 0; i < 5; i++) {
    setColor(4,0,i);
    delay(del);
  }
  for (i = 4; i >= 0; i--) {
    setColor(i,0,4);
    delay(del);
  }
  //delay(500);
  clearCube();
}


void clearCube() {
  for (uint8_t k = 0; k < 8; k++) {
    for (uint8_t j = 0; j < 8; j++) {
      for (uint8_t i = 0; i < 3; i++) {
        cube0[k][j][i] = 0x00;
        cube1[k][j][i] = 0x00;
        cube2[k][j][i] = 0x00;
        cube3[k][j][i] = 0x00;
      }
    }
  }
}

void setFirstLayer() {
  for (uint8_t k = 0; k < 1; k++) {
    for (uint8_t j = 0; j < 8; j++) {
      for (uint8_t i = 0; i < 3; i++) {
        cube0[k][j][i] = 0xFF;
        cube1[k][j][i] = 0xFF;
        cube2[k][j][i] = 0xFF;
        cube3[k][j][i] = 0xFF;
      }
    }
  }
}

void setColor(int red, int green, int blue) {
  red = constrain(red, 0, 4);
  blue = constrain(blue, 0, 4);
  green = constrain(green, 0, 4);
  for (uint8_t k = 0; k < 8; k++) {
    for (uint8_t j = 0; j < 8; j++) {
      switch (red) {
        case 4: cube0[k][j][0] = 0xFF;
        case 3: cube1[k][j][0] = 0xFF;
        case 2: cube2[k][j][0] = 0xFF;
        case 1: cube3[k][j][0] = 0xFF;
      }
      switch (green) {
        case 4: cube0[k][j][1] = 0xFF;
        case 3: cube1[k][j][1] = 0xFF;
        case 2: cube2[k][j][1] = 0xFF;
        case 1: cube3[k][j][1] = 0xFF;
      }
      switch (blue) {
        case 4: cube0[k][j][2] = 0xFF;
        case 3: cube1[k][j][2] = 0xFF;
        case 2: cube2[k][j][2] = 0xFF;
        case 1: cube3[k][j][2] = 0xFF;
      }
    }
  }
}

void setGreen() {
  for (uint8_t j = 0; j < 8; j++) {
    red0[j] = 0x00; red1[j] = 0x00; red2[j] = 0x00; red3[j] = 0x00;
    blue0[j] = 0x00; blue1[j] = 0x00; blue2[j] = 0x00; blue3[j] = 0x00;
    green0[j] = 0xFF; green1[j] = 0xFF; green2[j] = 0xFF; green3[j] = 0xFF;
  }
}
void setRed() {
  for (uint8_t j = 0; j < 8; j++) {
    red0[j] = 0xFF; red1[j] = 0xFF; red2[j] = 0xFF; red3[j] = 0xFF;
    blue0[j] = 0x00; blue1[j] = 0x00; blue2[j] = 0x00; blue3[j] = 0x00;
    green0[j] = 0x00; green1[j] = 0x00; green2[j] = 0x00; green3[j] = 0x00;
  }
}
void setBlue() {
  for (uint8_t j = 0; j < 8; j++) {
    red0[j] = 0x00; red1[j] = 0x00; red2[j] = 0x00; red3[j] = 0x00;
    blue0[j] = 0xFF; blue1[j] = 0xFF; blue2[j] = 0xFF; blue3[j] = 0xFF;
    green0[j] = 0x00; green1[j] = 0x00; green2[j] = 0x00; green3[j] = 0x00;
  }
}
