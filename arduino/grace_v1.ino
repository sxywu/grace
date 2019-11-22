#include <Adafruit_NeoPixel.h>

int count = 5;
int outputPins[5] = {3, 5, 6, 9, 10}; // array of pin numbers for LED/neopixels
Adafruit_NeoPixel neopixels[5];
int brightnesses[5]; // keep track of pin brightness
int inputPins[5] = {2}; // array of pin numbers for tilt switches
int inputVals[5]; // keep track of which orb is being interacted with
const int initialBrightness = 0;
const int maxBrightness = 255;
const int addBrightness = 10;

void setup() {
  Serial.begin(9600);
  Serial.println("setup");
  while (!Serial);
    
  for (byte i = 0; i < count; i += 1) {
    // 1. set default brightness
    brightnesses[i] = initialBrightness;
    // 2. register output pins
    neopixels[i] = Adafruit_NeoPixel(1, outputPins[i]);
    neopixels[i].begin();
    Serial.println(neopixels[i].getPin());
//    // 3. register input pins
//    pinMode(inputPins[i], INPUT);
//    // 4. set all input values to default 0
//    inputVals[i] = 0;
  }
}

void loop() {
//  readSwitches();
  writeLights();
  delay(100);
}

void writeLights() {
  for (byte i = 0; i < count; i += 1) {
    if (inputVals[i]) {
       // if being pressed
      neopixels[i].setPixelColor(0, brightnesses[i], brightnesses[i], brightnesses[i]);
    } else {
      neopixels[i].setPixelColor(0, brightnesses[i], brightnesses[i], brightnesses[i]);
    }
    neopixels[i].show();
  }
}
//
//void readSwitches() {
//  for (byte i = 0; i < count; i += 1) {
//    // READ switch states
//    int value = digitalRead(inputPins[i]);
//
//    // if on, make sure it was off before
//    if (value && !inputVals[i]) {
//      // then add brightness
//      brightnesses[i] += addBrightness;
//      // make sure it fits in the range of LED output
//      brightnesses[i] = constrain(brightnesses[i], initialBrightness, maxBrightness - initialBrightness);
//      Serial.println(brightnesses[i]);
//    }
//    // and then remember the value
//    inputVals[i] = value;
//  }
//}
