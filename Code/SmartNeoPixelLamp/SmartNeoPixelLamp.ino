/*
  Author: Sascha Vis
  Student number: 0962873
  Website: https://www.lenteguppie.com
  Required: ESP8266 board version 2.5.0!
  Hardware:
    - Wemos D1 R2 WiFi board
    - NeoPixel Strip WS2812B
    - RobotDyn capacitive touch sensor
    - some wires
  Version: 1.0
*/
/* Libraries */
#include <Adafruit_NeoPixel.h> //Library for adressing the NeoPixels

/*NeoPixel strip variables*/
#define BUTTON_PIN   2 // Button pin to change the the animations without being connected to the internet
#define PIXEL_PIN    0  // Digital IO pin connected to the NeoPixels.
#define PIXEL_COUNT 50  // Number of NeoPixels

// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

int mode = 0;    // Currently-active animation mode, 0-9

/* Code that need to run once */
void setup() {
  pinMode(BUTTON_PIN, INPUT); // Declare our button to be an input component
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), nextAnimation, RISING);  //Attach the interrupt to the button pin.
  strip.begin(); // Initialize NeoPixel strip object (REQUIRED)
  strip.show();  // Initialize all pixels to 'off'
}

void loop() {
  switch (mode) {          // Start the new animation...
    case 0:
      colorWipe(strip.Color(  0,   0,   0), 50);    // Black/off
      break;
    case 1:
      colorWipe(strip.Color(255,   0,   0), 50);    // Red
      break;
    case 2:
      colorWipe(strip.Color(  0, 255,   0), 50);    // Green
      break;
    case 3:
      colorWipe(strip.Color(  0,   0, 255), 50);    // Blue
      break;
    case 4:
      theaterChase(strip.Color(127, 127, 127), 50); // White
      break;
    case 5:
      theaterChase(strip.Color(127,   0,   0), 50); // Red
      break;
    case 6:
      theaterChase(strip.Color(  0,   0, 127), 50); // Blue
      break;
    case 7:
      rainbow(10); // Create a rainbow animation
      break;
    case 8:
      theaterChaseRainbow(50);
      break;
  }
}

void nextAnimation() {
  if (++mode > 8) mode = 0; // Advance to next mode, wrap around after #8
}

/* Fill strip pixels one after another with a color. Strip is NOT cleared
   first; anything there will be covered pixel by pixel. Pass in color
   (as a single 'packed' 32-bit value, which you can get by calling
   strip.Color(red, green, blue) as shown in the loop() function above),
   and a delay time (in milliseconds) between pixels.*/
void colorWipe(uint32_t color, int wait) {
  for (int i = 0; i < strip.numPixels(); i++) { // For each pixel in strip...
    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
    delay(wait);                           //  Pause for a moment
  }
}

/* Theater-marquee-style chasing lights. Pass in a color (32-bit value,
   a la strip.Color(r,g,b) as mentioned above), and a delay time (in ms)
   between frames.*/
void theaterChase(uint32_t color, int wait) {
  for (int a = 0; a < 10; a++) { // Repeat 10 times...
    for (int b = 0; b < 3; b++) { //  'b' counts from 0 to 2...
      strip.clear();         //   Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of strip in steps of 3...
      for (int c = b; c < strip.numPixels(); c += 3) {
        strip.setPixelColor(c, color); // Set pixel 'c' to value 'color'
      }
      strip.show(); // Update strip with new contents
      delay(wait);  // Pause for a moment
    }
  }
}

// Rainbow cycle along whole strip. Pass delay time (in ms) between frames.
void rainbow(int wait) {
  // Hue of first pixel runs 3 complete loops through the color wheel.
  // Color wheel has a range of 65536 but it's OK if we roll over, so
  // just count from 0 to 3*65536. Adding 256 to firstPixelHue each time
  // means we'll make 3*65536/256 = 768 passes through this outer loop:
  for (long firstPixelHue = 0; firstPixelHue < 3 * 65536; firstPixelHue += 256) {
    for (int i = 0; i < strip.numPixels(); i++) { // For each pixel in strip...
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
  for (int a = 0; a < 30; a++) { // Repeat 30 times...
    for (int b = 0; b < 3; b++) { //  'b' counts from 0 to 2...
      strip.clear();         //   Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of strip in increments of 3...
      for (int c = b; c < strip.numPixels(); c += 3) {
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
