#include <SPI.h>
#include <digitalWriteFast.h>
#define LATCH_PIN 10
#define LEDS_PER_LEVEL 64
#define LED_RESOLUTION 8
uint8_t cube[LED_RESOLUTION][8][8][3]; //LED_Resolution is color control //First 8 is the level of the cube we are currently on //Second 8 is the rows //3 is the colors


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
  int mode = 1;
  switch (mode) {
    case 0: setFirstLayer(); break;
    case 1: setColor(4, 0, 5); break;
    default: colorchange(); break;
  }
}

ISR(TIMER1_COMPA_vect) {
  uint8_t k;
  int i;
  SPI.transfer(0x01 << anode_level);

  for (i = 2; i > -1; i--) {
    for (k = 0; k < 8; k++) {
      SPI.transfer(cube[BAM_BIT][anode_level][k][i]);
    }
  }
  digitalWriteFast(LATCH_PIN, HIGH);
  BAM_BIT++;
  if (BAM_BIT == LED_RESOLUTION) {
    BAM_BIT = 0;
    anode_level++;
  }
  if (anode_level == 8) {
    anode_level = 0;
  }
  digitalWriteFast(LATCH_PIN, LOW); //Pull our latch pin high to store the data
}

void colorchange() {
}


void clearCube() {
  for (int l = 0; l < LED_RESOLUTION; l++) {
    for (uint8_t k = 0; k < 8; k++) {
      for (uint8_t j = 0; j < 8; j++) {
        for (uint8_t i = 0; i < 3; i++) {
          cube[l][k][j][i] = 0x00;
        }
      }
    }
  }
}

void setFirstLayer() {
  for (int l = 0; l < LED_RESOLUTION; l++) {
    for (uint8_t k = 0; k < 1; k++) {
      for (uint8_t j = 0; j < 8; j++) {
        for (uint8_t i = 0; i < 3; i++) {
          cube[l][k][j][i] = 0xFF;
        }
      }
    }
  }
}

void setColor(int red, int green, int blue) {
  red = constrain(red, 0, LED_RESOLUTION);
  blue = constrain(blue, 0, LED_RESOLUTION);
  green = constrain(green, 0, LED_RESOLUTION);
  for (int l = 0; l < red; l++) {
    for (uint8_t k = 0; k < 8; k++) {
      for (uint8_t j = 0; j < 8; j++) {
        cube[l][k][j][0] = 0xFF;
      }
    }
  }
  for (int l = 0; l < green; l++) {
    for (uint8_t k = 0; k < 8; k++) {
      for (uint8_t j = 0; j < 8; j++) {
        cube[l][k][j][1] = 0xFF;
      }
    }
  }
  for (int l = 0; l < blue; l++) {
    for (uint8_t k = 0; k < 8; k++) {
      for (uint8_t j = 0; j < 8; j++) {
        cube[l][k][j][2] = 0xFF;
      }
    }
  }
  
}
