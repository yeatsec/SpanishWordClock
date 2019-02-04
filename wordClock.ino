/*
 * Eric Yeats
 * Word Clock Birthday Gift for Audrianna
 * February 5, 2019
 * 
 * 3D Printed word clock face (Spanish) 'powered' by an individually adressable RGB LED Matrix
 * Time- Color- Displaymode- Adjustable with Buttons, Blackout-Persistent
 * 
 */

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>


const uint16_t NUM_LEDS = 256;
const int8_t MAT_PIN = 6;

mode_t mode = word_disp;

uint8_t mode = 0; // 

Adafruit_NeoPixel matrix;

// converts row, col format to LED strip format  with 0 based indexing of row and col
// this function depends on how the LED Matrix is oriented
// @param row [0, 15]
// @param col [0, 15]
// @return decoded index of led of interest [0, 255]
uint8_t matrix_decode(uint8_t row, uint8_t col) {
  if (col & 0x01) // odd col
  {
    return col*16+row;
  } else { // even col
    return (col+1)*16-row;
  }
}

// animate - specify the layout of the canvas, switching on mode

void animate(void) {
  switch (mode) {
    
  }
}

void setup() {
  // set up matrix with 256 LEDS on pin 6; GRB layout and 800kHz
  matrix = Adafruit_NeoPixel(NUM_LEDS, MAT_PIN, NEO_GRB + NEO_KHZ800);

  
}

void loop() {
  

}
