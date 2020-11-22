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

#define LED_PIN    6
#define ROWS      13
#define COLS      27
#define LED_COUNT COLS * ROWS
#define AURORA_LEN 20


Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
// Argument 3 = Pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)

uint32_t matrix[ROWS][COLS];

// setup() function -- runs once at startup --------------------------------

int starting_lines_row[AURORA_LEN];
int starting_lines_col[AURORA_LEN];
int lines_len[AURORA_LEN];
float modifiers[AURORA_LEN];

void init_lines() {
  for (int i = 0; i < AURORA_LEN; i++) {
    starting_lines_row[i] = 10;
    starting_lines_col[i] = i + 3;
    lines_len[i] = 7; //rand() % 4 + 3;
    modifiers[i] = 1.0;
  }
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
}


// loop() function -- runs repeatedly as long as board is on ---------------

uint32_t blue = strip.Color(59, 59, 111);
uint32_t red = strip.Color(180, 47, 53);
uint32_t white = strip.Color(50, 50, 95);
uint32_t white2 = strip.Color(70, 70, 135);
// uint32_t background = strip.Color(40, 10, 93);
uint32_t background = strip.Color(1, 1, 19);

Rgb color_start = {115, 222, 86};
Rgb color_end = {119, 0, 255};

int r1 = 115;
int g1 = 222;
int b1 = 86;
int r2 = 119;
int g2 = 0;
int b2 = 255;

#define LINE_LEN 7
Rgb line[LINE_LEN];

void init_line() {
  float pct, rx, gx, bx;

  for (int i = 0; i < LINE_LEN; i++) {
    pct = (float)i / (float)(LINE_LEN - 1);
    rx = (float)color_start.r * pct + (float)color_end.r * (1 - pct);
    gx = (float)color_start.g * pct + (float)color_end.g * (1 - pct);
    bx = (float)color_start.b * pct + (float)color_end.b * (1 - pct);
    line[i] = {(int)rx, (int)gx, (int)bx};
  }
}

// Row start decimal is the row start * 10
void render_line_at(int row_start_decimal, int col) {
  int row_start = ceil((float)row_start_decimal / 10.0);
  int additional_px = 0;
  float shift = (row_start_decimal % 10) / 10.0; // eg. row_start_decimal = 92, shift = 0.2
  uint32_t px_color;

  // If we don't align perfectly on the pixel grid…
  if (row_start_decimal % 10 != 0) {
    additional_px = 1;
  }

  Serial.print("\n\nadditional px? ");
  Serial.print(additional_px);

  for (int i = 0; i < (LINE_LEN + additional_px); i++) {
    if (shift == 0.0) {
      px_color = strip.Color(line[i].r, line[i].g, line[i].b);
    } else if (i == 0) {
      px_color = strip.Color(
        (float)line[i].r * (float)shift,
        (float)line[i].g * (float)shift,
        (float)line[i].b * (float)shift
      );
    } else if (i == LINE_LEN) {
      Serial.print("\n");
      Serial.print("END rgb(");
      Serial.print((float)line[i - 1].r * (1 - shift));
      Serial.print(", ");
      Serial.print((float)line[i - 1].g * (1 - shift));
      Serial.print(", ");
      Serial.print((float)line[i - 1].b * (1 - shift));
      Serial.print(")");
      px_color = strip.Color(
        (float)line[i - 1].r * (1 - shift),
        (float)line[i - 1].g * (1 - shift),
        (float)line[i - 1].b * (1 - shift)
      );
    } else if (i == LINE_LEN - 1) {
      Serial.print("\n");
      Serial.print("END-1 rgb(");
      Serial.print((float)line[i].r * (float)shift);
      Serial.print(", ");
      Serial.print((float)line[i].g * (float)shift);
      Serial.print(", ");
      Serial.print((float)line[i].b * (float)shift);
      Serial.print(")");
      px_color = strip.Color(
        (float)line[i].r * (float)shift,
        (float)line[i].g * (float)shift,
        (float)line[i].b * (float)shift
      );
      //px_color = strip.Color(255, 0, 0);
      px_color = strip.Color(
        (float)line[i].r * shift + (float)line[i - 1].r * (1 - shift),
        (float)line[i].g * shift + (float)line[i - 1].g * (1 - shift),
        (float)line[i].b * shift + (float)line[i - 1].b * (1 - shift)
      );
    } else {
      Serial.print("\n");
      Serial.print("ELSE rgb(");
      Serial.print((float)line[i].r * shift + (float)line[i + 1].r * (1 - shift));
      Serial.print(", ");
      Serial.print((float)line[i].g * shift + (float)line[i + 1].g * (1 - shift));
      Serial.print(", ");
      Serial.print((float)line[i].b * shift + (float)line[i + 1].b * (1 - shift));
      Serial.print(")");
      px_color = strip.Color(
        (float)line[i].r * shift + (float)line[i + 1].r * (1 - shift),
        (float)line[i].g * shift + (float)line[i + 1].g * (1 - shift),
        (float)line[i].b * shift + (float)line[i + 1].b * (1 - shift)
      );
    }

    strip.setPixelColor(cartesianToStrip(col, row_start - i), px_color);
  }
}

void loop() {
  Serial.begin(9600); // Démarrage de la communication série  
  //Serial.print("starting...");

  init_line();
  /*
  render_line_at(90, 12);
  render_line_at(95, 15);
  render_line_at(100, 18);
  */

  for (int i = 0; i <= 5; i++) {
    render_line_at(90 + (i * 2), 3 + i);
  }

  for (int i = 0; i <= 10; i++) {
    render_line_at(90 + i, 12 + i);
  }
  strip.show();
  while(true) {}

  Serial.print("line: ");
  for (int i = 0; i < LINE_LEN; i++) {
    Serial.print("\n");
    Serial.print("rgb(");
    Serial.print(line[i].r);
    Serial.print(", ");
    Serial.print(line[i].g);
    Serial.print(", ");
    Serial.print(line[i].b);
    Serial.print(")");
  }

  while(true) {}

  //return;
  for (int i = 0; i < 27 + 13; i++) {
    //bhm_gradient_line(0, i, i, 0, 0, 65534);
  }

  aurora_loop();

  //strip.setPixelColor(cartesianToStrip(i, i), strip.Color(  255,   0, 0));
}


// Some functions of our own for creating animated effects -----------------

int cycle = 0;
int current_direction = 0;
Rgb a, b, y, z; // a & b: two top pixels, y & z: two down pixels

void aurora_loop() {
  while (true) {
    cycle++;
    if (cycle > 10) {
      cycle = 0;
    }
    clear_screen();
    update_lines();
    render_lines();
    strip.show();
    //delay(50);
  }
}

void clear_screen() {
  //for (int i = 0; i < LED_COUNT; i++) {
    //strip.setPixelColor(i, background);
  //}

  for (int col = 0; col < COLS; col++) {
    bhm_gradient_line_rgb(col, 12, col, 0, 1, 1, 19, 0, 0, 4);
  }

  // Stars
  strip.setPixelColor(cartesianToStrip(3, 1), (rand() % 2 == 0) ? white : white2);
  strip.setPixelColor(cartesianToStrip(2, 11), (rand() % 2 == 0) ? white : white2);
  strip.setPixelColor(cartesianToStrip(6, 10), (rand() % 2 == 0) ? white : white2);
  strip.setPixelColor(cartesianToStrip(7, 12), (rand() % 2 == 0) ? white : white2);
  strip.setPixelColor(cartesianToStrip(11, 11), (rand() % 2 == 0) ? white : white2);
  strip.setPixelColor(cartesianToStrip(17, 9), (rand() % 2 == 0) ? white : white2);
  strip.setPixelColor(cartesianToStrip(23, 12), (rand() % 2 == 0) ? white : white2);
  strip.setPixelColor(cartesianToStrip(26, 8), (rand() % 2 == 0) ? white : white2);
  strip.setPixelColor(cartesianToStrip(25, 3), (rand() % 2 == 0) ? white : white2);
  strip.setPixelColor(cartesianToStrip(7, 6), (rand() % 2 == 0) ? white : white2);
  strip.setPixelColor(cartesianToStrip(21, 4), (rand() % 2 == 0) ? white : white2);
  strip.setPixelColor(cartesianToStrip(15, 2), (rand() % 2 == 0) ? white : white2);
}

void render_lines() {
  for (int i = 0; i < 1; i++) {
    // Green 115, 222, 86
    // Purple 119, 0, 255
    // Red 232, 72, 142
    // Blue 66, 148, 154

    r1 = 115;
    g1 = 222;
    b1 = 86;
    r2 = 119;
    g2 = 0;
    b2 = 255;


    bhm_gradient_line_rgb(
      starting_lines_col[i], starting_lines_row[i], 
      starting_lines_col[i], starting_lines_row[i] - lines_len[i], 
      r1, g1, b1,
      r2, g2, b2
    );
 /*   
    bhm_gradient_line_rgb(
      starting_lines_col[i], starting_lines_row[i], 
      starting_lines_col[i], starting_lines_row[i] - lines_len[i], 
      r1, g1, b1,
      r2, g2, b2
    ); //60300, 19300);
    */
  }
  Serial.print("---");
}

void update_lines() {
  int random_chance = 2;
  
  for (int i = 0; i < AURORA_LEN; i++) {
    // Update starting rows
    if (cycle >= 0) {
      // Update starting lines row – begginning of a new cycle
      

      // Update current direction
      if (i == 0) {
        if (rand() % 2 == 0 && starting_lines_row[i] > 0) {
          current_direction = -1;
        } else if(starting_lines_row[i] < 12) {
          current_direction = 1;
        } else {
          current_direction = 0;
        }
      } else {
        if (starting_lines_row[i - 1] - starting_lines_row[i] > 1) {
          current_direction = 1;
        } else if (starting_lines_row[i - 1] - starting_lines_row[i] < -1) {
          current_direction = -1;
        } else if (starting_lines_row[i - 1] - starting_lines_row[i] == 1) {
          if (rand() % random_chance == 0 && starting_lines_row[i] < 12) {
            current_direction = 1;
          } else {
            current_direction = 0;
          }
        } else if (starting_lines_row[i - 1] - starting_lines_row[i] == -1) {
          if (rand() % random_chance == 0 && (starting_lines_row[i] - lines_len[i]) > 0) {
            current_direction = -1;
          } else {
            current_direction = 0;
          }
        } else {
          if (rand() % 2 == 0 && (starting_lines_row[i] - lines_len[i]) > 0) {
            current_direction = -1;
          } else if (starting_lines_row[i] < 12) {
            current_direction = 1;
          } else {
            current_direction = 1;
          }
        }
      }

      if (current_direction == -1) {
        starting_lines_row[i]--;
      } else if (current_direction == 1) {
        starting_lines_row[i]++;
      }

      // Compute a, b, y, z
      a.r = r2;
      a.g = g2;
      a.b = b2;

      int pct_b =  1 / lines_len[i];
      b.r = r1 * pct_b + r2 * (1 - pct_b);
      b.g = g1 * pct_b + g2 * (1 - pct_b);
      b.b = b1 * pct_b + b2 * (1 - pct_b);

      int pct_y = (lines_len[i] - 1) / lines_len[i];
      y.r = r1 * pct_y + r2 * (1 - pct_y);
      y.g = g1 * pct_y + g2 * (1 - pct_y);
      y.b = b1 * pct_y + b2 * (1 - pct_y);

      z.r = r1;
      z.g = g1;
      z.b = b1;

      // Update lines length
      /*
      if (rand() % 2 == 0 && lines_len[i] < 7) {
        lines_len[i]++;
      } else if (rand() % 2 == 1 && lines_len[i] > 5) {
        lines_len[i]--;
      }*/
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

// fast drawing where x1 == x2
void bhm_gradient_line(int x1, int y1, int x2, int y2, long startHue, long endHue) {
  long hue;
  int gradStep;
  int yStart = min(y1, y2);
  int yEnd = max(y1, y2);
  hue = startHue;
  gradStep = gradientStep(startHue, endHue, yEnd - yStart + 1);
  
  for (int i = yStart; i <= yEnd; i++) {
    strip.setPixelColor(cartesianToStrip(x1, i), strip.gamma32(strip.ColorHSV(hue)));
    hue += gradStep;
  }
}

long gradientStep(long startHue, long endHue, int len) {
  startHue = startHue % 65535;
  endHue = endHue % 65535;

  return (endHue - startHue) / len;
}