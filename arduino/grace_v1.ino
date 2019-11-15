int count = 1;
int outputPins[1] = {3};
int inputPins[1] = {2};
int brightnesses[1]; // keep track of pin brightness
int inputVals[1];
const int initialBrightness = 25;
const int maxBrightness = 255;
const int addBrightness = 10;

void setup() {
  Serial.begin(9600);
  
  for (byte i = 0; i <= count; i += 1) {
    // 1. set default brightness
    brightnesses[i] = initialBrightness;
    // 2. register output pins
    pinMode(outputPins[i], OUTPUT);
    // 3. register input pins
    pinMode(inputPins[i], INPUT);
    // 4. set all input values to default 0
    inputVals[i] = 0;
  }
}

void loop() {
  readSwitches();
  writeLights();
  delay(100);
}

void writeLights() {
  for (byte i = 0; i < count; i += 1) {
    if (inputVals[i]) {
       // if being pressed
      analogWrite(outputPins[i], maxBrightness);
    } else {
      analogWrite(outputPins[i], brightnesses[i]);
    }
  }
}

void readSwitches() {
  for (byte i = 0; i < count; i += 1) {
    // READ switch states
    int value = digitalRead(inputPins[i]);

    // if on, make sure it was off before
    if (value && !inputVals[i]) {
      // then add brightness
      brightnesses[i] += addBrightness;
      // make sure it fits in the range of LED output
      brightnesses[i] = constrain(brightnesses[i], initialBrightness, maxBrightness - initialBrightness);
      Serial.println(brightnesses[i]);
    }
    // and then remember the value
    inputVals[i] = value;
  }
}
