//
// most helpful:
//    https://forum.arduino.cc/t/arduino-uno-event-keypad-implementation/476904/19
//
// references:
//    https://github.com/FastLED/FastLED/wiki/Basic-usage
//    https://www.quora.com/In-Arduino-how-to-measure-exact-time-between-two-events-happening-eg-sensor-interrupted-Is-there-any-specific-function
//
#include <FastLED.h>
#include "Keypad.h"

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
#define NUM_LEDS    64
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

// 
// LED stuff
//
uint32_t buttonColours[] = {CRGB::Green, CRGB::Blue, CRGB::Red, CRGB::Orange, CRGB::Yellow, CRGB::Black};
int button1ColourIndex = 0;
int button2ColourIndex = 0;
int button3ColourIndex = 0;
int button4ColourIndex = 0;

int player1[] = {0,21};
int player2[] = {player1[1],player1[1]+11};
int player3[] = {player2[1],player2[1]+21};
int player4[] = {player3[1],player3[1]+11};

//initialize an instance of class NewKeypad
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

const unsigned long LongPress = 1000;
bool random_mode = false;

void setup() {
  delay(3000); // 3 second delay for recovery

  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
  Serial.begin(9600);

  Serial.println("hello world");

  // blink the lights during setup
  for (int index = 0; index<3; index++){
      for (int dot = 0; dot < NUM_LEDS; dot++) {
        leds[dot] = CRGB::Green;
      }
      FastLED.show();
      delay(500);
      for (int dot = 0; dot < NUM_LEDS; dot++) {
        leds[dot] = CRGB::Black;
      }
      FastLED.show();
      delay(500);
  }
}

// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = { rainbow, rainbowWithGlitter, confetti, sinelon, juggle, bpm };

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns

//
// main loop
//
void loop()
{
  if (keypad.getKeys()) // check for keypad activity
  {
    // we won't handle multiple keypresses, just single ones, so just key index 0
    const byte key = keypad.key[0].kchar;
    const byte state = keypad.key[0].kstate; // IDLE, PRESSED, HOLD, RELEASED

    switch (key) {

      case '1': {
          static unsigned long pressedTime; // static so the value is remembered like a global
          if (state == PRESSED) {
            pressedTime = millis();
          } else if (state == RELEASED) {
            if (millis() - pressedTime > LongPress) {
              //handleLongPress();
              Serial.println("LONG*** press");
            } else {
              Serial.println("SHORT press");
              button1ShortPress();
            }
          } else if (state == HOLD) {
              Serial.println("holding...");
          }
        } break;

      case '2': {
          static unsigned long pressedTime; // static so the value is remembered like a global
          if (state == PRESSED) {
            pressedTime = millis();
          } else if (state == RELEASED) {
            if (millis() - pressedTime > LongPress) {
              //handleLongPress();
              Serial.println("LONG*** press");
            } else {
              Serial.println("SHORT press");
              button2ShortPress();
            }
          } else if (state == HOLD) {
              Serial.println("holding...");
          }
        } break;
      case '3': {
          static unsigned long pressedTime; // static so the value is remembered like a global
          if (state == PRESSED) {
            pressedTime = millis();
          } else if (state == RELEASED) {
            if (millis() - pressedTime > LongPress) {
              //handleLongPress();
              Serial.println("LONG*** press");
            } else {
              Serial.println("SHORT press");
              button3ShortPress();
            }
          } else if (state == HOLD) {
              Serial.println("holding...");
          }
        } break;
      case '4': {
          static unsigned long pressedTime; // static so the value is remembered like a global
          if (state == PRESSED) {
            pressedTime = millis();
          } else if (state == RELEASED) {
            if (millis() - pressedTime > LongPress) {
              //handleLongPress();
              Serial.println("LONG*** press");
            } else {
              Serial.println("SHORT press");
              button4ShortPress();
            }
          } else if (state == HOLD) {
              Serial.println("holding...");
          }
        } break;
      case '0': {
          // all OFF
          allLEDsOff();
        } break;
      case '*': {
          // random mode on!
          allLEDsOff();
          if (random_mode){
             random_mode = false;
          } else {
             random_mode = true;
          }
        } break;

    }
  } else if (random_mode){
    // taken directly from FastLED DemoReel100
    // Call the current pattern function once, updating the 'leds' array
    gPatterns[gCurrentPatternNumber]();

    // send the 'leds' array out to the actual LED strip
    FastLED.show();
    // insert a delay to keep the framerate modest
    FastLED.delay(1000/FRAMES_PER_SECOND);

    // do some periodic updates
    EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow
    EVERY_N_SECONDS( 10 ) { nextPattern(); } // change patterns periodically
  }
}

void button1ShortPress() {
  Serial.println("button 1 short");
  // light 'em up!
  random_mode = false;
  button1ColourIndex++;
  button1ColourIndex = button1ColourIndex % (sizeof(buttonColours) / sizeof((buttonColours)[0]));

  for (int dot = player1[0]; dot < player1[1]; dot++) {
    leds[dot] = buttonColours[button1ColourIndex];
  }
  FastLED.show();
}

void button2ShortPress() {
  Serial.println("button 2 short");
  // light 'em up!
  random_mode = false;
  button2ColourIndex++;
  button2ColourIndex = button2ColourIndex % (sizeof(buttonColours) / sizeof((buttonColours)[0]));

  for (int dot = player2[0]; dot < player2[1]; dot++) {
    leds[dot] = buttonColours[button2ColourIndex];
  }
  FastLED.show();
}

void button3ShortPress() {
  Serial.println("button 3 short");
  // light 'em up!
  random_mode = false;
  button3ColourIndex++;
  button3ColourIndex = button3ColourIndex % (sizeof(buttonColours) / sizeof((buttonColours)[0]));

  for (int dot = player3[0]; dot < player3[1]; dot++) {
    leds[dot] = buttonColours[button3ColourIndex];
  }
  FastLED.show();
}

void button4ShortPress() {
  Serial.println("button 4 short");
  // light 'em up!
  random_mode = false;
  button4ColourIndex++;
  button4ColourIndex = button4ColourIndex % (sizeof(buttonColours) / sizeof((buttonColours)[0]));

  for (int dot = player4[0]; dot < player4[1]; dot++) {
    leds[dot] = buttonColours[button4ColourIndex];
  }
  FastLED.show();
}

void allLEDsOff() {
  // all off
  Serial.println("button 0: all off");
  random_mode = false;

  for (int dot = 0; dot < NUM_LEDS; dot++) {
    leds[dot] = CRGB::Black;
  }
  FastLED.show();
  random_mode = false;
}

//
// from FASTLED examples, DemoReel100
//
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
  if( random8() < chanceOfGlitter) {
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
  int pos = beatsin16( 13, 0, NUM_LEDS-1 );
  leds[pos] += CHSV( gHue, 255, 192);
}

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, 20);
  uint8_t dothue = 0;
  for( int i = 0; i < 8; i++) {
    leds[beatsin16( i+7, 0, NUM_LEDS-1 )] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}

