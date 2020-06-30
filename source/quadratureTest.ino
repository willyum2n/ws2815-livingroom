/* This help the learner watch the 1/4 steps in a Rotary Encoder */

#include <Bounce2.h>
#include "Rotary.h"

// Our buttons are Active Low. So let’s define some helper words for not screwing that up
#define ROTARY_ACTIVE_LOW
#define PRESSED LOW
#define RELEASED HIGH

const uint8_t SW_PIN = 2;        // (Active-Low) Input pin connects to the SW pin on the Rotary Encoder that will turn on/off the lights
Bounce debouncer = Bounce();  // SW on device

const int RE_DATA_PIN = 8;
const int RE_CLK_PIN = 9;
Rotary rotary = Rotary(RE_CLK_PIN, RE_DATA_PIN);
int lastDataState = 0;
int lastClkState = 0;
 
int counter = 0;

void setup() { 
    Serial.begin(57600);

    debouncer.attach(SW_PIN,INPUT_PULLUP); // Attach the debouncer to a pin with INPUT_PULLUP mode
    debouncer.interval(25); // Use a debounce interval of 25 milliseconds

    pinMode (RE_DATA_PIN,INPUT);
    pinMode (RE_CLK_PIN,INPUT);
   
    // Reads the initial state of the Rotary Encoder
    lastDataState = digitalRead(RE_DATA_PIN);
    lastClkState = digitalRead(RE_CLK_PIN);
    Serial.print("[CLK,DATA] = ["); Serial.print(lastClkState); Serial.print(","); Serial.print(lastDataState); Serial.println("]");

} 

void loop() { 

    debouncer.update(); // Update the Bounce instance
    if (debouncer.fell()) {
        Serial.println("--------------------------------");
    }
  
    int currentDataState = digitalRead(RE_DATA_PIN);
    int currentClkState = digitalRead(RE_CLK_PIN);

    if (lastDataState != currentDataState || lastClkState != currentClkState) {
        lastDataState = currentDataState;
        lastClkState = currentClkState;
        Serial.print("[CLK,DATA] = ["); Serial.print(lastClkState); Serial.print(","); Serial.print(lastDataState); Serial.println("]");
    }

    unsigned char result = rotary.process();
    if (result != DIR_NONE) {
        Serial.println("********************************");
        if (result == DIR_CW) {
            counter++;
        } else if (result == DIR_CCW) {
            counter--;
        }
        Serial.print("Counter="); Serial.Println(coutner);
        Serial.println("********************************");
    }


 }