#include <Keypad.h>

const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
//define the keypad symbols
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {11, 12, 13, 14}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {7, 8, 9, 10}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup() {

  Serial.begin(9600);
  //keypad.addEventListener(keypadEvent);   // add an event listener
  //keypad.setDebounceTime(250);            // Default is 50 ms
  //keypad.setHoldTime(500);                // default is 1000 ms
  
}

void loop() {
  char key = keypad.getKey(); // non-blocking
  // try using: keypad.waitForKey(); instead (blocking)

  if (key != NO_KEY) {
    Serial.print(key);
  }
}
