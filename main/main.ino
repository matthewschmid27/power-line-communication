/*
 =================================
 LCD pin              Connect to
 ---------------------------------
 01 - GND             GND, pot
 02 - VCC             +5V, pot
 03 - Contrast        Pot wiper
 04 - RS              Pin8 (P2.0)
 05 - R/W             GND
 06 - EN              Pin9 (P2.1)
 07 - DB0             GND
 08 - DB1             GND
 09 - DB2             GND
 10 - DB3             GND
 11 - DB4             Pin10 (P2.2)
 12 - DB5             Pin11 (P2.3)
 13 - DB6             Pin12 (P2.4)
 14 - DB7             Pin13 (P2.5)
 15 - BL+             +5V
 16 - BL-             GND
 =================================

ASCII Table
#     35      00100011
*     42      00101010
0     48      00110000
1     49      00110001
2     50      00110010
3     51      00110011
4     52      00110100
5     53      00110101
6     54      00110110
7     55      00110111
8     56      00111000
9     57      00111001
A     65      01000001
B     66      01000010
C     67      01000011
D     68      01000100
 
 */

#include <LiquidCrystal.h>
#include <Keypad.h>

// https://playground.arduino.cc/Code/Keypad/#CurrentVersion
// see: char waitForKey()

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(P2_0, P2_1, P2_2, P2_3, P2_4, P2_5);

const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
//define the cymbols on the buttons of the keypads
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
//byte rowPins[ROWS] = {11, 12, 13, 14}; //connect to the row pinouts of the keypad
//byte colPins[COLS] = {7, 8, 9, 10}; //connect to the column pinouts of the keypad
//byte rowPins[ROWS] = {5, 4, 3, 2}; //connect to the row pinouts of the keypad
//byte colPins[COLS] = {9, 8, 7, 6}; //connect to the column pinouts of the keypad  

byte rowPins[ROWS] = {P1_0, P1_1, P1_2, P1_3};
byte colPins[COLS] = {P1_4, P1_5, P1_6, P1_7};

//initialize an instance of class NewKeypad
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS); // change hexaKeys to keys?


void setup() {

  Serial.begin(115200); // sett ST7540 datasheet
  
  // set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);
  //lcd.cursor(); // // turn on the cursor (lcd.noCursor();)
  //lcd.noBlink(); // turn off the blinking cursor (lcd.blink();)
  //lcd.autoscroll();   // turn on automatic scrolling (lcd.NoAutoscroll();)
  //lcd.setCursor(0, 0);
  // Print a message to the LCD.
  lcd.print("Braydon ");

  char variable = B01000100;
  lcd.print(variable);
//  Serial.println(variable); // NOT WORKING

}

void loop() {
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(0, 1);
  // print the number of seconds since reset:
//  lcd.print(millis()/1000);

  char key = keypad.getKey();
  if (key){ // use waitForKey() instead?????
    lcd.print(key);
    Serial.println(key);
  }


  //Serial.println(millis()/1000);
  
  //lcd.clear();

  
}
