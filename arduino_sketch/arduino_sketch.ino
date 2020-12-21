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

uint32_t matrix[ROWS][COLS];

// setup() function -- runs once at startup --------------------------------

bool full_aurora_mode = true;

int starting_lines_row[FULL_AURORA_LEN];
int starting_lines_col;
int lines_len[FULL_AURORA_LEN];
int current_direction[FULL_AURORA_LEN];

int starting_lines_row2[AURORA_LEN];
int starting_lines_col2;
int current_direction2[AURORA_LEN];

Rgb color_end = {115, 222, 86};
Rgb color_start = {119, 0, 255};
// Rgb color_start = {6, 28, 1};
//Rgb color_end = {255, 0, 0};
//Rgb color_start = {0, 0, 255};

float modifiers[FULL_AURORA_LEN];

uint32_t white = strip.Color(30, 30, 65);
uint32_t white2 = strip.Color(40, 40, 75);
uint32_t background = strip.Color(1, 1, 19);

#define LINE_LEN 10 // Max possible line length
Rgb line[LINE_LEN];

int cycle = 0;

float toggle_aurora_pct = 0;
Rgb color_src_start;
Rgb color_src_end;
Rgb color_background = {1, 1, 19};
Rgb original_color_end = {115, 222, 86};
Rgb original_color_start = {119, 0, 255};
bool visible_aurora = true;


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

bool update_toggle_aurora_pct(void *) {
  toggle_aurora_pct += 0.01;
  if (visible_aurora == true) {
    // Note: store base colors somewhere
    color_start = {
      lerp(color_src_start.r, original_color_start.r, toggle_aurora_pct),
      lerp(color_src_start.g, original_color_start.g, toggle_aurora_pct),
      lerp(color_src_start.b, original_color_start.b, toggle_aurora_pct)
    };

    color_end = {
      lerp(color_src_end.r, original_color_end.r, toggle_aurora_pct),
      lerp(color_src_end.g, original_color_end.g, toggle_aurora_pct),
      lerp(color_src_end.b, original_color_end.b, toggle_aurora_pct)
    };
  } else {
    color_start = {
      lerp(color_src_start.r, color_background.r, toggle_aurora_pct),
      lerp(color_src_start.g, color_background.g, toggle_aurora_pct),
      lerp(color_src_start.b, color_background.b, toggle_aurora_pct)
    };

    color_end = {
      lerp(color_src_end.r, color_background.r, toggle_aurora_pct),
      lerp(color_src_end.g, color_background.g, toggle_aurora_pct),
      lerp(color_src_end.b, color_background.b, toggle_aurora_pct)
    };
  }

  if (visible_aurora == false && toggle_aurora_pct >= 0.99) {
    full_aurora_mode = !full_aurora_mode;
    init_lines();
  }

  return (toggle_aurora_pct >= 0.99) ? false : true;
}



bool toggle_aurora(void *) {
  Serial.print("toggling aurora... \n");
  toggle_aurora_pct = 0;
  color_src_start.r = color_start.r;
  color_src_start.g = color_start.g;
  color_src_start.b = color_start.b;

  color_src_end.r = color_end.r;
  color_src_end.g = color_end.g;
  color_src_end.b = color_end.b;

  visible_aurora = !visible_aurora;

  /*
  if (color_start.r == 0 && color_start.g == 0 && color_start.b == 0 &&
      color_end.r == 0 && color_end.g == 0 && color_end.b == 0) {
    color_end = {115, 222, 86};
    color_start = {119, 0, 255};
  } else {
    color_start = {1, 1, 19};
    color_end = {1, 1, 19};
  }
  */

  timer.every(100, update_toggle_aurora_pct);

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

  srand(time(NULL));

  timer.every(20000, toggle_aurora);
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
    rx = (float)color_start.r * pct + (float)color_end.r * (1 - pct);
    gx = (float)color_start.g * pct + (float)color_end.g * (1 - pct);
    bx = (float)color_start.b * pct + (float)color_end.b * (1 - pct);
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