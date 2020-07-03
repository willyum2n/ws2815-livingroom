#include "FastLED.h"
#include <Bounce2.h>
#include "Rotary.h"

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
const int LED_STRIP_DATA_PIN = 6;    // This is the output pin that delivers Color data to the LED Strip
CRGB ledStrip[LED_COUNT];            // Storage for color data
LightingMode lightingMode = Off;     // We start with the lights off

// Rotary Encoder with OnBoard switch
const uint8_t RE_SW_PIN = 2;        // (Active-Low) Input pin connects to the SW pin on the Rotary Encoder that will turn on/off the lights
const uint8_t RE_DATA_PIN = 8;
const uint8_t RE_CLK_PIN = 9;
Bounce re_Switch_Debouncer = Bounce();  // SW on device
Rotary rotary = Rotary(RE_CLK_PIN, RE_DATA_PIN);

const uint8_t MIN_BRIGHTNESS = 16;
const uint8_t MAX_BRIGHTNESS = 255;
const uint8_t BRIGHTNESS_STEP = 16;
int currentBrightness = 255;          // Tracks the current value of the Rotary Encoder
int lastBrightness = 255;

// Mode Button (Monentary Switch)
const uint8_t MODE_BUTTON_PIN = 3; // (Active-Low) Input pin connects to the button that will change the mode of the lights
Bounce mode_Debouncer = Bounce();

// Animation Variables
int animationFramePeriod = 0;   // The length of time a frame should be shown until it is redrawn. 0 means to never redraw
long nextFrameTime = 0;         // Used to know when the animation frame should draw
int currentFrame = 0;
typedef void (*GeneralFunction) ();

GeneralFunction currentAnimation = NULL;


void setup() {
    Serial.begin(57600);

    // Initialize the lights as off
    FastLED.addLeds<WS2812, LED_STRIP_DATA_PIN, GRB>(ledStrip, LED_COUNT);
    changeLightingMode(lightingMode);

    // Initialize the Rotary Encoder
    re_Switch_Debouncer.attach(RE_SW_PIN,INPUT_PULLUP); // Attach the debouncer to a pin with INPUT_PULLUP mode
    re_Switch_Debouncer.interval(25); // Use a debounce interval of 25 milliseconds

    pinMode(RE_DATA_PIN, INPUT);
    pinMode(RE_CLK_PIN, INPUT);


    // Initialize the Momentary Switch
    mode_Debouncer.attach(MODE_BUTTON_PIN,INPUT_PULLUP); // Attach the debouncer to a pin with INPUT_PULLUP mode
    mode_Debouncer.interval(25); // Use a debounce interval of 25 milliseconds

}

void loop() {
  
    mode_Debouncer.update(); // Update the Bounce instance

    // ---- Watch the Rotary Encoder for changes ----------------------------------------------

    // Check the button for presses
    re_Switch_Debouncer.update(); // Update the Bounce instance
    if ( re_Switch_Debouncer.fell() ) {  // Button is active-low
        Serial.print("handling button press\n");
        lightingMode = (lightingMode == Off ? FullOn : Off);
        changeLightingMode(lightingMode);
    }

    // ---- Watch the Momentary Switch - Mode Button for state changes --------------------------
    // Check the button for presses
    mode_Debouncer.update(); // Update the Bounce instance

    //Serial.print("inputline state="); Serial.print(blah); Serial.print("duration="); Serial.print(mode_Debouncer.duration()); Serial.print("\n");
    if ( mode_Debouncer.fell() ) {  // Button is active-low
        // zzModeButtonzz
        switch (lightingMode) {
            case Off:
                lightingMode = FullOn;
                animationFramePeriod=1;
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
                Serial.print("This should never happen. lightingMode="); Serial.println(lightingMode);
                lightingMode = Off;
                break;
        }
        changeLightingMode(lightingMode);
    }

    // ---- Execute any animations for the current lighting mode --------------------------
    if (currentAnimation != NULL) {
        currentAnimation();
    }

    // ---- Watch Rotary Encoder for turns and adjust brightness --------------------------
    unsigned char result = rotary.process();
    switch(result) {
        case DIR_CW:
            currentBrightness += BRIGHTNESS_STEP;
            Serial.print("currentBrightness="); Serial.println(currentBrightness);
            break;
        case DIR_CCW:
            currentBrightness -= BRIGHTNESS_STEP;
            Serial.print("currentBrightness="); Serial.println(currentBrightness);
            break;
        case DIR_NONE:
            break;
    }
    if (currentBrightness < MIN_BRIGHTNESS) 
        currentBrightness = MIN_BRIGHTNESS;
    if (currentBrightness > MAX_BRIGHTNESS)
        currentBrightness = MAX_BRIGHTNESS;

    if (lastBrightness != currentBrightness) {
        Serial.print("currentBrightness="); Serial.println(currentBrightness);
        FastLED.setBrightness(currentBrightness);
        lastBrightness = currentBrightness;
        FastLED.show();
    }

}

// Puts the LED lighting into which ever lighting mode we should be in.
void changeLightingMode(LightingMode mode) {
    // Reset global variables
    currentAnimation = NULL;
    currentFrame = 0;
    nextFrameTime = 0;

    switch(mode) {
        case Off:
            Serial.println("Moving to the 'Off' mode");
            fill_solid(ledStrip, LED_COUNT, CRGB::Black);
            break;
        case FullOn:
            Serial.println("Moving to the 'FullOn' mode");
            fill_solid(ledStrip, LED_COUNT, CRGB(127,127,127));
            break;
        case CraftingOnly:
            //TODO: Determine real values for the room empirically on the lighting circuits are installed
            //TODO: This is important because the correct blocks of LED to change could be across several
            //TODO: Different ranges. Kicking this can down the road for testing purposes
            Serial.println("Moving to the 'CraftingOnly' mode");
            fill_solid(ledStrip, LED_COUNT, CRGB::Green);
            break;  
        case Party:
            Serial.println("Moving to the 'Party' mode");
            fill_solid(ledStrip, LED_COUNT, CRGB::Red);
            break;
        case DancingWater:
            Serial.println("Moving to the 'DancingWater' mode");
            initDancingWaterAnimation();
            currentAnimation = DancingWaterAnimation;
            break;
        default:
            //This should never happen!!! If it does...flash a few times
            //TODO: Do something better than this. Kicking this can down the road
            Serial.println("WE SHOULD NOT BE HERE! OMG! WHAT HAPPENED!");
            fill_solid(ledStrip, LED_COUNT, CRGB::Purple);
    }

    FastLED.show();
}

/* -------------------------------- Animation functions --------------------------------
 * These functions are more like enumerators. Each time they are called, they will draw
 * the next frame if the frame period has elapsed. This way we don't have to use delay()
 * to pause between frames. This keeps the main loop() pumping quickly so we can poll
 * for hardware changes in the buttons and Rotary Encoders
 * -----------------------------------------------------------------------------------*/


// ---------------- DancingWaterAnimation ----------------
int gr[9] = {50, 100, 150, 200, 255, 200, 150, 100, 50}; //highlights
void initDancingWaterAnimation() {
    int i, j;
    memset(ledStrip, 0, LED_COUNT * 3); //clear the display

    // Draw display
    for(i = 0 ; i < LED_COUNT; i++ ) {
        //fill each LED with blue
        ledStrip[i].b = 192;
        // Add some green highlights
        if (i % 20 == 0) {
            ledStrip[i+j].g =gr[j];                
        }
    }
}
void DancingWaterAnimation() {

    // Is it time to draw the next frame?
    if (nextFrameTime < millis()) {
        // TIME TO DRAW!!!!

        //rotate display
        ledStrip[LED_COUNT] = ledStrip[0];          // Copy first index of data to the end
        memcpy(ledStrip, ledStrip+1, LED_COUNT-1);  // Copy the entire array to itself but shift data by 1

        // Draw the display
        FastLED.show();

        // Set the timestamp for the next draw event
        nextFrameTime += animationFramePeriod;

    }
}
