#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif
#include "defs.h"
#include <arduino-timer.h>

#define LED_PIN    6
#define ROWS      13
#define COLS      27
#define LED_COUNT COLS * ROWS
#define AURORA_LEN 14 // when having two smaller auroras
#define FULL_AURORA_LEN 20

auto timer = timer_create_default();

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
// Argument 3 = Pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)

// setup() function -- runs once at startup --------------------------------

int starting_lines_row[FULL_AURORA_LEN];
int starting_lines_col;
int lines_len[FULL_AURORA_LEN];
int current_direction[FULL_AURORA_LEN];

int starting_lines_row2[AURORA_LEN];
int starting_lines_col2;
int current_direction2[AURORA_LEN];

float modifiers[FULL_AURORA_LEN];

uint32_t white = strip.Color(30, 30, 65);
uint32_t white2 = strip.Color(40, 40, 75);

#define LINE_LEN 7 // Max possible line length
Rgb line[LINE_LEN];

int cycle = 0;

float shift_aurora_pct = 0;

ColorPair green_purple = {{119, 0, 255}, {0, 255, 0}};
ColorPair blue_red = {{9, 66, 181}, {161, 32, 10}};
ColorPair orange_green = {{222, 128, 13}, {65, 153, 11}};
ColorPair turquoise_purple = {{8, 201, 131}, {63, 10, 199}};

ColorPair previous_color;
ColorPair current_color = green_purple; //{{160, 0, 255}, {115, 222, 12}};
ColorPair next_color;
ColorPair background_color = {{1, 1, 19}, {0, 0, 4}};

bool full_aurora_mode = true;
bool visible_aurora = true;

ColorPair colors[3];

void setup_colors() {
  colors[0] = green_purple;
  colors[1] = blue_red;
  colors[2] = orange_green;
  colors[3] = {{8, 201, 131}, {63, 10, 199}};
}

void init_lines() {
  int length = AURORA_LEN;

  if (full_aurora_mode == true) {
    length = FULL_AURORA_LEN;
  }

  for (int i = 0; i < length; i++) {
    starting_lines_row[i] = 10 * 10;
    starting_lines_col = 3;
    lines_len[i] = 7;

    if (full_aurora_mode == false) {
      starting_lines_row[i] = 12 * 10;
      starting_lines_col = 2;
      starting_lines_row2[i] = 6 * 10;
      starting_lines_col2 = 12;
      lines_len[i] = 4;
    }

    modifiers[i] = 1.0;
  }
}

int lerp(int src, int dest, float pct) {
  return src * (1 - pct) + dest * pct; 
}

bool update_shift_aurora_pct(void *) {
  shift_aurora_pct += 0.01;

  current_color.start = {
    lerp(previous_color.start.r, next_color.start.r, shift_aurora_pct),
    lerp(previous_color.start.g, next_color.start.g, shift_aurora_pct),
    lerp(previous_color.start.b, next_color.start.b, shift_aurora_pct)
  };

  current_color.end = {
    lerp(previous_color.end.r, next_color.end.r, shift_aurora_pct),
    lerp(previous_color.end.g, next_color.end.g, shift_aurora_pct),
    lerp(previous_color.end.b, next_color.end.b, shift_aurora_pct)
  };

  if (next_color.start.r == background_color.start.r && 
    next_color.start.g == background_color.start.g && 
    next_color.start.b == background_color.start.b && 
    next_color.end.r == background_color.end.r && 
    next_color.end.g == background_color.end.g && 
    next_color.end.b == background_color.end.b && shift_aurora_pct >= 0.99) {
    full_aurora_mode = !full_aurora_mode;
    init_lines();
  }

  return (shift_aurora_pct >= 0.99) ? false : true;
}

void color_shift() {
  shift_aurora_pct = 0;

  previous_color.start.r = current_color.start.r;
  previous_color.start.g = current_color.start.g;
  previous_color.start.b = current_color.start.b;
  previous_color.end.r = current_color.end.r;
  previous_color.end.g = current_color.end.g;
  previous_color.end.b = current_color.end.b;

  timer.every(100, update_shift_aurora_pct);
}

bool change_aurora(void *) {
  if (rand() % 2 == 0 || (next_color.start.r == background_color.start.r && 
    next_color.start.g == background_color.start.g && 
    next_color.start.b == background_color.start.b && 
    next_color.end.r == background_color.end.r && 
    next_color.end.g == background_color.end.g && 
    next_color.end.b == background_color.end.b)) {
    next_color = colors[rand() % 3];
  } else {
    // Split/merge aurora(s)
    next_color = background_color;
  }

  color_shift();

  return true; // keep timer active
}

void setup() {
  // These lines are specifically to support the Adafruit Trinket 5V 16 MHz.
  // Any other board, you can remove this part (but no harm leaving it):
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
  // END of Trinket-specific code.

  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)

  init_lines();
  setup_colors();

  srand(time(NULL));

  timer.every(20000, change_aurora);
}


// loop() function -- runs repeatedly as long as board is on ---------------

// Row start decimal is the row start * 10
void render_line_at(int row_start_decimal, int col, int len) {
  float pct, rx, gx, bx;

  int row_start = ceil((float)row_start_decimal / 10.0);
  int additional_px = 0;
  float shift = (float)(row_start_decimal % 10) / 10.0; // eg. row_start_decimal = 92, shift = 0.2
  uint32_t px_color;

  float coef = 1;// (float)(1.0 - ((float)1.0 / (float)(len - 1)));

  for (int i = 0; i < len; i++) {
    pct = (float)i / (float)(len - 1);
    rx = (float)current_color.start.r * pct + (float)current_color.end.r * (1 - pct);
    gx = (float)current_color.start.g * pct + (float)current_color.end.g * (1 - pct);
    bx = (float)current_color.start.b * pct + (float)current_color.end.b * (1 - pct);
    line[i] = {(int)rx, (int)gx, (int)bx};
  }

  // If we don't align perfectly on the pixel grid…
  if (row_start_decimal % 10 != 0) {
    additional_px = 1;
  }

  for (int i = 0; i < (len + additional_px); i++) {
    if (shift == 0.0) {
      if (i == 0 || i == (len + additional_px) - 1) {
        px_color = strip.Color(line[i].r, line[i].g, line[i].b);
      } else {
        coef = 0.9;
        px_color = strip.Color(
          (float)line[i].r * (1 - coef) + (float)line[i + 1].r * coef,
          (float)line[i].g * (1 - coef) + (float)line[i + 1].g * coef,
          (float)line[i].b * (1 - coef) + (float)line[i + 1].b * coef
        );
      }
    } else if (i == 0) {
      px_color = strip.Color(
        (float)line[i].r * (float)shift,
        (float)line[i].g * (float)shift,
        (float)line[i].b * (float)shift
      );
      //px_color = strip.Color(0, 0, 0);
    } else if (i == len) {
      px_color = strip.Color(
        (float)line[i - 1].r * (1 - shift),
        (float)line[i - 1].g * (1 - shift),
        (float)line[i - 1].b * (1 - shift)
      );
      //px_color = strip.Color(0, 0, 0);
    } else if (i == len - 1) {
      px_color = strip.Color(
        (float)line[i].r * shift + (float)line[i - 1].r * (1 - shift),
        (float)line[i].g * shift + (float)line[i - 1].g * (1 - shift),
        (float)line[i].b * shift + (float)line[i - 1].b * (1 - shift)
      );
      //px_color = strip.Color(0, 0, 0);
    } else {
      px_color = strip.Color(
        (float)line[i].r * (1 - shift) + (float)line[i + 1].r * shift,
        (float)line[i].g * (1 - shift) + (float)line[i + 1].g * shift,
        (float)line[i].b * (1 - shift) + (float)line[i + 1].b * shift
      );
    }

    if (col >= 0 && col <= 26 && row_start - i >= 0 && row_start - i <= 12) {
      strip.setPixelColor(cartesianToStrip(col, row_start - i), px_color);
    }
  }
}

void loop() {
  Serial.begin(9600);
  //Serial.print("starting...");

  aurora_loop();
}

void aurora_loop() {
  while (true) {
    timer.tick();
    cycle++;
    if (cycle > 10) {
      cycle = 0;
    }
    clear_screen();
    update_lines();
    render_lines();
    strip.show();
  }
}

void clear_screen() {
  for (int col = 0; col < COLS; col++) {
    bhm_gradient_line_rgb(col, 12, col, 0, 1, 1, 19, 0, 0, 4);
  }

  // Stars
  strip.setPixelColor(cartesianToStrip(3, 1), (rand() % 10 == 0) ? white : white2);
  strip.setPixelColor(cartesianToStrip(2, 11), (rand() % 10 == 0) ? white : white2);
  strip.setPixelColor(cartesianToStrip(6, 10), (rand() % 10 == 0) ? white : white2);
  strip.setPixelColor(cartesianToStrip(7, 12), (rand() % 10 == 0) ? white : white2);
  strip.setPixelColor(cartesianToStrip(11, 11), (rand() % 10 == 0) ? white : white2);
  strip.setPixelColor(cartesianToStrip(17, 9), (rand() % 10 == 0) ? white : white2);
  strip.setPixelColor(cartesianToStrip(23, 12), (rand() % 10 == 0) ? white : white2);
  strip.setPixelColor(cartesianToStrip(26, 8), (rand() % 10 == 0) ? white : white2);
  strip.setPixelColor(cartesianToStrip(25, 3), (rand() % 10 == 0) ? white : white2);
  strip.setPixelColor(cartesianToStrip(7, 6), (rand() % 10 == 0) ? white : white2);
  strip.setPixelColor(cartesianToStrip(21, 4), (rand() % 10 == 0) ? white : white2);
  strip.setPixelColor(cartesianToStrip(15, 2), (rand() % 10 == 0) ? white : white2);
}

void render_lines() {
  int length = AURORA_LEN;

  if (full_aurora_mode == true) {
    length = FULL_AURORA_LEN;
  }

  for (int i = 0; i < length; i++) {
    // Green 115, 222, 86
    // Purple 119, 0, 255
    // Red 232, 72, 142
    // Blue 66, 148, 154

    render_line_at(starting_lines_row[i], starting_lines_col + i, lines_len[i]);

    if (full_aurora_mode == false) {
      render_line_at(starting_lines_row2[i], starting_lines_col2 + i, lines_len[i]);
    }
  }
}

void update_lines() {
  int random_chance = 2;
  int length = AURORA_LEN;
  int aurora1_upper_limit = 9;

  if (full_aurora_mode == true) {
    length = FULL_AURORA_LEN;
    aurora1_upper_limit = 7;
  }
  
  for (int i = 0; i < length; i++) {
    // Update starting rows
    if (cycle == 0 || cycle == 5) {
      // Update starting lines row – begginning of a new cycle

      // Update current direction
      
      if (i == 0) {
        if (rand() % 2 == 0 && starting_lines_row[i] > aurora1_upper_limit * 10) {
          current_direction[i] = -1;
        } else if(starting_lines_row[i] < 12 * 10) {
          current_direction[i] = 1;
        } else {
          current_direction[i] = 0;
        }
      } else {
        if (starting_lines_row[i - 1] - starting_lines_row[i] > 1 * 5) {
          current_direction[i] = 1;
        } else if (starting_lines_row[i - 1] - starting_lines_row[i] < -1 * 5) {
          current_direction[i] = -1;
        } else if (starting_lines_row[i - 1] - starting_lines_row[i] == 1 * 10) {
          if (rand() % random_chance == 0 && starting_lines_row[i] < 12 * 10) {
            current_direction[i] = 1;
          } else {
            current_direction[i] = 0;
          }
        } else if (starting_lines_row[i - 1] - starting_lines_row[i] == -1 * 10) {
          if (rand() % random_chance == 0 && (starting_lines_row[i] - lines_len[i]) > aurora1_upper_limit * 10) {
            current_direction[i] = -1;
          } else {
            current_direction[i] = 0;
          }
        } else {
          if (rand() % 2 == 0 && (starting_lines_row[i] - lines_len[i]) > aurora1_upper_limit * 10) {
            current_direction[i] = -1;
          } else if (starting_lines_row[i] < 12 * 10) {
            current_direction[i] = 1;
          } else {
            current_direction[i] = 1;
          }
        }
      }

      // SECOND AURORA

      if (full_aurora_mode == false) {
        if (i == 0) {
          if (rand() % 2 == 0 && starting_lines_row2[i] > 3 * 10) {
            current_direction2[i] = -1;
          } else if(starting_lines_row2[i] < 6 * 10) {
            current_direction2[i] = 1;
          } else {
            current_direction2[i] = 0;
          }
        } else {
          if (starting_lines_row2[i - 1] - starting_lines_row2[i] > 1 * 5) {
            current_direction2[i] = 1;
          } else if (starting_lines_row2[i - 1] - starting_lines_row2[i] < -1 * 5) {
            current_direction2[i] = -1;
          } else if (starting_lines_row2[i - 1] - starting_lines_row2[i] == 1 * 10) {
            if (rand() % random_chance == 0 && starting_lines_row2[i] < 6 * 10) {
              current_direction2[i] = 1;
            } else {
              current_direction2[i] = 0;
            }
          } else if (starting_lines_row2[i - 1] - starting_lines_row2[i] == -1 * 10) {
            if (rand() % random_chance == 0 && (starting_lines_row2[i] - lines_len[i]) > 3 * 10) {
              current_direction2[i] = -1;
            } else {
              current_direction2[i] = 0;
            }
          } else {
            if (rand() % 2 == 0 && (starting_lines_row2[i] - lines_len[i]) > 3 * 10) {
              current_direction2[i] = -1;
            } else if (starting_lines_row2[i] < 6 * 10) {
              current_direction2[i] = 1;
            } else {
              current_direction2[i] = 1;
            }
          }
        }
      }

      // Update lines length
      if (rand() % 2 == 0 && lines_len[i] < 7) {
        //lines_len[i]++;
      } else if (rand() % 2 == 1 && lines_len[i] > 5) {
        //lines_len[i]--;
      }
    }
   
    if (current_direction[i] == -1) {
      starting_lines_row[i]--;
    } else if (current_direction[i] == 1) {
      starting_lines_row[i]++;
    }

    if (full_aurora_mode == false) {
      if (current_direction2[i] == -1) {
        starting_lines_row2[i]--;
      } else if (current_direction2[i] == 1) {
        starting_lines_row2[i]++;
      }
    }
  }
}

int cartesianToStrip(int x, int y) {
  return (ROWS - 1 - y) * COLS + (y % 2 == 0 ? COLS - 1 - x : x); // inverted
}

// RGB version fast drawing where x1 == x2
void bhm_gradient_line_rgb(int x1, int y1, int x2, int y2, int r1, int g1, int b1, int r2, int g2, int b2) {
  int rx, gx, bx;
  float yStart = (float) min(y1, y2);
  float yEnd = (float) max(y1, y2);
  float pct;
  
  for (float i = yStart; i <= yEnd; i++) {
    pct = (i - yStart) / (yEnd - yStart);
    rx = (float)r1 * pct + (float)r2 * (1 - pct);
    gx = (float)g1 * pct + (float)g2 * (1 - pct);
    bx = (float)b1 * pct + (float)b2 * (1 - pct);
    strip.setPixelColor(cartesianToStrip(x1, i), strip.Color((int)rx, (int)gx, (int)bx));
  }
}