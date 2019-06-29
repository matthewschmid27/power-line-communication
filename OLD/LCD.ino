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
#     35
*     42
0     48
1     49
2     50
3     51
4     52
5     53
6     54
7     55
8     56
9     57
A     65      01000001
B     66      01000010
C     67      01000011
D     68      01000100
 
 */

#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(P2_0, P2_1, P2_2, P2_3, P2_4, P2_5);

void setup() {

  Serial.begin(115200);
  
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
  Serial.println(variable);

}

void loop() {
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(0, 1);
  // print the number of seconds since reset:
  lcd.print(millis()/1000);

  Serial.println(millis()/1000);
  
  //lcd.clear();

  
}
