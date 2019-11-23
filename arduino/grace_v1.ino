#include <Adafruit_NeoPixel.h>

// use #define for read-only integers to save memory
#define numOrbs 5 // update to number of orbs
#define numGroups 2 // update to number of groups/categories
#define initialBrightness 10
#define maxBrightness 255
#define addBrightness 10
#define debounce 500 // if switch reading is unstable, only keep the reading if it stays constant for >50 secs
// read-only arrays
const int outputPins[numOrbs] = {3, 5, 6, 9, 10}; // array of pin numbers for LED/neopixels
const int inputPins[numOrbs] = {2, 11}; // array of pin numbers for tilt switches
const int orbToGroup[numOrbs][2] = { // for each orb, the group it belongs to and the index of that orb within the group
  {0, 1}, // so this orb belongs to group 0, and is the first orb in group 0
  {1, 1},
  {0, 2}, // and this one is group 0, and second orb in group 0
  {1, 2}, // I'm keeping track of the index so that we can stagger the fade animation
  {0, 3}
};

// arrays that will be modified as the installation goes on
unsigned long currentTime = 0;
Adafruit_NeoPixel neopixels[numOrbs];
unsigned long switchStates[numOrbs][3]; // keep track of which orb is being interacted with: [0 or 1, millis, debounce 0 or 1]
int lightBrightnesses[numOrbs]; // keep track of pin brightness
unsigned long groupStates[numGroups][2]; // keep track of which group is getting lit up: [0 or 1, millis]

void setup() {
  Serial.begin(9600);
  
  for (byte i = 0; i < numOrbs; i += 1) {
    // 1. set default brightness
    lightBrightnesses[i] = initialBrightness;
    // 2. register output pins
    neopixels[i] = Adafruit_NeoPixel(1, outputPins[i]);
    neopixels[i].begin();
    // 3. register input pins
    pinMode(inputPins[i], INPUT_PULLUP);
    // 4. set all input values to default 0
    switchStates[i][0] = 0;
    switchStates[i][1] = currentTime;
    switchStates[i][2] = 0;
  }
}

void loop() {
  currentTime = millis();
  readSwitches();
  writeLights();
  delay(100);
}

void readSwitches() {  
  // TODO: CHANGE BACK TO numOrbs
  for (byte i = 0; i < 2; i += 1) {
    // READ switch states
    int value = !digitalRead(inputPins[i]);
    int prevValue = switchStates[i][0];
    int prevTime = switchStates[i][1];
    int prevDebounceValue = switchStates[i][2];

    if (value != prevDebounceValue) {
      // if the value has changed for orb, remember current time 
      switchStates[i][1] = currentTime;
    }
    // DEBOUNCE
    if ((currentTime - prevTime) > debounce) {
      // this is a stable reading, so we can trust it
      if (value != prevValue) {
        // if it was previously turned off and is now on:
        if (value && !prevValue) {
          // then add brightness
          lightBrightnesses[i] += addBrightness;
          // make sure it fits in the range of LED output
          lightBrightnesses[i] = constrain(lightBrightnesses[i], initialBrightness, maxBrightness - initialBrightness);
        }

        // if value has changed, update orb state & time 
        switchStates[i][0] = value;
        switchStates[i][1] = currentTime;
      }
    }
    
    // also update its group
    int group = orbToGroup[i][0];
    int groupValue = groupStates[group][0];
    if (value != groupValue) {
      groupStates[group][0] = value;
      groupStates[group][1] = currentTime;
    }

    // set prevDebounceValue to the reading
    switchStates[i][2] = value;
  }
}

void writeLights() {
  for (byte i = 0; i < numOrbs; i += 1) {
    int value = switchStates[i][0];
    int group = orbToGroup[i][0];
    int groupValue = groupStates[group][0];
    
    if (value) {
       // if being pressed
      neopixels[i].setPixelColor(0, maxBrightness, maxBrightness, maxBrightness);
    } else if (groupValue) {
      // if group is being lit up
      neopixels[i].setPixelColor(0, maxBrightness, maxBrightness, maxBrightness);
    } else {
      neopixels[i].setPixelColor(0, lightBrightnesses[i], lightBrightnesses[i], lightBrightnesses[i]);
    }
    neopixels[i].show();
  }
}
