#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

enum LightingMode {
   Off = 0,
   FullOn = 1,
   CraftingOnly = 2,
   Party = 3
};

// Our buttons are Active Low. So let’s define some helper words for not screwing that up
#define PRESSED LOW
#define RELEASED HIGH

// Variables for lighting system
const int LED_COUNT = 300;           // This is the total count of LEDs for the entire livingroom
const int LED_DATA_PIN = 6;          // This is the output pin that delivers Color data to the LED Strip
LightingMode lightingMode = Off;     // We start with the lights off

//TODO: START REMOVE
const int ledPin = 13;   // this needs to be removed
int ledState = HIGH;     // the current state of the output pin
//TODO: END REMOVE

// On Button
const int LIGHTING_ON_BUTTON_PIN = 2; // (Active-Low) Input pin connects to the button that will turn on/off the lights
int onButton_State = RELEASED;        // the current currentButtonState of the input pin for Lighting_On_Button
int onButton_LastState = RELEASED;    // the previous currentButtonState from the input pin for Lighting_On_Button
long onButton_LastDebounceTime = 0;   // the last time the output pin was toggled
bool onButton_pressHandled = false;   // We only want to fire our LED drawing routines once when a button is pressed.

// Mode Button
const int LIGHTING_MODE_BUTTON_PIN = 3; // (Active-Low) Input pin connects to the button that will change the mode of the lights
int modeButton_State = RELEASED;        // the current currentButtonState from the input pin for Lighting_Mode_Button
int modeButton_LastState = RELEASED;    // the previous currentButtonState from the input pin for the Lighting_Mode_Button
long modeButton_LastDebounceTime = 0;   // the last time the output pin was toggled
bool modeButton_pressHandled = false;   // We only want to fire our LED drawing routines once when a button is pressed.

// the following variables are long's because the time, measured in milliseconds,
// will quickly become a bigger number than can be stored in an int.
long debounceDelay = 50;    // the debounce time; increase if the output flickers

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_COUNT, LED_DATA_PIN, NEO_GRB + NEO_KHZ800);

//TODO:REMOVE
// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

void setup() {
  // Initialize the lights as off
  strip.begin();
  strip.show(); 

  // Initialize the buttons
  pinMode(LIGHTING_ON_BUTTON_PIN, INPUT_PULLUP);
  pinMode(LIGHTING_MODE_BUTTON_PIN, INPUT_PULLUP);

  //TODO:REMOVE
  pinMode(ledPin, OUTPUT);
}

void loop() {
  
  // Check the states of both of our buttons to determine if there have been any
  // button presses.
  int currentButtonState = HIGH;

  // Check On Button
  // read the state of the switch into a local variable:
  currentButtonState = digitalRead(LIGHTING_ON_BUTTON_PIN);

  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH),  and you've waited
  // long enough since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (currentButtonState != onButton_LastState) {
    // reset the debouncing timer
    onButton_LastDebounceTime = millis();
  }

  if ((millis() - onButton_LastDebounceTime) > debounceDelay) {
    // whatever the currentButtonState is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:
    onButton_State = currentButtonState;
  }
  // ----- Use On Button State to determine light output ---------------------
  // If the the On Button has been pressed, and the button press has not yet
  // been handled, we need to execute our On/Off logic
  if (onButton_State == LOW && ! onButton_pressHandled) {
    if ( lightingMode == Off ) {
      lightingMode = FullOn;
    } else {
      lightingMode = Off;
    }
    
  }

  // Check Mode Button
  // read the state of the switch into a local variable:
  currentButtonState = digitalRead(LIGHTING_MODE_BUTTON_PIN);

  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH),  and you've waited
  // long enough since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (currentButtonState != modeButton_LastState) {
    // reset the debouncing timer
    modeButton_LastDebounceTime = millis();
  }

  if ((millis() - modeButton_LastDebounceTime) > debounceDelay) {
    // whatever the currentButtonState is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:
    modeButton_State = currentButtonState;
  }






/*  // Some example procedures showing how to display to the pixels:
  colorWipe(strip.Color(255, 0, 0), 50); // Red
  colorWipe(strip.Color(0, 255, 0), 50); // Green
  colorWipe(strip.Color(0, 0, 255), 50); // Blue
//colorWipe(strip.Color(0, 0, 0, 255), 50); // White RGBW
  // Send a theater pixel chase in...
  theaterChase(strip.Color(127, 127, 127), 50); // White
  theaterChase(strip.Color(127, 0, 0), 50); // Red
  theaterChase(strip.Color(0, 0, 127), 50); // Blue

  rainbow(20);
  rainbowCycle(20);
  theaterChaseRainbow(50);
  */
 
}

// Puts the LED lighting into which ever lighting mode we should be in.
void changeLightingMode(LightingMode mode) {
  switch(mode) {
    case Off:
      strip.fill(strip.Color(0, 0, 0), 0, LED_COUNT);
      break;
    case FullOn:
      strip.fill(strip.Color(255, 255, 255), 0, LED_COUNT);
      break;
    case CraftingOnly:
      //TODO: Determine real values for the room empirically on the lighting circuits are installed
      //TODO: This is important because the correct blocks of LED to change could be across several
      //TODO: Different ranges. Kicking this can down the road for testing purposes
      strip.fill(strip.Color(255, 255, 255), 149, LED_COUNT - 150);
      break;
    case Party:
      strip.fill(strip.Color(255, 255, 255), 149, LED_COUNT - 150);
      Break;
    default:
      //This should never happen!!! If it does...flash a few times
      strip.fill(strip.Color(0, 0, 127), 0, LED_COUNT);
  }
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

