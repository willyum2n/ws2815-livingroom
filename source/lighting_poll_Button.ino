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

// Rotary Encoder with OnBoard switch
const int RE_SW_PIN = 2;        // (Active-Low) Input pin connects to the SW pin on the Rotary Encoder that will turn on/off the lights
int sw_State = RELEASED;        // the current currentButtonState of the input pin for Lighting_On_Button
bool sw_IsDebouncing = false;   // Used to track when we are in a debounce situation after the OnButton input toggles state
long sw_LastDebounceTime = 0;   // the last time the output pin was toggled
uint8_t reValue = 100;          // Tracks the current value of the Rotary Encoder
const int RE_MIN = 10;
const int RE_MAX = 100;
const int RE_STEP = 5;

// Mode Button
const int LIGHTING_MODE_BUTTON_PIN = 3; // (Active-Low) Input pin connects to the button that will change the mode of the lights
int modeButton_State = RELEASED;        // the current currentButtonState from the input pin for Lighting_Mode_Button
bool modeButton_IsDebouncing = false;   // Used to track when we are in a debounce situation after the OnButton input toggles state
long modeButton_LastDebounceTime = 0;   // the last time the output pin was toggled

// the following variables are long's because the time, measured in milliseconds,
// will quickly become a bigger number than can be stored in an int.
long debounceDelay = 50;    // the debounce time; increase if the output flickers

const int RE_DATA_PIN = 8;
const int RE_CLK_PIN = 9;


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

  // Initialize the Rotary Encoder
  pinMode(RE_SW_PIN, INPUT);        //TODO: I think the Rotary Encoder has a pullup resistor on board...check this
  pinMode(RE_CLK_PIN, INPUT_PULLUP);
  pinMode(RE_DATA_PIN, INPUT_PULLUP);

  // Initialize the Momentary Switch
  pinMode(LIGHTING_MODE_BUTTON_PIN, INPUT_PULLUP);

}

void loop() {
  
  // Check the states of both of our buttons to determine if there have been any
  // button presses.
  int currentButtonState = RELEASED;

  // ---- Watch the main On Button for state changes -------------------------------------------
  currentButtonState = digitalRead(RE_SW_PIN);
  if (currentButtonState == sw_State) {
    sw_IsDebouncing = false;
  } else {
    if (sw_IsDebouncing) {
      if ((millis() - sw_LastDebounceTime) > debounceDelay) {
        // whatever the currentButtonState is at, it's been there for longer
        // than the debounce delay, so take it as the actual current state:
        sw_State = currentButtonState;
        sw_IsDebouncing = false;
        Serial.print("Debounce complete! Current state is: ");
        Serial.print(sw_State == PRESSED ? "PRESSED\n" : "RELEASED\n");
        digitalWrite(TEST_LED_PIN, ((sw_State == PRESSED) ? HIGH : LOW));

        // We are only interested in RELEASED --> PRESSED and not PRESSED --> RELEASED
        if (sw_State == RELEASED){
          return;
        }

        Serial.print("valid button press: ");
        Serial.print(sw_State);
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
      sw_IsDebouncing = true;
      sw_LastDebounceTime = millis();
      Serial.print("starting debounce\n");
      return;
    }
  }

  // ---- Watch the Rotary Encoder for changes ----------------------------------------------
  int reChange = read_rotary();
  if( reChange ) {
    Serial.printf("RE Changed=%d\n", reChange);
    Serial.printf("RE old value = %d\n", reValue);
    revalue = revalue + (reChange * RE_STEP);
    if (reValue < RE_MIN )
      revalue = RE_MIN;
    if (reValue > RE_MAX)
      revalue = RE_MAX;
    serial.printf("RE new value = %d\n", reValue);
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
        digitalWrite(TEST_LED_PIN, ((sw_State == PRESSED) ? HIGH : LOW));

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

// A vald CW or CCW move returns 1, invalid returns 0.
//  CW = ClockWise
//  CCW = Counter Clockwise
//  -----------------------------------------------------------
//  | PSNS (Prev State, Next State) | Valid code | Direction  |
//  |                          0000 |     X      |     X      |
//  |                          0001 |  Valid     |     CW     |
//  |                          0010 |  Valid     |     CCW    |
//  |                          0011 |     X      |     X      |
//  |                          0100 |  Valid     |     CCW    |
//  |                          0101 |     X      |     X      |
//  |                          0110 |     X      |     X      |
//  |                          0111 |  Valid     |     CW     |
//  |                          1000 |  Valid     |     CW     |
//  |                          1001 |     X      |     X      |
//  |                          1010 |     X      |     X      |
//  |                          1011 |  Valid     |     CCW    |
//  |                          1100 |     X      |     X      |
//  |                          1101 |  Valid     |     CCW    |
//  |                          1110 |  Valid     |     CW     |
//  |                          1111 |     X      |     X      |
//  -----------------------------------------------------------
static int8_t INV = 0;
static int8_t CW = 1;
static int8_t CCW = -1;
static int8_t rot_enc_table[] = {0,CW,CCW,0,CCW,0,0,CW,CW,0,0,CCW,0,CCW,CW,0};

// Reads the Rotary Encoder and deterines if it's current state compared to it's last state
// is a valid transition. This is needed because rotary encoders are really noisy devices
// It returns the direction the encoder is turning as 1 for Clock Wise ror -1 for Coutner Clock Wise.
// If the encoder returned a bad value because of noise or the knob has not moved...just throw out 
// the value by returning a 0.
int8_t read_rotary() {
    // Shift the 2 LSB up to move them into the "previous" position
    // 0000 00XX --> 0000 XX00  ("XX" is the last value ). D and C values are 0
    prevNextCode <<= 2;

    // Read in both inputs and put the bits in the correct part of the "next" position
    if (digitalRead(DATA)) {
    // 0000 00XX --> 0000 XXDC  (The 'D' is now 1)
        prevNextCode |= 0x02; 
    }

    if (digitalRead(CLK)) {
    // 0000 00XX --> 0000 XXDC  (The 'C' is now 1)
        prevNextCode |= 0x01;  
    }
  
    // This clears the top nibble (Because the previous previous value will be here because of the 2 it shift)
    prevNextCode &= 0x0f;
    return ( rot_enc_table[( prevNextCode & 0x0f )]);
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


