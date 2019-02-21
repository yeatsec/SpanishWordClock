/*
 * Eric Yeats
 * Word Clock Birthday Gift for Audrianna
 * February 5, 2019
 * 
 * 3D Printed word clock face (Spanish) 'powered' by an individually adressable RGB LED Matrix
 * Time- Color- Displaymode- Adjustable with Buttons, Blackout-Persistent
 * 
 * SOFTWARE TODO
 * 
 * 
 */

#include <Arduino.h>
#include <FastLED.h>
#include <RTClib.h>
#include "resources.h"

const int delaytime = 250;
const uint16_t NUM_LEDS = 256;
const int8_t MAT_PIN = 6;

CRGB leds[NUM_LEDS];

const uint8_t num_bright_levels = 10;
const uint8_t num_color_combinations = 2;

uint8_t cur_sec = 0;
uint8_t cur_min = 0;
uint8_t cur_hour = 0;
uint8_t cur_day = 0;
uint8_t cur_month = 0;
uint8_t cur_bri = 1;
uint8_t cur_color = 0;

enum paint_t {
  none_t,
  time_t,
  date_t,
  both_t,
  spec_t
};

enum mode_t {
  word_disp,
  dig_disp,
  dig_color,
  dig_bright,
  dig_min,
  dig_hour,
  dig_day,
  dig_month
};

const uint8_t days_in_months[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

const int pin_mode = 7, pin_plus = 8, pin_minus = 9;

enum mode_t mode = word_disp;
const uint8_t birth_day = 4, birth_month = 1; // February 5

RTC_DS3231 rtc;
const uint16_t default_year = 1997;

int checkInput(const int pinnum) {
  return digitalRead(pinnum) == LOW ? 1 : 0; // if pulled down, button input recvd
}

int condExecAndWait(const int pinnum, void (*f)(void), int delaytime) {
  if (checkInput(pinnum)) {
    f();
    delay(delaytime);
    return 1;
  }
  return 0;
}

void changeMode(void) {
  mode = (mode_t) ((((uint8_t) mode)+1)%8);
}

const uint8_t getMin(void) {
  return cur_min;
}

const uint8_t getHour(void) {
  return cur_hour;
}

const uint8_t getDay(void) {
  return cur_day + 1;
}

const uint8_t getMonth(void) {
  return cur_month + 1;
}

const uint8_t getColor(void) {
  return cur_color;
}

const uint8_t getBri(void) {
  return cur_bri;
}

CRGB getPaint(paint_t intype) {
  uint8_t iy = (cur_bri+1) * 10;
  switch(intype) {
    case time_t:
      return (!cur_color) ? CRGB(iy, 0, iy) : CRGB(iy, 0, 0);
      break;
    case date_t:
      return (!cur_color) ? CRGB(0, iy, iy) : CRGB(0, 0, iy);
      break;
    case both_t:
      return (!cur_color) ? CRGB(0, 0, iy) : CRGB(iy, 0, iy);
      break;
    case spec_t:
      return CRGB(iy, iy, iy);
      break;
    default:
      return CRGB(0,0,0); // none_t
      break; // won't get here
  }
}

void plus(void) {
  DateTime current = rtc.now();
  uint8_t nowsec = current.second();
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
      nowday = (nowday%days_in_months[cur_month])+1;
      break;
    case dig_month: // display me top left, day and month
      nowmonth = (nowmonth%12)+1;
      if (nowday > days_in_months[nowmonth-1]) {
        nowday = days_in_months[nowmonth-1];
      }
      break;
    case dig_color: // display co top left
      cur_color = (getColor()+1)%num_color_combinations;
      break;
    case dig_bright: // display br top left
      if (cur_bri < num_bright_levels - 1) {
        cur_bri = (getBri()+1)%num_bright_levels;
      }
      break;
    default: // word_disp
      break;
  }
  // set new time
  rtc.adjust(DateTime(default_year, nowmonth, nowday, nowhour, nowmin, nowsec));
}

void minus(void) {
  DateTime current = rtc.now();
  uint8_t nowsec = current.second();
  uint8_t nowmin = current.minute();
  uint8_t nowhour = current.hour();
  uint8_t nowday = current.day();
  uint8_t nowmonth = current.month();
  switch (mode) {
    case dig_disp: // display hour, min, day, month
      break;
    case dig_min: // display mi
      if (nowmin == 0) {
        nowmin = 59;
      } else {
        nowmin = (nowmin-1) % 60; // unecessary modulo
      }
      break;
    case dig_hour: // display ho
      if (nowhour == 0) {
        nowhour = 23;
      } else {
        nowhour = (nowhour-1) % 24;
      }
      break;
    case dig_day: // display di
      if (nowday == 1) {
        nowday = days_in_months[cur_month];
      } else {
        nowday = (nowday - 1)%days_in_months[cur_month];
      }
      break;
    case dig_month: // display me top left, day and month
      if (nowmonth == 1) {
        nowmonth = 12;
      } else {
        nowmonth = nowmonth - 1;
      }
      if (nowday > days_in_months[nowmonth-1]) {
        nowday = days_in_months[nowmonth-1];
      }
      break;
    case dig_color: // display co top left
      if (dig_color == 0) {
        cur_color = num_color_combinations-1;
      } else {
        cur_color = (cur_color - 1)%num_color_combinations;
      }
      break;
    case dig_bright: // display br top left
      if (cur_bri > 0) {
        cur_bri = (cur_bri - 1) % num_bright_levels;
      }
      break;
    default: // word_disp
      break;
  }
  rtc.adjust(DateTime(default_year, nowmonth, nowday, nowhour, nowmin, nowsec));
}

// converts row, col format to LED strip format  with 0 based indexing of row and col
// this function depends on how the LED Matrix is oriented
// @param row [0, 15]
// @param col [0, 15]
// @return decoded index of led of interest [0, 255]
uint8_t matrix_decode(uint8_t row, uint8_t col) {
  if (col & 0x01) // odd col
  {
    return (col*16)+(15-row);
  } else { // even col
    return col*16+row;
  }
}

void paint_canvas_dig(uint8_t base_row, uint8_t base_col, uint8_t data_width, const uint8_t data[], paint_t pa) {
  // iterate through data
  for (uint8_t data_row = 0; data_row < 8; ++data_row) {
    for (uint8_t data_col = 0; data_col < data_width; ++data_col) {
      if (bitRead(data[data_row], data_width - data_col - 1)) {
        leds[matrix_decode(base_row + data_row, base_col + data_col)] = getPaint(pa);
      }
    }
  }
}

// animate - specify the layout of the canvas, switching on mode

void animate(void) {
  switch (mode) {
    case dig_disp: // display hour, min, day, month
      paint_canvas_dig(0, 0, 4, dig_maps[getHour()/10], time_t); // paint tens of hour
      paint_canvas_dig(0, 4, 4, dig_maps[getHour()%10], time_t); // paint ones of hour
      paint_canvas_dig(0, 8, 4, dig_maps[getMin()/10], time_t); // paint tens of min
      paint_canvas_dig(0, 12, 4, dig_maps[getMin()%10], time_t); // paint ones of min
      paint_canvas_dig(8, 0, 4, dig_maps[getDay()/10], date_t); // paint tens of day
      paint_canvas_dig(8, 4, 4, dig_maps[getDay()%10], date_t); // paint ones of day
      paint_canvas_dig(8, 8, 8, month_maps[cur_month], date_t); // paint month
      break;
    case dig_min: // display mi
      paint_canvas_dig(0, 0, 8, mi, time_t);
      paint_canvas_dig(0, 8, 4, dig_maps[getMin()/10], time_t); // paint tens of min
      paint_canvas_dig(0, 12, 4, dig_maps[getMin()%10], time_t); // paint ones of min
      break;
    case dig_hour: // display ho
      paint_canvas_dig(0, 0, 8, ho, time_t);
      paint_canvas_dig(0, 8, 4, dig_maps[getHour()/10], time_t); // paint tens of hour
      paint_canvas_dig(0, 12, 4, dig_maps[getHour()%10], time_t); // paint ones of hour
      break;
    case dig_day: // display di
      paint_canvas_dig(0, 0, 8, di, date_t);
      paint_canvas_dig(0, 8, 4, dig_maps[getDay()/10], date_t); // paint tens of day
      paint_canvas_dig(0, 12, 4, dig_maps[getDay()%10], date_t); // paint ones of day
      break;
    case dig_month: // display me top left, day and month
      paint_canvas_dig(0, 0, 8, me, date_t);
      paint_canvas_dig(0, 8, 4, dig_maps[getMonth()/10], date_t); // paint tens of day
      paint_canvas_dig(0, 12, 4, dig_maps[getMonth()%10], date_t); // paint ones of day
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

void paint_canvas_word(uint8_t base_row, uint8_t base_col, uint8_t len, paint_t pa, bool alongCol=false) {
  if (!alongCol) {
    for (uint8_t col = base_col; col < base_col + len; ++col) {
      leds[matrix_decode(base_row, col)] = getPaint(pa);
    }
  } else {
    for (uint8_t row = base_row; row < base_row + len; ++row) {
      leds[matrix_decode(row, base_col)] = getPaint(pa);
    }
  }
}

void display_word(void) {
  
  uint8_t disp_hour = cur_hour;
  if (cur_min >= 45) {
    ++disp_hour; // will be using 'menos'
    if (disp_hour == 24) {
      disp_hour = 0; // handle edge case
    }
  }
  
  // check for special message time
  if (cur_hour == rand_hours[cur_day] && cur_min == rand_minutes[cur_day]) {
    paint_canvas_word(0, 8, 1, time_t); // e
    paint_canvas_word(0, 12, 1, time_t); // r
    paint_canvas_word(0, 15, 1, time_t); // e
    paint_canvas_word(6, 14, 1, time_t); // s
    paint_canvas_word(7, 4, 1, date_t); // l
    paint_canvas_word(8, 4, 1, date_t); // i
    paint_canvas_word(9, 11, 1, date_t); // n
    paint_canvas_word(12, 5, 1, date_t); // d
    paint_canvas_word(14, 9, 1, date_t); // a // i should change this to guapa --> will do
    return;
  }
  // paint canvas with WORDS
  // switch on second for lighting up either hola or audri
  switch (cur_sec%2) {
    case 1:
      paint_canvas_word(0, 0, 4, spec_t);
      break;
     default:
      paint_canvas_word(0, 3, 5, spec_t);
      break;
  }
  // switch on hour for lighting up es la una or son las ?getHour and for 'de la manana, de la tarde, de la noche
  if (disp_hour%12 == 1) {
    paint_canvas_word(1, 3, 2, time_t); // ES
    paint_canvas_word(2, 8, 2, time_t); // LA
  } else {
    paint_canvas_word(3, 1, 3, time_t); // SON
    paint_canvas_word(3, 5, 3, time_t); // LAS
  }
  switch(disp_hour%12) {
    case 0:
      paint_canvas_word(8, 0, 4, time_t); // DOCE
      break;
    case 1:
      paint_canvas_word(4, 12, 3, time_t); // UNA
      break;
    case 2:
      paint_canvas_word(5, 12, 3, time_t); // DOS
      break;
    case 3:
      paint_canvas_word(5, 5, 4, time_t); // TRES
      break;
    case 4:
      paint_canvas_word(4, 1, 6, time_t); // CUATRO
      break;
    case 5:
      paint_canvas_word(4, 7, 5, time_t); // CINCO
      break;
    case 6:
      paint_canvas_word(5, 1, 4, time_t); // SEIS
      break;
    case 7:
      paint_canvas_word(6, 0, 5, time_t); // SIETE
      break;
    case 8:
      paint_canvas_word(6, 5, 4, time_t); // OCHO
      break;
    case 9:
      paint_canvas_word(6, 9, 5, time_t); // NUEVE
      break;
    case 10:
      paint_canvas_word(7, 0, 4, time_t); // DIEZ
      break;
    case 11:
      paint_canvas_word(7, 5, 4, time_t); // ONCE
      break;
    default:
      break;
  }
  // de la
  paint_canvas_word(9, 12, 2, both_t); // DE
  paint_canvas_word(9, 14, 2, time_t); // LA
  if (disp_hour < 12) {
    paint_canvas_word(14, 10, 6, time_t); // MANANA
  } else if (disp_hour < 18) {
    paint_canvas_word(12, 0, 5, time_t); // TARDE
  } else {
    paint_canvas_word(15, 11, 5, time_t); // NOCHE
  }
  // switch on minute_state for 'y cuarto/media or menos cuarto' -- SEE de la tarde/noche/manana
  if (cur_min >= 45) {
    // MENOS CUARTO
    paint_canvas_word(8, 6, 5, time_t); // MENOS
    paint_canvas_word(9, 5, 6, time_t); // CUARTO
  } else if (cur_min >= 30) {
    paint_canvas_word(8, 5, 1, time_t); // Y
    paint_canvas_word(9, 0, 5, time_t); // MEDIA
  } else if (cur_min >= 15) {
    paint_canvas_word(8, 5, 1, time_t); // Y
    paint_canvas_word(9, 5, 6, time_t); // CUARTO
  }
  // switch on day for hoy es el ?dia
  paint_canvas_word(0, 9, 3, date_t); // HOY
  paint_canvas_word(0, 13, 2, date_t); // ES
  paint_canvas_word(1, 1, 2, date_t); // EL
  paint_t shared = (cur_day == disp_hour%12) ? both_t : date_t;
  if ((getDay() >= 10 && getDay() <= 15) || getDay() == 20) {
    switch (getDay()) {
      case 10:
        paint_canvas_word(7, 0, 4, shared); // DIEZ
        break;
      case 11:
        paint_canvas_word(7, 5, 4, shared); // ONCE
        break;
      case 12:
        paint_canvas_word(8, 0, 4, shared); // DOCE
        break;
      case 13:
        paint_canvas_word(8, 11, 5, date_t); // TRECE
        break;
      case 14: 
        paint_canvas_word(3, 8, 7, date_t); // CATORCE
        break;
      case 15:
        paint_canvas_word(7, 9, 6, date_t); // QUINCE
        break;
      case 20:
        paint_canvas_word(2, 10, 6, date_t);
        break;
      default:
        break;
    }
  } else {
    switch (getDay()/10) {
      case 1: // DIECI
        paint_canvas_word(1, 5, 5, date_t); // DIECI
        break;
      case 2: // VEINTI
        paint_canvas_word(1, 10, 6, date_t); // VEINTI
        break;
      case 3: // TREINTA // do some logic for 'Y'
        paint_canvas_word(2, 1, 7, date_t); // TREINTA
        if (getDay() == 31) {
          paint_canvas_word(3, 4, 1, date_t); // Y
        }
        break;
      default:
        break;
    }
    switch (getDay()%10) {
      case 1:
        paint_canvas_word(5, 9, 3, date_t); // UNO
        break;
      case 2:
        paint_canvas_word(5, 12, 3, shared); // DOS
        break;
      case 3:
        paint_canvas_word(5, 5, 4, shared); // TRES
        break;
      case 4:
        paint_canvas_word(4, 1, 6, shared); // CUATRO
        break;
      case 5:
        paint_canvas_word(4, 7, 5, shared); // CINCO
        break;
      case 6:
        paint_canvas_word(5, 1, 4, shared); // SEIS
        break;
      case 7:
        paint_canvas_word(6, 0, 5, shared); // SIETE
        break;
      case 8:
        paint_canvas_word(6, 5, 4, shared); // OCHO
        break;
      case 9:
        paint_canvas_word(6, 9, 5, shared); // NUEVE
        break;
    default:
        break;
    }
  }
  
  // switch on month for de ?mes
  switch(cur_month) {
    case 0:
      paint_canvas_word(10, 0, 5, date_t); // ENERO
      break;
    case 1:
      paint_canvas_word(11, 0, 7, date_t); // FEBRERO
      break;
    case 2:
      paint_canvas_word(11, 7, 5, date_t); // MARZO
      break;
    case 3:
      paint_canvas_word(10, 5, 5, date_t); // ABRIL
      break;
    case 4:
      paint_canvas_word(11, 12, 4, date_t); // MAYO
      break;
    case 5:
      paint_canvas_word(10, 10, 5, date_t); // JUNIO
      break;
    case 6:
      paint_canvas_word(15, 0, 5, date_t); // JULIO
      break;
    case 7:
      paint_canvas_word(13, 0, 6, date_t); // AGOSTO
      break;
    case 8:
      paint_canvas_word(13, 6, 10, date_t); // SEPTIEMBRE
      break;
    case 9:
      paint_canvas_word(15, 4, 7, date_t); // OCTUBRE
      break;
    case 10:
      paint_canvas_word(14, 0, 9, date_t); // NOVIEMBRE
      break;
    case 11:
      paint_canvas_word(12, 6, 9, date_t); // DECIEMBRE
      break;
    default:
      break;
  }

  // birthday!!
  if (cur_month == birth_month && cur_day == birth_day) {
    paint_canvas_word(1, 0, 5, spec_t, true); // FELIZ
    paint_canvas_word(3, 15, 10, spec_t, true); // CUMPLEANOS
  }
}

void setup() {
  // set up matrix with 256 LEDS on pin 6; GRB layout and 800kHz
  FastLED.addLeds<NEOPIXEL, MAT_PIN>(leds, NUM_LEDS);
  pinMode(pin_mode, INPUT);
  pinMode(pin_plus, INPUT);
  pinMode(pin_minus, INPUT);

  if (!rtc.begin()) {
    paint_canvas_word(0, 3, 5, spec_t); // AUDRI
    FastLED.show();
    while(1);
  }
  if (rtc.lostPower()) {
    rtc.adjust(DateTime(default_year, cur_month+1, cur_day+1, cur_hour, cur_min, 0));
  }

}

void loop() {
  // update time
  DateTime nowtime = rtc.now();
  cur_month = nowtime.month() - 1;
  cur_day = nowtime.day() - 1;
  cur_hour = nowtime.hour();
  cur_min = nowtime.minute();
  cur_sec = nowtime.second();
  
  // check if button is pressed
  condExecAndWait(pin_mode, changeMode, delaytime);
  condExecAndWait(pin_plus, plus, delaytime);
  condExecAndWait(pin_minus, minus, delaytime);
  
  // clear canvas
  for (uint16_t i = 0; i < NUM_LEDS; ++i) {
    leds[i] = getPaint(none_t);
  }
  
  // draw on canvas
  animate();

  // show canvas
  FastLED.show();
}
