#include "FastLED.h"
#include <Bounce2.h>

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

// Variables for FastLED and LED Strip
const int LED_COUNT = 300;           // This is the total count of LEDs for the entire livingroom
CRGB ledStrip[LED_COUNT];            // Storage for color data
const int LED_STRIP_DATA_PIN = 6;    // This is the output pin that delivers Color data to the LED Strip
LightingMode lightingMode = Off;     // We start with the lights off

// Rotary Encoder with OnBoard switch
const uint8_t RE_SW_PIN = 2;        // (Active-Low) Input pin connects to the SW pin on the Rotary Encoder that will turn on/off the lights
const uint8_t RE_DATA_PIN = 8;
const uint8_t RE_CLK_PIN = 9;
const int RE_MIN = 10;
const int RE_MAX = 100;
const int RE_STEP = 5;
Bounce re_Switch_Debouncer = Bounce();  // SW on device
// Bounce re_Data_Debouncer = Bounce();    // DATA on device
int dataPinValue = RELEASED;
 Bounce re_Clock_Debouncer = Bounce();   // CLK on device
int clockPinValue = RELEASED;
uint8_t reValue = 100;          // Tracks the current value of the Rotary Encoder

// Mode Button
const uint8_t MODE_BUTTON_PIN = 3; // (Active-Low) Input pin connects to the button that will change the mode of the lights
//Bounce mode_Debouncer(MODE_BUTTON_PIN,INPUT_PULLUP);
Bounce mode_Debouncer = Bounce();


void setup() {
  Serial.begin(57600);

  // Initialize the lights as off
  FastLED.addLeds<WS2812, LED_STRIP_DATA_PIN, GRB>(ledStrip, LED_COUNT);
  changeLightingMode(lightingMode);
  FastLED.show();

  // Initialize the Rotary Encoder
  re_Switch_Debouncer.attach(RE_SW_PIN,INPUT_PULLUP); // Attach the debouncer to a pin with INPUT_PULLUP mode
  re_Switch_Debouncer.interval(25); // Use a debounce interval of 25 milliseconds
//  re_Data_Debouncer.attach(RE_DATA_PIN,INPUT_PULLUP); // Attach the debouncer to a pin with INPUT_PULLUP mode
//  re_Data_Debouncer.interval(50); // Use a debounce interval of 25 milliseconds
//  re_Data_Debouncer.update();
  re_Clock_Debouncer.attach(RE_CLK_PIN,INPUT_PULLUP); // Attach the debouncer to a pin with INPUT_PULLUP mode
  re_Clock_Debouncer.interval(5); // Use a debounce interval of 25 milliseconds
  re_Clock_Debouncer.update();
  pinMode(RE_DATA_PIN, INPUT);
  pinMode(RE_CLK_PIN, INPUT);


  // Initialize the Momentary Switch
  mode_Debouncer.attach(MODE_BUTTON_PIN,INPUT_PULLUP); // Attach the debouncer to a pin with INPUT_PULLUP mode
  mode_Debouncer.interval(25); // Use a debounce interval of 25 milliseconds


  // Serial.print("--------------  START  -------------\n");
  // Serial.print("Initial data pin value ="); Serial.print(re_Data_Debouncer.read()) ; Serial.print("\n");
  // Serial.print("Initial clock pin value="); Serial.print(re_Clock_Debouncer.read()); Serial.print("\n");
  // Serial.print("--------------  START  -------------\n");

  Serial.print("--------------  START  -------------\n");
  Serial.print("Initial Data Pin Value=");  Serial.print(digitalRead(RE_DATA_PIN)); Serial.print("\n");
  Serial.print("Initial Clock Pin Value="); Serial.print(digitalRead(RE_CLK_PIN));  Serial.print("\n");
  Serial.print("---------------  END  --------------\n");

}

void loop() {
  
  mode_Debouncer.update(); // Update the Bounce instance

  // ---- Watch the Rotary Encoder for changes ----------------------------------------------
  // Check the button for presses
  // re_Switch_Debouncer.update(); // Update the Bounce instance
  // if ( re_Switch_Debouncer.fell() ) {  // Button is active-low
  //   Serial.print("handling button press\n");
  //   lightingMode = (lightingMode == Off ? FullOn : Off);
  //   changeLightingMode(lightingMode);
  // }


  // Check the knob for turns
  // DATA and CLK lines on the encoder are really noisy too...debounce them
  // uint8_t preDateUpdate = re_Data_Debouncer.read();
  // uint8_t preClockUpdate = re_Clock_Debouncer.read();
  
  //bool dataChanged = re_Data_Debouncer.update();
  bool clockChanged = re_Clock_Debouncer.update();


 
  if ( clockChanged ) {
    dataPinValue = digitalRead(RE_DATA_PIN);
    clockPinValue = re_Clock_Debouncer.read();

    Serial.print("--------------  START  -------------\n");
    Serial.println("CLK line toggled");
    Serial.print("Data=");  Serial.println(dataPinValue);
    Serial.print("Clock="); Serial.println(clockPinValue);
    int reChange = read_rotary();
    if( reChange ) {
      //Serial.print("RE Changed="); Serial.print(reChange); Serial.print("\n");
      //Serial.print("RE old value="); Serial.print(reValue);  Serial.print("\n");
      reValue = reValue + (reChange * RE_STEP);
      if (reValue < RE_MIN)
        reValue = RE_MIN;
      if (reValue > RE_MAX)
        reValue = RE_MAX;
      Serial.print("reValue="); Serial.print(reValue); Serial.print("\n");
    }
    Serial.print("---------------  END  --------------\n");
  }
  


  // ---- Watch the Momentary Switch - Mode Button for state changes --------------------------
  // Check the button for presses
  // mode_Debouncer.update(); // Update the Bounce instance

  // //Serial.print("inputline state="); Serial.print(blah); Serial.print("duration="); Serial.print(mode_Debouncer.duration()); Serial.print("\n");
  // if ( mode_Debouncer.fell() ) {  // Button is active-low
  //   // zzModeButtonzz
  //   switch (lightingMode) {
  //     case Off:
  //       lightingMode = FullOn;
  //       break;
  //     case FullOn:
  //       lightingMode = CraftingOnly;
  //       break;
  //     case CraftingOnly:
  //       lightingMode = Party;
  //       break;
  //     case Party:
  //       lightingMode = DancingWater;
  //       break;
  //     case DancingWater:
  //       lightingMode = FullOn;
  //       break;
  //     Default:
  //       // This shouldn’t be possible. We have a problem..turn off
  //       Serial.print("OMFG! lightingMode is fucked! lightingMode="); Serial.print(lightingMode); Serial.print("\n");
  //       lightingMode = Off;
  //       break;
  //   }
  //   changeLightingMode(lightingMode);
  // }
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
static uint8_t prevNextCode = 0;
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
    if (dataPinValue) {
    // 0000 00XX --> 0000 XXDC  (The 'D' is now 1)
        prevNextCode |= 0x02; 
    }

    if (clockPinValue) {
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
      Serial.print("Moving to the 'Off' mode\n");
      fill_solid(ledStrip, LED_COUNT, CRGB::Black);
      
      break;
    case FullOn:
      Serial.print("Moving to the 'FullOn' mode\n");
      fill_solid(ledStrip, LED_COUNT, CRGB::White);
      break;
    case CraftingOnly:
      //TODO: Determine real values for the room empirically on the lighting circuits are installed
      //TODO: This is important because the correct blocks of LED to change could be across several
      //TODO: Different ranges. Kicking this can down the road for testing purposes
      Serial.print("Moving to the 'CraftingOnly' mode\n");
      fill_solid(ledStrip, LED_COUNT, CRGB::Green);
      break;  
    case Party:
      Serial.print("Moving to the 'Party' mode\n");
      fill_solid(ledStrip, LED_COUNT, CRGB::Red);
      break;
    case DancingWater:
      Serial.print("Moving to the 'DancingWater' mode\n");
      fill_solid(ledStrip, LED_COUNT, CRGB::Blue);
      break;
    default:
      //This should never happen!!! If it does...flash a few times
      //TODO: Do something better than this. Kicking this can down the road
      Serial.print("WE SHOULD NOT BE HERE! OMG! WHAT HAPPENED!\n");
      fill_solid(ledStrip, LED_COUNT, CRGB::Purple);
  }

  FastLED.show();

}
