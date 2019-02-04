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
#include <RTClib.h>

const int delaytime = 250;
const uint16_t NUM_LEDS = 256;
const int8_t MAT_PIN = 6;
uint8_t canvas[NUM_LEDS];

const uint8_t num_bright_levels = 5;
const uint8_t num_color_combinations = 3;

uint8_t cur_min = 0;
uint8_t cur_hour = 0;
uint8_t cur_day = 1;
uint8_t cur_month = 0;
uint8_t cur_bri = 2;



const int pin_mode = 6, pin_plus = 7, pin_minus = 8;

mode_t mode = word_disp;

Adafruit_NeoPixel matrix;
RTC_DS3231 rtc;

int condExecAndWait(const int pinnum, void (*f)(void), int delaytime) {
  if (checkInput(pinnum)) {
    f();
    delay(delaytime);
    return 1;
  }
  return 0;
}

void changeMode(void) {
  mode = (mode+1)%8;
}

void plus(void) {
  DateTime current = rtc.now();

  uint8_t nowmin = current.minute();
  uint8_t nowhour = current.hour();
  uint8_t nowday = current.day();
  uint8_t nowmonth = current.month();
  switch (mode) {
    case dig_disp: // display hour, min, day, month
      break;
    case dig_min: // increment minute
      nowmin = (nowmin+1)%60;
      break;
    case dig_hour: // display ho
      nowhour = (nowhour+1)%24;
      break;
    case dig_day: // display di
      switch (cur_month) {
        case 1: // february
          nowday = (nowday+1)%29;
          break;
         case 3:
         case 5:
         case 8:
         case 10:
          nowday = (nowday+1)%31;
          break;
        default:
          nowday = (nowday+1)%32;
          break;
      }
      if (nowday == 0) {
        nowday = 1;
      }
      break;
    case dig_month: // display me top left, day and month
      nowmonth = (nowmonth+1)%12;
      break;
    case dig_color: // display co top left
      cur_color = (cur_color+1)%num_color_combinations;
      break;
    case dig_bright: // display br top left
      cur_bright = (cur_bright+1)%num_bright_levels;
      break;
    default: // word_disp
      break;
  }
  // TODO - set new time
}

void minus(void) {
  switch (mode) {
    case dig_disp: // display hour, min, day, month
      break;
    case dig_min: // display mi
      break;
    case dig_hour: // display ho
      break;
    case dig_day: // display di
      break;
    case dig_month: // display me top left, day and month
      break;
    case dig_color: // display co top left
      break;
    case dig_bright: // display br top left
      break;
    default: // word_disp
      break;
  }
}

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

void paint_canvas_dig(uint8_t base_row, uint8_t base_col, uint8_t data_width, const uint8_t[] data, paint_t pa) {
  // iterate through data
  for (uint8_t data_row = 0; data_row < 8; ++data_row) {
    for (uint8_t data_col = 0; data_col < data_width; ++data_col) {
      if (bitRead(data[row], data_width - data_col - 1)) {
        canvas[matrix_decode(base_row + data_row, base_col + data_col)] = pa;
      }
    }
  }
}

// animate - specify the layout of the canvas, switching on mode

void animate(void) {
  switch (mode) {
    case dig_disp: // display hour, min, day, month
      paint_canvas_dig(0, 0, 4, dig_maps[cur_hour/10], time_t); // paint tens of hour
      paint_canvas_dig(0, 4, 4, dig_maps[cur_hour%10], time_t); // paint ones of hour
      paint_canvas_dig(0, 8, 4, dig_maps[cur_min/10], time_t); // paint tens of min
      paint_canvas_dig(0, 12, 4, dig_maps[cur_min%10], time_t); // paint ones of min
      paint_canvas_dig(8, 0, 4, dig_maps[cur_day/10], date_t); // paint tens of day
      paint_canvas_dig(8, 4, 4, dig_maps[cur_day%10], date_t); // paint ones of day
      paint_canvas_dig(8, 8, 8, month_maps[cur_month], date_t); // paint month
      break;
    case dig_min: // display mi
      paint_canvas_dig(0, 0, 8, mi, time_t);
      paint_canvas_dig(0, 8, 4, dig_maps[cur_min/10], time_t); // paint tens of min
      paint_canvas_dig(0, 12, 4, dig_maps[cur_min%10], time_t); // paint ones of min
      break;
    case dig_hour: // display ho
      paint_canvas_dig(0, 0, 8, ho, time_t);
      paint_canvas_dig(0, 8, 4, dig_maps[cur_hour/10], time_t); // paint tens of hour
      paint_canvas_dig(0, 12, 4, dig_maps[cur_hour%10], time_t); // paint ones of hour
      break;
    case dig_day: // display di
      paint_canvas_dig(0, 0, 8, di, date_t);
      paint_canvas_dig(0, 8, 4, dig_maps[cur_day/10], date_t); // paint tens of day
      paint_canvas_dig(0, 12, 4, dig_maps[cur_day%10], date_t); // paint ones of day
      break;
    case dig_month: // display me top left, day and month
      paint_canvas_dig(0, 0, 8, me, date_t);
      paint_canvas_dig(0, 8, 4, dig_maps[cur_month/10], date_t); // paint tens of day
      paint_canvas_dig(0, 12, 4, dig_maps[cur_month%10], date_t); // paint ones of day
      break;
    case dig_color: // display co top left
      paint_canvas_dig(0, 0, 8, co, time_t);
      paint_canvas_dig(0, 8, 8, co, date_t);
      break;
    case dig_bright: // display br top left
      paint_canvas_dig(0, 0, 8, br, spec_t);
      paint_canvas_dig(0, 12, 4, dig_maps[cur_bri], spec_t);
      break;
    default: // word_disp
      display_word();
      break;
  }
}

void display_word(void) {
  // paint canvas with
}

void setup() {
  // set up matrix with 256 LEDS on pin 6; GRB layout and 800kHz
  matrix = Adafruit_NeoPixel(NUM_LEDS, MAT_PIN, NEO_GRB + NEO_KHZ800);
  pinMode(
  
}

void loop() {
  // check if button is pressed
    
  // update all the time variables
  

}
