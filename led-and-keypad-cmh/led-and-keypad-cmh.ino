//
// helpful:
//    https://github.com/FastLED/FastLED/wiki/Basic-usage
//    https://www.quora.com/In-Arduino-how-to-measure-exact-time-between-two-events-happening-eg-sensor-interrupted-Is-there-any-specific-function
//
#include <FastLED.h>
#include "Adafruit_Keypad.h"

FASTLED_USING_NAMESPACE

// FastLED "100-lines-of-code" demo reel, showing just a few
// of the kinds of animation patterns you can quickly and easily
// compose using FastLED.
//
// This example also shows one easy way to define multiple
// animations patterns and have them automatically rotate.
//
// -Mark Kriegsman, December 2014

//
// LED stuff
//
#define DATA_PIN    3
//#define CLK_PIN   4
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB
#define NUM_LEDS    30
CRGB leds[NUM_LEDS];

#define BRIGHTNESS          96
#define FRAMES_PER_SECOND  120

//
// keypad stuff
//
const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
//define the symbols on the buttons of the keypads
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {11, 10, 9, 8}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {7, 6, 5, 4}; //connect to the column pinouts of the keypad

int button1PRESSEventTime;
int button1PressHOLDtime = 0;
bool button1PRESSED = false;
uint32_t button1Colours[] = {CRGB::Green, CRGB::Blue, CRGB::Red, CRGB::Orange, CRGB::Yellow};
int button1ColourIndex = 0;

//initialize an instance of class NewKeypad
Adafruit_Keypad customKeypad = Adafruit_Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup() {
  delay(3000); // 3 second delay for recovery

  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  //FastLED.addLeds<LED_TYPE,DATA_PIN,CLK_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
  Serial.begin(9600);

  customKeypad.begin();

}


// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = { rainbow, rainbowWithGlitter, confetti, sinelon, juggle, bpm };

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns

void loop()
{

  customKeypad.tick();

  // button 1
  if (button1PRESSED) {
    button1PressHOLDtime  = millis() -  button1PRESSEventTime;
    button1Behaviour();
  }

  // check keypad
  if (customKeypad.available()) {
    keypadEvent e = customKeypad.read();

    // button 1
    if ((char)e.bit.KEY == '1') {

      // has the 1 button just been pressed?
      if (e.bit.EVENT == KEY_JUST_PRESSED) {
        button1PRESSEventTime = millis();
        button1PressHOLDtime = 0;
        button1PRESSED = true;
      } else if (e.bit.EVENT == KEY_JUST_RELEASED) {
        // reset button 1 press
        button1PRESSEventTime = 0;
        button1PRESSED = false;
      }

      button1Behaviour();

    } else if ((char)e.bit.KEY == '2') {
      for (int dot = 0; dot < 10; dot++) {
        leds[dot] = CRGB::Blue;
      }
      FastLED.show();
    } else if ((char)e.bit.KEY == '0') {
      // clear this led for the next time around the loop
      for (int dot = 0; dot < NUM_LEDS; dot++) {
        leds[dot] = CRGB::Black;
      }
      FastLED.show();
    }
  }
}

//
// button 1 control
//
void button1Behaviour()
{

  if (button1PressHOLDtime > 1000) {
    // light 'em up!
    for (int dot = 0; dot < 10; dot++) {
      leds[dot] = button1Colours[button1ColourIndex];
    }
    FastLED.show();
    delay(500);
    for (int dot = 0; dot < 10; dot++) {
      leds[dot] = CRGB::Black;
    }
    FastLED.show();
    delay(500);
  } else {
    // light 'em up!
    for (int dot = 0; dot < 10; dot++) {
      leds[dot] = CRGB::Red;
    }
    FastLED.show();
  }
}

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void nextPattern()
{
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);
}

void rainbow()
{
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, NUM_LEDS, gHue, 7);
}

void rainbowWithGlitter()
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
}

void addGlitter( fract8 chanceOfGlitter)
{
  if ( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}

void confetti()
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV( gHue + random8(64), 200, 255);
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16( 13, 0, NUM_LEDS - 1 );
  leds[pos] += CHSV( gHue, 255, 192);
}

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for ( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue + (i * 2), beat - gHue + (i * 10));
  }
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, 20);
  uint8_t dothue = 0;
  for ( int i = 0; i < 8; i++) {
    leds[beatsin16( i + 7, 0, NUM_LEDS - 1 )] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}
