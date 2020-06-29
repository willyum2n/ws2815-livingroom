#include "FastLED.h"

// ----------- ADD NEW MODES HERE ----------
// Search for NEW MODES to find other places to change
// When adding new modes, these modes need to be reflected in the mode button code: Search for zzModeButtonzz
enum LightingMode {
   Off = 0,
   FullOn = 1,
   CraftingOnly = 2,
   Party = 3,
   DancingWater = 4
};

// Our buttons are Active Low. So let’s define some helper words for not screwing that up
#define PRESSED LOW
#define RELEASED HIGH

// Variables for lighting system
const int LED_COUNT = 300;           // This is the total count of LEDs for the entire livingroom
CRGB ledStrip[LED_COUNT];            // Storage for color data
const int LED_DATA_PIN = 6;          // This is the output pin that delivers Color data to the LED Strip
LightingMode lightingMode = Off;     // We start with the lights off

// On Button
const int LIGHTING_ON_BUTTON_PIN = 2; // (Active-Low) Input pin connects to the button that will turn on/off the lights
int onButton_State = RELEASED;        // the current currentButtonState of the input pin for Lighting_On_Button
bool onButton_IsDebouncing = false;   // Used to track when we are in a debounce situation after the OnButton input toggles state
long onButton_LastDebounceTime = 0;   // the last time the output pin was toggled

// Mode Button
const int LIGHTING_MODE_BUTTON_PIN = 3; // (Active-Low) Input pin connects to the button that will change the mode of the lights
int modeButton_State = RELEASED;        // the current currentButtonState from the input pin for Lighting_Mode_Button
bool modeButton_IsDebouncing = false;   // Used to track when we are in a debounce situation after the OnButton input toggles state
long modeButton_LastDebounceTime = 0;   // the last time the output pin was toggled

// the following variables are long's because the time, measured in milliseconds,
// will quickly become a bigger number than can be stored in an int.
long debounceDelay = 50;    // the debounce time; increase if the output flickers


// OnBoard LED for testing buttons
const int TEST_LED_PIN = 13;


void setup() {

  //TODO:REMOVE once buttons work
  pinMode(TEST_LED_PIN, OUTPUT);
  Serial.begin(57600);

  // Initialize the lights as off
  FastLED.addLeds<WS2812, LED_DATA_PIN, GRB>(ledStrip, LED_COUNT);
  fill_solid(ledStrip, LED_COUNT, CRGB::Black);
  FastLED.show();

  // Initialize the buttons
  pinMode(LIGHTING_ON_BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(LIGHTING_ON_BUTTON_PIN), buttonPressChanged, FALLING);
  pinMode(LIGHTING_MODE_BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(LIGHTING_MODE_BUTTON_PIN), buttonPressChanged, FALLING);

}

void loop() {
  
  // Check the states of both of our buttons to determine if there have been any
  // button presses.
  int currentButtonState = RELEASED;

  // ---- Watch the main On Button for state changes -------------------------------------------
  currentButtonState = digitalRead(LIGHTING_ON_BUTTON_PIN);
  if (currentButtonState == onButton_State) {
    onButton_IsDebouncing = false;
  } else {
    if (onButton_IsDebouncing) {
      if ((millis() - onButton_LastDebounceTime) > debounceDelay) {
        // whatever the currentButtonState is at, it's been there for longer
        // than the debounce delay, so take it as the actual current state:
        onButton_State = currentButtonState;
        onButton_IsDebouncing = false;
        Serial.print("Debounce complete! Current state is: ");
        Serial.print(onButton_State == PRESSED ? "PRESSED\n" : "RELEASED\n");
        digitalWrite(TEST_LED_PIN, ((onButton_State == PRESSED) ? HIGH : LOW));

        // We are only interested in RELEASED --> PRESSED and not PRESSED --> RELEASED
        if (onButton_State == RELEASED){
          return;
        }

        Serial.print("valid button press: ");
        Serial.print(onButton_State);
        Serial.print("\n");
        // Handle the button press now
        Serial.print("handling button press\n");
        lightingMode = (lightingMode == Off ? FullOn : Off);
        changeLightingMode(lightingMode);
      } else {
        return;
      }
    } else {
      // reset the debouncing timer
      onButton_IsDebouncing = true;
      onButton_LastDebounceTime = millis();
      Serial.print("starting debounce\n");
      return;
    }
  }

  // ---- Watch the Mode Button for state changes -------------------------------------------
  currentButtonState = digitalRead(LIGHTING_MODE_BUTTON_PIN);
  if (currentButtonState == modeButton_State) {
    modeButton_IsDebouncing = false;
  } else {
    if (modeButton_IsDebouncing) {
      if ((millis() - modeButton_LastDebounceTime) > debounceDelay) {
        // whatever the currentButtonState is at, it's been there for longer
        // than the debounce delay, so take it as the actual current state:
        modeButton_State = currentButtonState;
        modeButton_IsDebouncing = false;
        Serial.print("Debounce complete! Current state is: ");
        Serial.print(modeButton_State == PRESSED ? "PRESSED\n" : "RELEASED\n");
        digitalWrite(TEST_LED_PIN, ((onButton_State == PRESSED) ? HIGH : LOW));

        // We are only interested in RELEASED --> PRESSED and not PRESSED --> RELEASED
        if (modeButton_State == RELEASED){
          return;
        }

        Serial.print("valid button press: ");
        Serial.print(modeButton_State);
        Serial.print("\n");
        // Handle the button press now
        digitalWrite(TEST_LED_PIN, ((modeButton_State == PRESSED) ? HIGH : LOW));
        Serial.print("handling button press\n");

        // ------------ ADD MAX MODE COUNT HERE TO ENABLE NEW MODES --------
        // zzModeButtonzz
        switch (lightingMode) {
          case Off:
            lightingMode = FullOn;
            break;
          case FullOn:
            lightingMode = CraftingOnly;
            break;
          case CraftingOnly:
            lightingMode = Party;
            break;
          case Party:
            lightingMode = DancingWater;
            break;
          case DancingWater:
            lightingMode = FullOn;
            break;
          Default:
            // This shouldn’t be possible. We have a problem..turn off
            lightingMode = Off;
            break;

        }

        changeLightingMode(lightingMode);
      } else {
        return;
      }
    } else {
      // reset the debouncing timer
      modeButton_IsDebouncing = true;
      modeButton_LastDebounceTime = millis();
      Serial.print("starting debounce\n");
      return;
    }
  }

}

void buttonPressChanged() {
  
}

// Puts the LED lighting into which ever lighting mode we should be in.
void changeLightingMode(LightingMode mode) {
  switch(mode) {
    case Off:
      Serial.print("Moving to the 'Off' mode");
      fill_solid(ledStrip, LED_COUNT, CRGB::Black);
      
      break;
    case FullOn:
      Serial.print("Moving to the 'FullOn' mode");
      fill_solid(ledStrip, LED_COUNT, CRGB::White);
      break;
    case CraftingOnly:
      //TODO: Determine real values for the room empirically on the lighting circuits are installed
      //TODO: This is important because the correct blocks of LED to change could be across several
      //TODO: Different ranges. Kicking this can down the road for testing purposes
      Serial.print("Moving to the 'CraftingOnly' mode");
      fill_solid(ledStrip, LED_COUNT, CRGB::Green);
      break;  
    case Party:
      Serial.print("Moving to the 'Party' mode");
      fill_solid(ledStrip, LED_COUNT, CRGB::Red);
      break;
    case DancingWater:
      Serial.print("Moving to the 'DancingWater' mode");
      fill_solid(ledStrip, LED_COUNT, CRGB::Blue);
      break;
    default:
      //This should never happen!!! If it does...flash a few times
      //TODO: Do something better than this. Kicking this can down the road
      Serial.print("WE SHOULD NOT BE HERE! OMG! WHAT HAPPENED!");
      fill_solid(ledStrip, LED_COUNT, CRGB::Purple);
  }

  FastLED.show();

}


