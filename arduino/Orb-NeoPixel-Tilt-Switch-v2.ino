#include <Adafruit_NeoPixel.h>

// use #define for read-only integers to save memory
#define numOrbs 3 // UPDATE to number of orbs
#define numGroups 3 // UPDATE to number of groups/categories
#define maxOrbsPerGroup 5 // UPDATE
#define initialBrightness 10
#define maxBrightness 255
#define addBrightness 1
#define debounce 500 // if switch reading is unstable, only keep the reading if it stays constant for >500 ms
#define duration 1000 // fade animation duration
#define stagger 500
// read-only arrays
const int outputPins[numOrbs] = {3, 5, 6}; // array of pin numbers for LED/neopixels // ASSUMES FIRST IS GLOWING INSTRUCTION ORB
const int inputPins[numOrbs] = {11, 10, 9}; // array of pin numbers for tilt switches // ASSUMES FIRST IS GLOWING INSTRUCTION ORB
const int groups[numGroups][maxOrbsPerGroup] = {
  {0}, // FIRST GROUP IS ONLY GLOWING INSTRUCTION ORB DO NOT ADD ANY OTHER ORBS
  {1}, // group one with index of orbs that belong to it
  {2}
};
int orbToGroup[numOrbs][2]; // this array will be created in setup so we can look up what group an orb belongs to, and what index within that group

// arrays that will be modified as the installation goes on
unsigned long currentTime = 0; // like int, but allows for higher value memory // ask someone who is good with Arduino // whether we need to reset over time
Adafruit_NeoPixel neopixels[numOrbs];
int neutralBrightnesses[numOrbs]; // keep track of pin brightness
unsigned long switchStates[numOrbs][3]; // [value, prevTime, prevDebounceValue]
unsigned long lightStates[numOrbs][3]; // [startBrightness, endBrightness, startTime]
unsigned long groupStates[numGroups][3]; // keep track of which group is getting lit up: [0 or 1, startTime, activeOrb]

void setup() {
  Serial.begin(9600);
  
  for (byte i = 0; i < numOrbs; i += 1) {
    // 1. set default brightness
    neutralBrightnesses[i] = initialBrightness;
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

  int orb = 0;
  currentTime = millis();
  for (byte i = 0; i < numGroups; i += 1) {
    for (byte j = 0; j < maxOrbsPerGroup; j += 1) {
      orb = groups[i][j];
      if (j == 0 || orb) { // HACKY but please don't touch ;___;
        orbToGroup[orb][0] = i; // remember group number
        orbToGroup[orb][1] = j; // remember orb index
      }
    }
  }
}

void loop() {
  currentTime = millis();

  if (currentTime > 0) {
    readSwitches();
    calculateBrightnesses();
    writeLights();
  }
}

void readSwitches() {
  for (byte i = 0; i < numOrbs; i += 1) {
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
    if ((currentTime - prevTime) > debounce && value != prevValue) {
      // this is a stable reading, so we can trust it

      if (!value) {
        // if it just got turned off, add brightness
        neutralBrightnesses[i] += addBrightness;
        // make sure it fits in the range of LED output
        neutralBrightnesses[i] = constrain(neutralBrightnesses[i], initialBrightness, maxBrightness * 0.8); 
      }

      int group = orbToGroup[i][0];
      groupStates[group][0] = value;
      groupStates[group][1] = currentTime;
      groupStates[group][2] = orbToGroup[i][1]; // orb index
      // (if orb is being turned off there's an edge case)
        
      // if value has changed, update orb state & time 
      switchStates[i][0] = value;
      switchStates[i][1] = currentTime;
    }

    // set prevDebounceValue to the reading
    switchStates[i][2] = value;
  }
}

void calculateBrightnesses() {
  int orb = 0;
  for (byte i = 0; i < numGroups; i += 1) {
    if (groupStates[i][1] == currentTime) {
      // if group was just updated
      if (i == 0) {
        // and it's the first group, that's the instructional orb
        orb = 0;
        if (groupStates[i][0]) { // if instructional orb is turned on
          lightStates[orb][0] = instructionalOrbBrightness(groupStates[i][1]); // calculate as function of time
          lightStates[orb][1] = maxBrightness; // end brightness
        } else { // if instructional orb is turned off
          lightStates[orb][0] = maxBrightness;
          lightStates[orb][1] = instructionalOrbBrightness(groupStates[i][1] + duration);
        }
        lightStates[orb][2] = groupStates[i][1]; // group start time
      } else {
        for (byte j = 0; j < maxOrbsPerGroup; j += 1) {
          orb = groups[i][j];
          if (j == 0 || orb) { // make sure orb exists (HACKY but please don't touch ;___; )
            if (groupStates[i][0]) { // if group is turned on
              lightStates[orb][0] = neutralBrightnesses[orb]; // start brightness
              lightStates[orb][1] = maxBrightness; // end brightness
            } else { // if group is turned off
              lightStates[orb][0] = maxBrightness;
              lightStates[orb][1] = neutralBrightnesses[orb];
            }
  
            int indexDiff = groupStates[i][2]- j;
            // start time + stagger * distance
            lightStates[orb][2] = groupStates[i][1] + stagger * abs(indexDiff); // start time
          }
        }
      }
    }
  }
}

void writeLights() {
  int brightness;
  for (byte i = 0; i < numOrbs; i += 1) {
    if (lightStates[i][2] <= currentTime && currentTime - lightStates[i][2] <= duration) {
      // if current time is after orb's start time and also within the duration of fade animation
      // figure out the brightness the orb should be at
      brightness = map(currentTime - lightStates[i][2], 0, duration, lightStates[i][0], lightStates[i][1]);
      neopixels[i].setPixelColor(0, brightness, brightness, brightness);
      neopixels[i].show();
    } else if (i == 0 && !switchStates[i][0]) {
      // if glowing instructional orb isn't lifted up, make it pulse as function of time
      brightness = instructionalOrbBrightness(currentTime);
      neopixels[i].setPixelColor(0, brightness, brightness, brightness);
      neopixels[i].show();
    }
  }
}

int instructionalOrbBrightness(unsigned long time) {
  return (sin(time * 0.003) + 1) / 2 * (maxBrightness * 0.75) + 10;
}
