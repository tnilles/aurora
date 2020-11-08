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

  srand(time(NULL));
}


// loop() function -- runs repeatedly as long as board is on ---------------

uint32_t blue = strip.Color(59, 59, 111);
uint32_t red = strip.Color(180, 47, 53);
uint32_t white = strip.Color(50, 50, 95);


// uint32_t background = strip.Color(40, 10, 93);
uint32_t background = strip.Color(1, 1, 19);

void loop() {
  Serial.begin(9600); // Démarrage de la communication série  
  Serial.print("starting...");

//  bhm_line(0, 0, 10, 0, red);
//  bhm_line(0, 0, 10, 10, red);
//  bhm_line(0, 0, 0, 10, red);

  //rainbowStripes(1);

  for (int i = 0; i < 27 + 13; i++) {
    //bhm_gradient_line(0, i, i, 0, 0, 65534);
  }
  clearScreen();
  aurora();
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

void clearScreen() {
  for (int i = 0; i < LED_COUNT; i++) {
    strip.setPixelColor(i, background);
  }

  // Stars
  strip.setPixelColor(cartesianToStrip(3, 1), white);
  strip.setPixelColor(cartesianToStrip(2, 11), white);
  strip.setPixelColor(cartesianToStrip(6, 10), white);
  strip.setPixelColor(cartesianToStrip(7, 12), white);
  strip.setPixelColor(cartesianToStrip(11, 11), white);
  strip.setPixelColor(cartesianToStrip(17, 9), white);
  strip.setPixelColor(cartesianToStrip(23, 12), white);
  strip.setPixelColor(cartesianToStrip(26, 8), white);
  strip.setPixelColor(cartesianToStrip(25, 3), white);
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
    bhm_gradient_line(x, y, x, y2, 12000, 20000);
    bhm_line(x, y2, x, 0, background);
    bhm_line(x, 12, x, y - 1, background);
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

void usFlag() {
  uint32_t color;
  for(int row = 0; row < ROWS; row++) { 
    for(int col = 0; col < COLS; col++) {
      if (row < 7 && col < 10) {
        if (col > 0 && col < 9 && row > 0 && row < 6 && ((row * 9 + col) % 2 == 0)) {
          color = strip.Color(255, 255, 255);
        } else {
          color = strip.Color(59, 59, 111);
        }
      } else {
        color = row % 2 == 0 ? strip.Color(180, 47, 53) : strip.Color(255, 255, 255);
      }
      strip.setPixelColor(cartesianToStrip(col, row), color);
    }
  }
  strip.show();
}

void ukFlag() {
  for(int i = 0; i < LED_COUNT; i++) {
    strip.setPixelColor(i, blue);
  }

  bhm_line(1, 0, 26, 11, white);
  bhm_line(0, 1, 25, 12, white);
  
  bhm_line(1, 12, 26, 1, white);
  bhm_line(0, 11, 25, 0, white);
  
  bhm_line(12, 0, 12, 12, white);
  bhm_line(14, 0, 14, 12, white);
  
  bhm_line(0, 5, 26, 5, white);
  bhm_line(0, 7, 26, 7, white);
  
  bhm_line(0, 0, 26, 12, red);
  bhm_line(0, 12, 26, 0, red);
  bhm_line(13, 0, 13, 12, red);
  bhm_line(0, 6, 26, 6, red);
}

void frFlag() {
  for (int i = 0; i < COLS; i++) {
    if (i < 9) {
      bhm_line(i, 0, i, 12, blue);
    } else if (i <= 18) {
      bhm_line(i, 0, i, 12, white);
    } else {
      bhm_line(i, 0, i, 12, red);
    }
  }
}

void setupMatrix() {
  for(int row = 0; row < ROWS; row++) {
    for(int col = 0; col < COLS; col++) {
      matrix[row][col] = (row % 2 == 0 ? strip.Color(255, 0, 0) : strip.Color(255, 255, 255));
    }
  }
}

int cartesianToStrip(int x, int y) {
  // return (LED_COUNT - 1) - COLS * y + (y % 2 == 0 ? x : COLS - 1 - x);
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
      strip.setPixelColor(cartesianToStrip(x, y), color);;
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

void bhm_gradient_line(int x1, int y1, int x2, int y2, long startHue, long endHue) {
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

  Serial.print("(");
  Serial.print(startHue);
  Serial.print("/");
  Serial.print(endHue);
  Serial.print("/");
  Serial.print(len);
  Serial.print("=>");
  Serial.print((endHue - startHue) / len);
  Serial.print(")");

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

//void printMatrix()
//{
//    int i, j;
//
//    // Print elements of two dimensional array.
//    for (i = 0; i < ROWS; i++)
//    {
//        for (j = 0; j < COLS; j++)
//        {
//            snprintf(buffer, sizeof(buffer), "%d ", matrix[i][j]);
//            Serial.print(buffer);
//        }
//    }
//    Serial.print("\n");
//}

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
