#include <SPI.h>
#include <digitalWriteFast.h>
#define LATCH_PIN 10
#define LEDS_PER_LEVEL 64
uint8_t cube0[8][24]; 
uint8_t cube1[8][24];
uint8_t cube2[8][24];
uint8_t cube3[8][24];

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
  OCR1A=30;
  interrupts();               //Enable interrupt
  //TIMER SETUP END
  
  setFirstLayer(3);
}

void loop () {
  int mode;
  switch(mode) {
    case 1: setFirstLayer(3); break;
    default: colorchange(); break;
  }
}

ISR(TIMER1_COMPA_vect){
  uint8_t k;
  SPI.transfer(0x01<<anode_level);
  int start_byte = anode_level * 8;
  int max_byte = (anode_level + 1) * 8;
  switch(BAM_BIT) {
    case 0: 
       for (k = start_byte; k < (max_byte); k++) {
        SPI.transfer(blue0[k]);
       }
       for (k = start_byte; k < (max_byte); k++) {
        SPI.transfer(green0[k]);
       }
       for (k = start_byte; k < (max_byte); k++) {
        SPI.transfer(red0[k]);
       }
      break;
      
     case 1: 
       for (k = start_byte; k < (max_byte); k++) {
        SPI.transfer(blue1[k]);
       }
       for (k = start_byte; k < (max_byte); k++) {
        SPI.transfer(green1[k]);
       }
       for (k = start_byte; k < (max_byte); k++) {
        SPI.transfer(red1[k]);
       }
      break;
      
    case 2: 
       for (k = start_byte; k < (max_byte); k++) {
        SPI.transfer(blue2[k]);
       }
       for (k = start_byte; k < (max_byte); k++) {
        SPI.transfer(green2[k]);
       }
       for (k = start_byte; k < (max_byte); k++) {
        SPI.transfer(red2[k]);
       }
      break;
      
     case 3: 
       for (k = start_byte; k < (max_byte); k++) {
        SPI.transfer(blue3[k]);
       }
       for (k = start_byte; k < (max_byte); k++) {
        SPI.transfer(green3[k]);
       }
       for (k = start_byte; k < (max_byte); k++) {
        SPI.transfer(red3[k]);
       }
      break;     
  }
  digitalWriteFast(LATCH_PIN, HIGH);

  BAM_BIT++;
  if (BAM_BIT == 4) {
    BAM_BIT = 0;
    anode_level++;
  }
  if (anode_level == 8) {anode_level = 0;} 
  digitalWriteFast(LATCH_PIN, LOW); //Pull our latch pin high to store the data
}

void colorchange() {
  for (int i = 0; i < 3; i++) {
      switch(i) {
        case 0: setBlue(); break;
        case 1: setGreen(); break;
        case 2: setRed(); break;
      }
      delay(500);
      clearCube();
    }
}

void clearCube() {
      for (uint8_t j = 0; j < 64; j++) {
        red0[j] = 0x00; red1[j] = 0x00; red2[j] = 0x00; red3[j] = 0x00;
        blue0[j] = 0x00; blue1[j] = 0x00; blue2[j] = 0x00; blue3[j] = 0x00;
        green0[j] = 0x00; green1[j] = 0x00; green2[j] = 0x00; green3[j] = 0x00;
      }
}
void setFirstLayer(int brightness) {
      for (uint8_t j = 0; j < 8; j++) {
        red0[j] = 0xFF; red1[j] = 0xFF; red2[j] = 0xFF; red3[j] = 0xFF;
        blue0[j] = 0xFF; blue1[j] = 0xFF; blue2[j] = 0xFF; blue3[j] = 0xFF;
        green0[j] = 0xFF; green1[j] = 0xFF; green2[j] = 0xFF; green3[j] = 0xFF;
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
