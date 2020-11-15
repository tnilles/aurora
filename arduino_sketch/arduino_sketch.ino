// A basic everyday NeoPixel strip test program.

// NEOPIXEL BEST PRACTICES for most reliable operation:
// - Add 1000 uF CAPACITOR between NeoPixel strip's + and - connections.
// - MINIMIZE WIRING LENGTH between microcontroller board and first pixel.
// - NeoPixel strip's DATA-IN should pass through a 300-500 OHM RESISTOR.
// - AVOID connecting NeoPixels on a LIVE CIRCUIT. If you must, ALWAYS
//   connect GROUND (-) first, then +, then data.
// - When using a 3.3V microcontroller with a 5V-powered NeoPixel strip,
//   a LOGIC-LEVEL CONVERTER on the data line is STRONGLY RECOMMENDED.
// (Skipping these may work OK on your workbench but can fail in the field)

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

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1:
#define LED_PIN    6

// How many NeoPixels are attached to the Arduino?
#define ROWS      13
#define COLS      27
#define LED_COUNT COLS * ROWS
#define AURORA_LEN 20

// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
// Argument 1 = Number of pixels in NeoPixel strip
// Argument 2 = Arduino pin number (most are valid)
// Argument 3 = Pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)

uint32_t matrix[ROWS][COLS];

// #define BUF_LEN 128
// char buffer[BUF_LEN];

// setup() function -- runs once at startup --------------------------------

int starting_lines_row[AURORA_LEN];
int starting_lines_col[AURORA_LEN];
int lines_len[AURORA_LEN];
float modifiers[AURORA_LEN];

void init_lines() {
  for (int i = 0; i < AURORA_LEN; i++) {
    starting_lines_row[i] = 10;
    starting_lines_col[i] = i + 3;
    lines_len[i] = rand() % 4 + 3;
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

void loop() {
  Serial.begin(9600); // Démarrage de la communication série  
  //Serial.print("starting...");

//  bhm_line(0, 0, 10, 0, red);
//  bhm_line(0, 0, 10, 10, red);
//  bhm_line(0, 0, 0, 10, red);

  //rainbowStripes(1);
  //return;
  for (int i = 0; i < 27 + 13; i++) {
    //bhm_gradient_line(0, i, i, 0, 0, 65534);
  }

  aurora_loop();
  
  clear_screen();
  render_lines();
  strip.show();

  //usFlag();

  for(int i = 0; i < 13; i++) {
    //strip.setPixelColor(cartesianToStrip(i, i), strip.Color(  255,   0, 0));
  }
  
  //strip.show();

//  usFlag();
 
//  setupMatrix();
//  printMatrix();
//  renderMatrix();

//  // Fill along the length of the strip in various colors...
//  colorWipe(strip.Color(255,   0,   0), 50); // Red
//  colorWipe(strip.Color(  0, 255,   0), 50); // Green
//  colorWipe(strip.Color(  0,   0, 255), 50); // Blue
//
//  // Do a theater marquee effect in various colors...
//  theaterChase(strip.Color(127, 127, 127), 50); // White, half brightness
//  theaterChase(strip.Color(127,   0,   0), 50); // Red, half brightness
//  theaterChase(strip.Color(  0,   0, 127), 50); // Blue, half brightness
//
//  rainbow(10);             // Flowing rainbow cycle along the whole strip
//  theaterChaseRainbow(50); // Rainbow-enhanced theaterChase variant
}


// Some functions of our own for creating animated effects -----------------

void aurora_loop() {
  while (true) {
    clear_screen();
    update_lines();
    render_lines();
    strip.show();
    delay(50);
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
  int r1, g1, b1, r2, g2, b2;
  for (int i = 0; i < AURORA_LEN; i++) {
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

    if (rand() % 2 == 0 && modifiers[i] < 1.5) {
      modifiers[i] += 0.1;
    } else if (rand() % 2 == 0 && modifiers[i] > 0.5) {
      modifiers[i] -= 0.1;
    }

    r1 = (float)r1 * modifiers[i];
    g1 = (float)g1 * modifiers[i];
    b1 = (float)b1 * modifiers[i];
    r2 = (float)r2 * modifiers[i];
    g2 = (float)g2 * modifiers[i];
    b2 = (float)b2 * modifiers[i];

    if (r1 < 0) { r1 = 0; }
    if (g1 < 0) { g1 = 0; }
    if (b1 < 0) { b1 = 0; }
    if (r2 < 0) { r2 = 0; }
    if (g2 < 0) { g2 = 0; }
    if (b2 < 0) { b2 = 0; }

    if (r1 > 255) { r1 = 255; }
    if (g1 > 255) { g1 = 255; }
    if (b1 > 255) { b1 = 255; }
    if (r2 > 255) { r2 = 255; }
    if (g2 > 255) { g2 = 255; }
    if (b2 > 255) { b2 = 255; }
    
    
    bhm_gradient_line_rgb(starting_lines_col[i], starting_lines_row[i], starting_lines_col[i], starting_lines_row[i] - lines_len[i], r1, g1, b1, r2, g2, b2); //60300, 19300);
  }
  Serial.print("---");
}

void update_lines() {
  int col_modifier = 0;
  int random_chance = 2;
  
  // Update starting cols
  if (rand() % 2 == 0 && starting_lines_row[0] > 0) {
    col_modifier = -1;
  } else if (rand() % 2 == 1 && starting_lines_row[AURORA_LEN] < 27) {
    col_modifier = 1;
  }
  
  for (int i = 0; i < AURORA_LEN; i++) {
    // Update starting cols
    //starting_lines_col[i] = starting_lines_col[i] + col_modifier;
    
    // Update starting rows
    if (i == 0) {
      if (rand() % 2 == 0 && starting_lines_row[i] > 0) {
        starting_lines_row[i]--;
      } else if(starting_lines_row[i] < 12) {
        starting_lines_row[i]++;
      }
    } else {
      if (starting_lines_row[i - 1] - starting_lines_row[i] > 1) {
        starting_lines_row[i]++;
      } else if (starting_lines_row[i - 1] - starting_lines_row[i] < -1) {
        starting_lines_row[i]--;
      } else if (starting_lines_row[i - 1] - starting_lines_row[i] == 1) {
        if (rand() % random_chance == 0 && starting_lines_row[i] < 12) {
          starting_lines_row[i]++;
        }
      } else if (starting_lines_row[i - 1] - starting_lines_row[i] == -1) {
        if (rand() % random_chance == 0 && (starting_lines_row[i] - lines_len[i]) > 0) {
          starting_lines_row[i]--;
        }
      } else {
        if (rand() % 2 == 0 && (starting_lines_row[i] - lines_len[i]) > 0) {
          starting_lines_row[i]--;
        } else if (starting_lines_row[i] < 12) {
          starting_lines_row[i]++;
        }
      }
    }

    // Update lines length
    if (rand() % 2 == 0 && lines_len[i] < 7) {
      lines_len[i]++;
    } else if (rand() % 2 == 1 && lines_len[i] > 5) {
      lines_len[i]--;
    }
  }
}

void aurora() {
  int xStart = 3;
  int yStart = 10;
  int len = 20;
  int x = xStart;
  int y = yStart;
  int y2;

  for (int i = 0; i < 20; i++) {
    y2 = rand() % 3;
    bhm_gradient_line(x, y, x, y2, 12000, 37000);
    x++;
    if (rand() % 2 == 0 && y < yStart) {
      y++;
    } else if (y > 0) {
      y--;
    }
  }
}

void rainbowBands(int wait) {
  for(long firstPixelHue = 0; firstPixelHue < 5*65536; firstPixelHue += 128) {
    for(int i = 0; i < COLS + ROWS; i++) {
      int pixelHue = firstPixelHue + (i * 65536L / strip.numPixels());
      bhm_line(i, 0, i, 12, strip.gamma32(strip.ColorHSV(pixelHue)));
    }
    strip.show();
    delay(wait);
  }
}


void rainbowStripes(int wait) {
  for(long firstPixelHue = 0; firstPixelHue < 5*65536; firstPixelHue += 256) {
    for(int i = 0; i < COLS + ROWS; i++) {
      int pixelHue = firstPixelHue + (i * 65536L / strip.numPixels());
      // strip.ColorHSV() can take 1 or 3 arguments: a hue (0 to 65535) or
      // optionally add saturation and value (brightness) (each 0 to 255).
      // Here we're using just the single-argument hue variant. The result
      // is passed through strip.gamma32() to provide 'truer' colors
      // before assigning to each pixel:
      bhm_line(0, i, i, 0, strip.gamma32(strip.ColorHSV(pixelHue)));
    }
    strip.show();
    delay(wait);
  }
}

int cartesianToStrip(int x, int y) {
  return (ROWS - 1 - y) * COLS + (y % 2 == 0 ? COLS - 1 - x : x); // inverted
}

void bhm_line(int x1, int y1, int x2, int y2, uint32_t color) {
  int x, y, dx, dy, dx1, dy1, px, py, xe, ye, i;
  dx = x2 - x1;
  dy = y2 - y1;
  dx1 = fabs(dx);
  dy1 = fabs(dy);
  px = 2 * dy1 - dx1;
  py = 2 * dx1 - dy1;
  if (dy1 <= dx1) {
    if (dx >= 0) {
      x = x1;
      y = y1;
      xe = x2;
    } else {
      x = x2;
      y = y2;
      xe = x1;
    }
    strip.setPixelColor(cartesianToStrip(x, y), color);
    
    for (i = 0; x < xe; i++) {
      x = x + 1;
      if (px < 0) {
        px = px + 2 * dy1;
      } else {
        if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) {
          y = y + 1;
        } else {
          y = y - 1;
        }
        px = px + 2 * (dy1 - dx1);
      }
      delay(0);
      strip.setPixelColor(cartesianToStrip(x, y), color);
    }
  } else {
    if (dy >= 0) {
      x = x1;
      y = y1;
      ye = y2;
    } else {
      x = x2;
      y = y2;
      ye = y1;
    }
    strip.setPixelColor(cartesianToStrip(x, y), color);
    
    for (i = 0; y < ye; i++) {
      y = y + 1;
      if (py <= 0) {
        py = py + 2 * dx1;
      } else {
        if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) {
          x = x + 1;
        } else {
          x = x - 1;
        }
        py = py + 2 * (dx1 - dy1);
      }
      delay(0);
      strip.setPixelColor(cartesianToStrip(x, y), color);
    }
  }
}

// RGB version
// fast drawing where x1 == x2
void bhm_gradient_line_rgb(int x1, int y1, int x2, int y2, int r1, int g1, int b1, int r2, int g2, int b2) {
  int rx, gx, bx;
  float yStart = (float) min(y1, y2);
  float yEnd = (float) max(y1, y2);
  float pct;
  
  for (float i = yStart; i <= yEnd; i++) {
    pct = (i - yStart) / (yEnd - yStart);
    rx = (float)r1 * ((i - yStart) / (yEnd - yStart)) + (float)r2 * (1 - ((i - yStart) / (yEnd - yStart)));
    gx = (float)g1 * ((i - yStart) / (yEnd - yStart)) + (float)g2 * (1 - ((i - yStart) / (yEnd - yStart)));
    bx = (float)b1 * ((i - yStart) / (yEnd - yStart)) + (float)b2 * (1 - ((i - yStart) / (yEnd - yStart)));
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

void bhm_gradient_line_slow(int x1, int y1, int x2, int y2, long startHue, long endHue) {
  int x, y, dx, dy, dx1, dy1, px, py, xe, ye, i, gradStep;
  long hue;
  dx = x2 - x1;
  dy = y2 - y1;
  dx1 = fabs(dx);
  dy1 = fabs(dy);
  px = 2 * dy1 - dx1;
  py = 2 * dx1 - dy1;
  hue = startHue;
  
  if (dy1 <= dx1) {
    if (dx >= 0) {
      x = x1;
      y = y1;
      xe = x2;
    } else {
      x = x2;
      y = y2;
      xe = x1;
    }
    gradStep = gradientStep(startHue, endHue, xe - x);
    Serial.print(hue);
    Serial.print(", ");
    strip.setPixelColor(cartesianToStrip(x, y), strip.gamma32(strip.ColorHSV(hue)));
    
    for (i = 0; x < xe; i++) {
      x = x + 1;
      hue += gradStep;
      if (px < 0) {
        px = px + 2 * dy1;
      } else {
        if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) {
          y = y + 1;
        } else {
          y = y - 1;
        }
        px = px + 2 * (dy1 - dx1);
      }
      delay(0);
      Serial.print(hue);
      Serial.print(", ");
      strip.setPixelColor(cartesianToStrip(x, y), strip.gamma32(strip.ColorHSV(hue)));
    }
  } else {
    if (dy >= 0) {
      x = x1;
      y = y1;
      ye = y2;
    } else {
      x = x2;
      y = y2;
      ye = y1;
    }
    gradStep = gradientStep(startHue, endHue, ye - y);
    strip.setPixelColor(cartesianToStrip(x, y), strip.gamma32(strip.ColorHSV(hue)));
    
    for (i = 0; y < ye; i++) {
      y = y + 1;
      hue += gradStep;
      if (py <= 0) {
        py = py + 2 * dx1;
      } else {
        if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) {
          x = x + 1;
        } else {
          x = x - 1;
        }
        py = py + 2 * (dx1 - dy1);
      }
      delay(0);
      strip.setPixelColor(cartesianToStrip(x, y), strip.gamma32(strip.ColorHSV(hue)));
    }
  }
}

long gradientStep(long startHue, long endHue, int len) {
  startHue = startHue % 65535;
  endHue = endHue % 65535;

  return (endHue - startHue) / len;
}

void renderMatrix() {
  strip.clear();
  for(int row = 0; row < ROWS; row++) {
    for(int col = 0; col < COLS; col++) {
      strip.setPixelColor(cartesianToStrip(col, row), matrix[row][col]);
    }
  }
  strip.show();
}

// Fill strip pixels one after another with a color. Strip is NOT cleared
// first; anything there will be covered pixel by pixel. Pass in color
// (as a single 'packed' 32-bit value, which you can get by calling
// strip.Color(red, green, blue) as shown in the loop() function above),
// and a delay time (in milliseconds) between pixels.
void colorWipe(uint32_t color, int wait) {
  for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
    delay(wait);                           //  Pause for a moment
  }
}

// Theater-marquee-style chasing lights. Pass in a color (32-bit value,
// a la strip.Color(r,g,b) as mentioned above), and a delay time (in ms)
// between frames.
void theaterChase(uint32_t color, int wait) {
  for(int a=0; a<10; a++) {  // Repeat 10 times...
    for(int b=0; b<3; b++) { //  'b' counts from 0 to 2...
      strip.clear();         //   Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of strip in steps of 3...
      for(int c=b; c<strip.numPixels(); c += 3) {
        strip.setPixelColor(c, color); // Set pixel 'c' to value 'color'
      }
      strip.show(); // Update strip with new contents
      delay(wait);  // Pause for a moment
    }
  }
}

// Rainbow cycle along whole strip. Pass delay time (in ms) between frames.
void rainbow(int wait) {
  // Hue of first pixel runs 5 complete loops through the color wheel.
  // Color wheel has a range of 65536 but it's OK if we roll over, so
  // just count from 0 to 5*65536. Adding 256 to firstPixelHue each time
  // means we'll make 5*65536/256 = 1280 passes through this outer loop:
  for(long firstPixelHue = 0; firstPixelHue < 5*65536; firstPixelHue += 256) {
    for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
      // Offset pixel hue by an amount to make one full revolution of the
      // color wheel (range of 65536) along the length of the strip
      // (strip.numPixels() steps):
      int pixelHue = firstPixelHue + (i * 65536L / strip.numPixels());
      // strip.ColorHSV() can take 1 or 3 arguments: a hue (0 to 65535) or
      // optionally add saturation and value (brightness) (each 0 to 255).
      // Here we're using just the single-argument hue variant. The result
      // is passed through strip.gamma32() to provide 'truer' colors
      // before assigning to each pixel:
      strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue)));
    }
    strip.show(); // Update strip with new contents
    delay(wait);  // Pause for a moment
  }
}

// Rainbow-enhanced theater marquee. Pass delay time (in ms) between frames.
void theaterChaseRainbow(int wait) {
  int firstPixelHue = 0;     // First pixel starts at red (hue 0)
  for(int a=0; a<30; a++) {  // Repeat 30 times...
    for(int b=0; b<3; b++) { //  'b' counts from 0 to 2...
      strip.clear();         //   Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of strip in increments of 3...
      for(int c=b; c<strip.numPixels(); c += 3) {
        // hue of pixel 'c' is offset by an amount to make one full
        // revolution of the color wheel (range 65536) along the length
        // of the strip (strip.numPixels() steps):
        int      hue   = firstPixelHue + c * 65536L / strip.numPixels();
        uint32_t color = strip.gamma32(strip.ColorHSV(hue)); // hue -> RGB
        strip.setPixelColor(c, color); // Set pixel 'c' to value 'color'
      }
      strip.show();                // Update strip with new contents
      delay(wait);                 // Pause for a moment
      firstPixelHue += 65536 / 90; // One cycle of color wheel over 90 frames
    }
  }
}
