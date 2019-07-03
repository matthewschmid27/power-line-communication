/*
 ==================================
 LCD pin              Connect to
 ----------------------------------
 01 - GND             GND, pot
 02 - VCC             +5V, pot
 03 - Contrast        Pot wiper
 04 - RS              P2.0
 05 - R/W             GND
 06 - EN              P2.1
 07 - DB0             GND
 08 - DB1             GND
 09 - DB2             GND
 10 - DB3             GND
 11 - DB4             P2.2
 12 - DB5             P2.3
 13 - DB6             P2.4
 14 - DB7             P2.5
 15 - BL+             +5V
 16 - BL-             GND
 ==================================

=======================
Key  ASCII   Binary
-----------------------
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
=======================
 */

#include <LiquidCrystal.h>

// initialize lcd interface pins
LiquidCrystal lcd(P2_0, P2_1, P2_2, P2_3, P2_4, P2_5);

void setup() {
  
  lcd.begin(16, 2); // col, row
  
  lcd.print("Big Brother 0_0");
  delay(2000);
  lcd.clear();

}

void loop() {

  char keys[16] = {B00110000, '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', '*', '#'};
  
  lcd.setCursor(0, 0);
  
  for (int i = 0; i < 16; i++){
    lcd.print(keys[i]);
    delay(500);
  }

  int j = 15;
  for (int i = 0; i < 16; i++){
    lcd.setCursor(j, 1);
    lcd.print(keys[i]);
    delay(500);
    j--;
  }
  
  delay(2000);
  lcd.clear();
  
}


// For final project:

// " Big Brother #1 "
// " You Pressed: X "

// " Big Brother #2 "
// " You Pressed: X " 

// also have the screens say "system booting.." or some shit when everything is first plugged in?

// lcd.autoScroll();
// lcd.scrollDisplayRight();
// lcd.rightToLeft();   could have used to reverse text print direction
// lcd.display; lcd.noDisplay();  blank display without losing contents
