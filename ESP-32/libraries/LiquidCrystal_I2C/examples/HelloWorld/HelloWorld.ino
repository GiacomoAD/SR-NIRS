//SmrazaKeen
//Compatible with the Arduino IDE 1.6.6-1.6.9
//Library version:1.2
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x3F,20,4);  // set the LCD address to 0x3F for a 20 chars and 4 line display

void setup()
{
  lcd.init();                      // initialize the lcd   
  lcd.backlight();
  lcd.setCursor(1,0);
  lcd.print("Welcome to Smraza!");  
  lcd.setCursor(0,1);
  lcd.print("Arduino LCM IIC 2004");
  lcd.setCursor(0,2);
  lcd.print("www.amazon.com/shops");
  lcd.setCursor(0,3);
  lcd.print("/smraza");
}



void loop()
{
}
