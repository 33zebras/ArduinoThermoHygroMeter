#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <Keypad.h>
short interval = 1000;
bool configMode = false;
bool fahrenMode = false;
byte cScaleLo = 0;
byte cScaleHi = 40;
byte fScaleLo = 32;
byte fScaleHi = 104;
const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
//define the cymbols on the buttons of the keypads
char hexaKeys[ROWS][COLS] = {
  {'0','1','2','3'},
  {'4','5','6','7'},
  {'8','9','A','B'},
  {'C','D','E','F'}
};
byte rowPins[ROWS] = {9, 8, 7, 6}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {10, 11, 12, 13}; //connect to the column pinouts of the keypad
//initialize an instance of class NewKeypad
Keypad pad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

 
DHT dht(A2, 11);
LiquidCrystal_I2C lcd(0x27, 20, 4); // set the LCD address to 0x27 for a 16 chars and 2 line display
uint8_t degree[8]  = {0x8, 0x14, 0x8};
uint8_t bar20[8]  = {0, 0x10, 0x10};
uint8_t bar40[8]  = {0, 0x18, 0x18};
uint8_t bar60[8]  = {0, 0x1C, 0x1C};
uint8_t bar80[8]  = {0, 0x1E, 0x1E};
uint8_t bar100[8]  = {0, 0x1F, 0x1F};
uint8_t arrow[8]  = {0x3, 0x7, 0xF, 0x1F, 0x1F, 0xF, 0x7, 0x3};
unsigned long prevCounter = 0;
byte _cursor = 0;

void setup()
{
  lcd.createChar(0, degree);
  lcd.createChar(1, bar20);
  lcd.createChar(2, bar40);
  lcd.createChar(3, bar60);
  lcd.createChar(4, bar80);
  lcd.createChar(5, bar100);
  lcd.createChar(6, arrow);
  
  lcd.init();                      // initialize the lcd
  lcd.backlight();

  dht.begin();
}

void loop()
{
  char key = pad.getKey();
  if(key){
    if(configMode)
    {
      switch(key)
      {
        case '2' : 
          if(_cursor == 0) _cursor = 2;
          else _cursor--; break;
        case '6' :
          if(_cursor == 2) _cursor = 0;
          else _cursor++; break;
        case '5' :
          switch(_cursor)
          {
            case 0 : fahrenMode = !fahrenMode; break;
            case 1 : if(interval <= 500) interval = 500;
                     else interval -= 100; break;
            case 2 : configMode = false; break;
          } break;
        case '7' :
          switch(_cursor)
          {
            case 0 : fahrenMode = !fahrenMode; break;
            case 1 : if(2000 <= interval) interval = 2000;
                     else interval += 100; break;
            case 2 : configMode = false; break;
          } break;
      }
    }
    else
    {
      configMode = true;
      _cursor = 0;
    }
    lcd.clear();
  }

  
  if(configMode)
  {
    lcd.setCursor(0, 0);
    lcd.print("====== CONFIG ======");
    lcd.setCursor(0, 1);
    lcd.print("Unit:");
    lcd.setCursor(5, 1);
    lcd.print(fahrenMode?"Fahrenheit(":"   Celcius(");
    lcd.print((char)0);
    lcd.print(fahrenMode?"F)":"C)");
    lcd.print(_cursor == 0?(char)6:' ');
    lcd.setCursor(0, 2);
    lcd.print("Refresh Rate:");
    lcd.setCursor(15, 2);
    lcd.print(interval);
    lcd.print(_cursor == 1?(char)6:' ');
    lcd.setCursor(7, 3);
    lcd.print("BACK");
    lcd.print(_cursor == 2?(char)6:' ');
  }
  else
  {
    if(millis() - prevCounter > interval)
    {
      char h = dht.readHumidity();
      // Read temperature as Celsius (the default)
      float t = dht.readTemperature(fahrenMode);
      // Check if any reads failed and exit early (to try again).
      if (isnan(h) || isnan(t)) {
        lcd.print(F("Failed to read from DHT sensor!"));
        return;
      }

      
      lcd.setCursor(0, 0);
      lcd.print("Temperature:");
      lcd.setCursor(13, 0);
      lcd.print(t);
      lcd.print((char)0);
      lcd.print(fahrenMode?'F':'C');

      byte bars = (byte)(fahrenMode?1.42*(t-32):2.5*t);
      lcd.setCursor(0, 1); //0~40 C and 32~102
      for(char i = 0; i < 20; i++)
      {
        if(5 <= bars){ lcd.print((char)5); bars -= 5; }
        else if(0 < bars){ lcd.print((char)bars); bars = 0; }
        else { lcd.print(' '); }
      }



      lcd.setCursor(0, 2);
      lcd.print("Humidity:");
      lcd.setCursor(16, 2);
      lcd.print(h, DEC);
      lcd.print('%');
      
      lcd.setCursor(0, 3);
      for(char i = 0; i < 20; i++)
      {
        if(5 <= h){ lcd.print((char)5); h -= 5; }
        else if(0 < h){ lcd.print(h); h = 0; }
        else lcd.print(' ');
      }

      prevCounter = millis();
    }
  }
}
