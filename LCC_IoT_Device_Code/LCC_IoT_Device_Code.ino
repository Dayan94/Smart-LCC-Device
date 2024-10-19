//#include <pcmConfig.h>
//#include <pcmRF.h>
#include <avr/pgmspace.h>
#include<LiquidCrystal.h>
#include<SD.h>
#include<TMRpcm.h>   // SD card libraries are included in TMRpcm library, that's why no need to include further
#include <SPI.h>
#include<SoftwareSerial.h> // software serial connection for gsm sim900

LiquidCrystal lcd(A5, 8, 7, 6, 5, 4);
TMRpcm tmrpcm;
SoftwareSerial gprsSerial(2, 3); // TX of gsm will go digital 2 pin of arduino and RX of gsm will digital 3 pin of arduino

byte lcdbuffer[8], n_chr = 0, i = 0; // lcdbuffer for progmem data in for pickup by lcd.create and n_chr is used to store number of character in each bangla word

const byte sgtm[][8]  PROGMEM =
{
  { // 0 for donto sho bo
    B11111,
    B01001,
    B00101,
    B00111,
    B01011,
    B10101,
    B00101,
    B00111
  },
  { // 1 for aa kar
    B00000,
    B11111,
    B00100,
    B00100,
    B00100,
    B00100,
    B00100,
    B00100
  },
  { // 2 for go
    B00000,
    B01101,
    B10011,
    B10001,
    B01001,
    B01001,
    B10001,
    B00001
  },
  { // 3 for to
    B00000,
    B11111,
    B00110,
    B01001,
    B00001,
    B10001,
    B01110,
    B00000
  },
  { // 4 for mo
    B00000,
    B11111,
    B01001,
    B00101,
    B01101,
    B01101,
    B00011,
    B00001
  }
} ;

const byte shs[][8]  PROGMEM =
{
  { // 0 for talobbo sho bo
    B00000,
    B10001,
    B01011,
    B00101,
    B01111,
    B00001,
    B00001,
    B00001
  },
  { // 1 for donto sho bo
    B11111,
    B01001,
    B00101,
    B00111,
    B01011,
    B10101,
    B00101,
    B00111
  },
  { // 2 for jo fola
    B00000,
    B11111,
    B00100,
    B00100,
    B01000,
    B00100,
    B00100,
    B01000
  }
} ;

const byte bci[][8]  PROGMEM =
{
  { // 0 for bo
    B00000,
    B11111,
    B00011,
    B00101,
    B01001,
    B01001,
    B00101,
    B00011
  },
  { // 1 for aa kar
    B00000,
    B11111,
    B00100,
    B00100,
    B00100,
    B00100,
    B00100,
    B00100
  },
  { // 2 for cho
    B11111,
    B01000,
    B01100,
    B01010,
    B01101,
    B00001,
    B00110,
    B00001
  },
  { // 3 for aa kar
    B00000,
    B11111,
    B00100,
    B00100,
    B00100,
    B00100,
    B00100,
    B00100
  },
  { // 4 for rosho i
    B11110,
    B00001,
    B11111,
    B00100,
    B01010,
    B00100,
    B01000,
    B00110
  }
} ;

byte power = 0, place = 0, digit = 0, count = 0, total_leaf = 0;
float avg_leaf_color = 0;
uint32_t urea = 0;
uint8_t land = 0, land1 = 0;
char button = 'n', crop = 'n', type = 'n', unit = 'n', irrigation = 'n', steps = '1', reset = 'u';
char wavFile[8];
String lat_long = ""; // 0,91.854856,24.911073

void setup() {
  lcd.begin(16, 2);
  tmrpcm.speakerPin = 9;
  tmrpcm.setVolume(5);

  if (!SD.begin(10)) {
    lcd.print(F("SD fail"));
    delay(5000);
  }
  else {
    player_cmd("S");
  }

  n_chr = sizeof(sgtm) / 8;
  for ( i = 0; i < n_chr; i++) {
    memcpy_P (&lcdbuffer, &sgtm[i], 8);
    lcd.createChar(i, lcdbuffer);
  }
  lcd.home();
  lcd.setCursor(5, 0);
  for (i = 0; i < n_chr; i++)
  {
    lcd.print(char(i));
  }
  lcd.print(F("!"));

  delay(3000);

  lcd.clear();
  
  n_chr = sizeof(shs) / 8;
  for ( i = 0; i < n_chr; i++) {
    memcpy_P (&lcdbuffer, &shs[i], 8);
    lcd.createChar(i, lcdbuffer);
  }
  delay(1000);
  lcd.home();
  lcd.setCursor(3, 0);
  for (i = 0; i < n_chr; i++)
  {
    lcd.print(char(i));
  }
  
  n_chr = sizeof(bci) / 8;
  for ( i = 0; i < n_chr; i++) {
    memcpy_P (&lcdbuffer, &bci[i], 8);
    lcd.createChar(i, lcdbuffer);
  }
  delay(1000);
  lcd.home();
  lcd.setCursor(7, 0);
  for (i = 0; i < n_chr; i++)
  {
    lcd.print(char(i));
  }
  lcd.print(":");

  lcd.setCursor(0, 1);
  lcd.print(F("    < Aman >"));

  pinMode(A0, INPUT_PULLUP);
  pinMode(A1, INPUT_PULLUP);
  pinMode(A2, INPUT_PULLUP);
  pinMode(A3, INPUT_PULLUP);
  pinMode(A4, INPUT_PULLUP);
}
void loop() {
  if (analogRead(0) < 800) button = readButtons(0);
  else if (analogRead(1) < 800) button = readButtons(1);
  else if (analogRead(2) < 800) button = readButtons(2);
  else if (analogRead(3) < 800) button = readButtons(3);
  else if (analogRead(4) < 800) button = readButtons(4);

  if (reset != 'u')
  {
    if (button == 'e') //for enter button
    {
      lcd.clear();
      if (reset == 'y')
      {
        power = 0, place = 0, digit = 0, count = 0, total_leaf = 0;
        button = 'n', crop = 'n', type = 'n', unit = 'n', irrigation = 'n', steps = '1';
        avg_leaf_color = 0;
        urea = 0, land = 0, land1 = 0;
        lcd.print(F(" Shosho Bachai:"));
        lcd.setCursor(0, 1);
        lcd.print(F("    < Aman >"));
        player_cmd("EA");
      }
      else if (reset == 'n')
      {
        if (total_leaf == 0)
        {
          lcd.print(F("Jomir Poriman:"));
          lcd.setCursor(0, 1);
          if (land1 != 0)
          {
            lcd.print(land1);
            if (unit == 'b' || unit == 'k')
            {
              lcd.print(F("Bigha "));
            }
            if (unit == 'a' || unit == 'd')
            {
              lcd.print(F("Acre "));
            }
          }
          if (land != 0)
          {
            lcd.print(land);
          }
          if (unit == 'k') lcd.print(F("Katha"));
          if (unit == 'd') lcd.print(F("Decimal"));
          player_cmd("JF");  // JF.WAV to play user don't want to reset and we come back to land size again
        }
        else
        {
          lcd.print(F("Pathar Poriman:"));
          lcd.setCursor(0, 1);
          lcd.print(F("       "));
          player_num(count);
          if (count == total_leaf)
          {
            lcd.clear();
            lcd.print(F("  Enter Button"));
            lcd.setCursor(0, 1);
            lcd.print(F("     Chapun"));
            player_cmd("PF"); // PF.WAV to play user don't want to reset and we come back to leaf color matching again
          }
        }
      }
      reset = 'u';
    }
    else if (button == '>' || button == '<')     // right/left button to choose different option to go in initial state
    {
      lcd.clear();
      lcd.print(F(" Shosho Bachai:"));
      if (reset == 'y')
      {
        lcd.setCursor(0, 1);
        lcd.print(F("    < Na >"));
        player_cmd("NA");   // NA.WAV to play no for resetting in the initial stage
        reset = 'n';
      }
      else if (reset == 'n')
      {
        lcd.setCursor(0, 1);
        lcd.print(F("    < Hae >"));
        player_cmd("HA");  // W.WAV to play wheat
        reset = 'y';
      }
    }
    else if (button == 'r')
    {
      if (total_leaf == 0)
      {
        lcd.print(F("Jomir Poriman:"));
        lcd.setCursor(0, 1);
        if (land1 != 0)
        {
          lcd.print(land1);
          if (unit == 'b' || unit == 'k')
          {
            lcd.print(F("Bigha "));
          }
          if (unit == 'a' || unit == 'd')
          {
            lcd.print(F("Acre "));
          }
        }
        if (land != 0)
        {
          lcd.print(land);
        }
        if (unit == 'k') lcd.print(F("Katha"));
        if (unit == 'd') lcd.print(F("Decimal"));
        player_cmd("JF");  // JF.WAV to play user don't want to reset and we come back to land size again
      }
      else
      {
        lcd.print(F("Pathar Poriman:"));
        lcd.setCursor(0, 1);
        lcd.print(F("       "));
        player_num(count);
        if (count == total_leaf)
        {
          lcd.clear();
          lcd.print(F("  Enter Button"));
          lcd.setCursor(0, 1);
          lcd.print(F("     Chapun"));
          player_cmd("PF"); // PF.WAV to play user don't want to reset and we come back to leaf color matching again
        }
      }
    }
    else if (button != 'n')
    {
      player_cmd("N"); //  N.WAV to play this button is not applicable now
    }

  }
  else if (steps != '4')  //for 1st step(selecting crop & type of crop)
  {
    if (button == 'c') //for crop button
    {
      if (steps == '1')
      {
        if (crop == 'n')
        {
          player_cmd("A");   // A.WAV to play aman paddy
          crop = 'a';
          delay(800);
        }
        if (crop == 'a')
        {
          player_cmd("AB");  // AB.WAV to play aman is selected, now select Ropa/Bona Aman
          lcd.clear();
          lcd.print(F(" Apni Aman Dhan"));
          lcd.setCursor(0, 1);
          lcd.print(F("Bachai korechen"));
          delay(1300);
          lcd.clear();
          lcd.print(F("Ropa/Bona Aman:"));
          lcd.setCursor(0, 1);
          lcd.print(F("    < Ropa >"));
          steps = '3';
        }
        else if (crop == 'b')
        {
          player_cmd("BB"); // BB.WAV to play boro is selected, now select Ropa/Bona boro
          lcd.clear();
          lcd.print(F(" Apni Boro Dhan"));
          lcd.setCursor(0, 1);
          lcd.print(F("Bachai korechen"));
          delay(1300);
          lcd.clear();
          lcd.print(F("Ropa/Bona Boro:"));
          lcd.setCursor(0, 1);
          lcd.print(F("    < Ropa >"));
          steps = '3';
        }
        else if (crop == 'w')
        {
          player_cmd("GB");  // GB.WAV to play wheat is selected, now select irrigation time of wheat
          lcd.clear();
          lcd.print(F("   Apni Gom"));
          lcd.setCursor(0, 1);
          lcd.print(F("Bachai korechen"));
          delay(1300);
          lcd.clear();
          lcd.print(F("   Jolsechon:"));
          lcd.setCursor(0, 1);
          lcd.print(F("  < Prothom >"));
          steps = '2';
        }
        else if (crop == 'm')
        {
          player_cmd("VV");  // VV.WAV to play maize is selected
          lcd.clear();
          lcd.print(F("  Apni Vutta"));
          lcd.setCursor(0, 1);
          lcd.print(F("Bachai korechen"));
          delay(1900);
          lcd.clear();
          lcd.print(F("Urea Diben Bij:"));
          lcd.setCursor(0, 1);
          lcd.print(F(" < Bonar Age >"));
          steps = '3';
        }
      }
      else if (steps == '2')
      {
        if (irrigation == 'n')
        {
          player_cmd("I1"); // I1 to play first irrigation
          irrigation = '1';
          delay(1000);
        }
        if (irrigation == '1')
        {
          player_cmd("FI");    // FI to play first irrigation selected, now select seeding time
          steps = '3';
          lcd.clear();
          lcd.print(F("   Apni Prothom"));
          lcd.setCursor(0, 1);
          lcd.print(F("    Jolsechon"));
          delay(1300);
          lcd.clear();
          lcd.print(F("Bachai korechen"));
          delay(1000);
        }
        else
        {
          steps = '3';
          player_cmd("SI");    // SI to play second irrigation selected, now select seeding time
          lcd.clear();
          lcd.print(F("   Apni ditio"));
          lcd.setCursor(0, 1);
          lcd.print(F("    Jolsechon"));
          delay(1300);
          lcd.clear();
          lcd.print(F("Bachai korechen"));
          delay(1000);
        }
        lcd.clear();
        lcd.print(F("Bij Bona hoyece:"));
        lcd.setCursor(0, 1);
        lcd.print(F(" < Shomoimoto >"));
      }
      else if (steps == '3')
      {
        if (crop == 'a')
        {
          if (type == 'n')
          {
            player_cmd("T"); // T.WAV to play transplanted
            type = 't';
            delay(1000);
          }
          if (type == 't')
          {
            steps = '4';
            player_cmd("RA");  // RA.WAV to play transplanted aman is selected
            lcd.clear();
            lcd.print(F(" Apni Ropa Aman"));
            lcd.setCursor(0, 1);
            lcd.print(F("Bachai korechen"));
            delay(2000);
          }
          else if (type == 's')
          {
            steps = '4';
            player_cmd("VA");  // VA.WAV to play sown aman is selected
            lcd.clear();
            lcd.print(F(" Apni Bona Aman"));
            lcd.setCursor(0, 1);
            lcd.print(F("Bachai korechen"));
            delay(2000);
          }
        }
        else if (crop == 'b')
        {
          if (type == 'n')
          {
            player_cmd("T");  // T.WAV to play transplanted
            type = 't';
            delay(1000);
          }
          if (type == 't')
          {
            steps = '4';
            player_cmd("RB");  // RB.WAV to play transplanted boro is selected
            lcd.clear();
            lcd.print(F(" Apni Ropa Boro"));
            lcd.setCursor(0, 1);
            lcd.print(F("Bachai korechen"));
            delay(2000);
          }
          else if (type == 's')
          {
            steps = '4';
            player_cmd("VB");  // VB.WAV to play sown boro is selected
            lcd.clear();
            lcd.print(F(" Apni Bona Boro"));
            lcd.setCursor(0, 1);
            lcd.print(F("Bachai korechen"));
            delay(2000);
          }
        }
        else if (crop == 'w')
        {
          if (type == 'n')
          {
            type = 'p';
            player_cmd("P");         // to play timely sown
          }
          if (type == 'p')
          {
            steps = '4';
            player_cmd("TS");  // TS.WAV for timely sown wheat is selected
            lcd.clear();
            lcd.print(F("Apni Shomoimoto"));
            lcd.setCursor(0, 1);
            lcd.print(F("   Gomer Bij"));
            delay(2000);
            lcd.clear();
            lcd.print(F("Bopon Korechen"));
            delay(2000);
          }
          else if (type == 'l')
          {
            steps = '4';
            player_cmd("LS");  // LS.WAV for late sown wheat is selected
            lcd.clear();
            lcd.print(F("  Apni Derite"));
            lcd.setCursor(0, 1);
            lcd.print(F("   Gomer Bij"));
            delay(2000);
            lcd.clear();
            lcd.print(F("Bopon Korechen"));
            delay(2000);
          }
        }
        else if (crop == 'm')
        {
          if (type == 'n')
          {
            player_cmd("MB"); // MB.WAV to play before sown
            type = 'b';
            delay(1000);
          }
          if (type == 'b')
          {
            steps = '4';
            player_cmd("AV");  // AV.WAV to play urea required for maize before sown
            lcd.clear();
            lcd.print(F(" Apni Bij Bonar"));
            lcd.setCursor(0, 1);
            lcd.print(F(" Age Urea Diben"));
            delay(3000);
          }
          else if (type == 'a')
          {
            steps = '4';
            player_cmd("PV");  // PV.WAV to play urea required for maize after sown
            lcd.clear();
            lcd.print(F(" Apni Bij Bonar"));
            lcd.setCursor(0, 1);
            lcd.print(F("Pore Urea Diben"));
            delay(3000);
          }
        }
        lcd.clear();
        lcd.print(F("Jomir Poriman:"));
        player_cmd("C2");
      }
    }

    else if (button == '>')     // right button to choose different option of crop, irriagation time, type etc
    {
      lcd.clear();
      lcd.print(F(" Shosho Bachai:"));
      if (steps == '1')
      {

        if (crop == 'n' || crop == 'a')
        {
          lcd.setCursor(0, 1);
          lcd.print(F("              "));
          lcd.setCursor(0, 1);
          lcd.print(F("    < Boro >"));
          player_cmd("B");   // B.WAV to play boro paddy
          crop = 'b';
        }
        else if (crop == 'b')
        {
          lcd.setCursor(0, 1);
          lcd.print(F("              "));
          lcd.setCursor(0, 1);
          lcd.print(F("    < Gom >"));
          player_cmd("W");  // W.WAV to play wheat
          crop = 'w';
        }
        else if (crop == 'w')
        {
          lcd.setCursor(0, 1);
          lcd.print(F("              "));
          lcd.setCursor(0, 1);
          lcd.print(F("   < Vutta >"));
          player_cmd("M");   // M.WAV to play maize
          crop = 'm';
        }
        else if (crop == 'm')
        {
          lcd.setCursor(0, 1);
          lcd.print(F("              "));
          lcd.setCursor(0, 1);
          lcd.print(F("    < Aman >"));
          player_cmd("A");   // A.WAV to play aman paddy
          crop = 'a';
        }
      }
      else if (steps == '2')
      {
        if (crop == 'w')
        {
          lcd.clear();
          lcd.print(F("   Jolsechon:"));
          if (irrigation == 'n' || irrigation == '1')
          {
            lcd.setCursor(0, 1);
            lcd.print(F("   < Ditio >"));
            player_cmd("I2");  // I2 to play second irrigation
            irrigation = '2';
          }
          else if (irrigation == '2')
          {
            lcd.setCursor(0, 1);
            lcd.print(F("  < Prothom >"));
            player_cmd("I1"); // I1 to play first irrigation
            irrigation = '1';
          }
        }
      }
      else if (steps == '3')
      {
        lcd.clear();
        if (crop == 'a' ) lcd.print(F("Ropa/Bona Aman:"));
        else if (crop == 'b') lcd.print(F("Ropa/Bona Boro:"));
        else if (crop == 'w') lcd.print(F("Bij Bona hoyece:"));
        else if (crop == 'm') lcd.print(F("Urea Diben Bij:"));
        if (crop == 'a' || crop == 'b')
        {
          if (type == 'n' || type == 't')
          {
            lcd.setCursor(0, 1);
            lcd.print(F("    < Bona >"));
            player_cmd("S1");  // s1 to play sown
            type = 's';
          }
          else if (type == 's')
          {
            lcd.setCursor(0, 1);
            lcd.print(F("    < Ropa >"));
            player_cmd("T"); // T.WAV to play transplanted
            type = 't';
            delay(500);
          }
        }
        else if (crop == 'w')
        {
          if (type == 'n' || type == 'p')
          {
            lcd.setCursor(0, 1);
            lcd.print(F("   < Derite >"));
            player_cmd("L"); // L.WAV to play belated sown
            type = 'l';
            delay(500);
          }
          else if (type == 'l')
          {
            lcd.setCursor(0, 1);
            lcd.print(F(" < Somoimoto >"));
            player_cmd("P"); // P.WAV to play timely sown
            type = 'p';
            delay(500);
          }
        }
        else if (crop == 'm')
        {
          if (type == 'n' || type == 'b')
          {
            lcd.setCursor(0, 1);
            lcd.print(F(" < Bonar Pore >"));
            player_cmd("MA"); // MA.WAV to play after sown
            type = 'a';
            delay(500);

          }
          else if (type == 'a')
          {
            lcd.setCursor(0, 1);
            lcd.print(F(" < Bonar Age >"));
            player_cmd("MB"); // MB.WAV to play after sown
            type = 'b';
            delay(500);
          }
        }
      }
    }
    else if (button == '<')     // left button to choose different option of crop, irriagation time, type etc
    {
      lcd.clear();
      lcd.print(F(" Shosho Bachai:"));
      if (steps == '1')
      {
        if (crop == 'n' || crop == 'a')
        {
          lcd.setCursor(0, 1);
          lcd.print(F("   < Vutta >"));
          player_cmd("M");   // M.WAV to play maize
          crop = 'm';
          delay(500);
        }
        else if (crop == 'm')
        {
          lcd.setCursor(0, 1);
          lcd.print(F("    < Gom >"));
          player_cmd("W");  // W.WAV to play wheat
          crop = 'w';
          delay(500);
        }
        else if (crop == 'w')
        {
          lcd.setCursor(0, 1);
          lcd.print(F("    < Boro >"));
          player_cmd("B");   // B.WAV to play boro paddy
          crop = 'b';
          delay(500);
        }
        else if (crop == 'b')
        {
          lcd.setCursor(0, 1);
          lcd.print(F("    < Aman >"));
          player_cmd("A");   // A.WAV to play aman paddy
          crop = 'a';
          delay(500);
        }
      }
      else if (steps == '2')
      {
        if (crop == 'w')
        {
          lcd.clear();
          lcd.print(F("   Jolsechon:"));
          if (irrigation == 'n' || irrigation == '1')
          {
            lcd.setCursor(0, 1);
            lcd.print(F("   < Ditio >"));
            player_cmd("I2");  // I2 to play second irrigation
            irrigation = '2';
            delay(500);
          }
          else if (irrigation == '2')
          {
            lcd.setCursor(0, 1);
            lcd.print(F("  < Prothom >"));
            player_cmd("I1"); // I1 to play first irrigation
            irrigation = '1';
            lcd.setCursor(0, 1);
            delay(500);

          }
        }
      }
      else if (steps == '3')
      {
        lcd.clear();
        if (crop == 'a' ) lcd.print(F("Ropa/Bona Aman:"));
        else if (crop == 'b') lcd.print(F("Ropa/Bona Boro:"));
        else if (crop == 'w') lcd.print(F("Bij Bona hoyece:"));
        else if (crop == 'm') lcd.print(F("Urea Diben Bij:"));
        if (crop == 'a' || crop == 'b')
        {
          if (type == 'n' || type == 't')
          {
            lcd.setCursor(0, 1);
            lcd.print(F("    < Bona >"));
            player_cmd("S1");  // s1 to play sown
            type = 's';
            delay(500);
          }
          else if (type == 's')
          {
            lcd.setCursor(0, 1);
            lcd.print(F("    < Ropa >"));
            player_cmd("T"); // T.WAV to play transplanted
            type = 't';
            delay(500);
          }
        }
        else if (crop == 'w')
        {
          if (type == 'n' || type == 'p')
          {
            lcd.setCursor(0, 1);
            lcd.print(F("   < Derite >"));
            player_cmd("L"); // L.WAV to play belated sown
            type = 'l';
            delay(500);
          }
          else if (type == 'l')
          {
            lcd.setCursor(0, 1);
            lcd.print(F(" < Shomoimoto >"));
            player_cmd("P"); // P.WAV to play timely sown
            type = 'p';
            delay(500);
          }
        }
        else if (crop == 'm')
        {
          if (type == 'n' || type == 'b')
          {
            lcd.setCursor(0, 1);
            lcd.print(F(" < Bonar Pore >"));
            player_cmd("MA"); // MA.WAV to play after sown
            type = 'a';
            delay(500);

          }
          else if (type == 'a')
          {
            lcd.setCursor(0, 1);
            lcd.print(F(" < Bonar Age >"));
            player_cmd("MB"); // MB.WAV to play after sown
            type = 'b';
            delay(500);
          }
        }
      }
    }
    else if (button == 'e') // enter button to play usefulness and rules for crops
    {
      if (crop == 'n')
      {
        player_cmd("U");  // U.WAV to play usefulness of LCC
      }
      else
      {
        if (crop == 'a')
        {
          player_cmd("R1"); // R1.WAV to play rules of using LCC for aman
        }
        else if (crop == 'b')
        {
          player_cmd("R2");  // R2.WAV to play rules of using LCC for boro
        }
        else if (crop == 'm')
        {
          player_cmd("R4");  // R4.WAV to play rules of using LCC for maize
        }
        else if (crop == 'w')
        {
          player_cmd("R3");  // R3.WAV to play rules of using LCC for wheat
        }
      }
    }

    else if (button == 'r') //back button(back for crop and type selection)
    {
      if (irrigation != 'n' && steps == '3')
      {
        irrigation = 'n';
        steps = '2';
        player_cmd("P3"); // P3.WAV to play previous irrigation is omitted
        lcd.clear();
        lcd.print(F("   Jolsechon:"));
        lcd.setCursor(0, 1);
        lcd.print(F("  < Prothom >"));
        delay(1000);
      }
      else if ((crop != 'n' && steps == '3') || (crop != 'n' && steps == '2'))
      {
        crop = 'n';
        type = 'n';
        steps = '1';
        player_cmd("P1"); // P1.WAV to play previous crop is omitted
        lcd.clear();
        lcd.print(F(" Shosho Bachai:"));
        lcd.setCursor(0, 1);
        lcd.print(F("    < Aman >"));
        delay(1000);
      }
      else
      {
        player_cmd("N"); //  N.WAV to play this button is not applicable now
      }
    }
    else if (button != 'n')
    {
      player_cmd("N");  //  N.WAV to play this button is not applicable now
    }
  }

  else if (total_leaf == 0) //for 4th steps(inputing landSize in 4 type of units)
  {
    lcd.setCursor(0, 1);
    for (byte i = 0; i <= 9; i++)
    {
      if (button == (i + '0') && power < 2)
      {
        if (unit == 'n' || unit == 'b' || unit == 'a')
        {
          digit = i;
          land = land * 10 + digit;
          if (power == 1) lcd.setCursor(place - 1, 1);
          else lcd.setCursor(place, 1);
          if (land == 0) player_cmd("N"); //  N.WAV to play this button is not applicable now
          else
          {
            player_num(land);
            power++;
            place++;
          }
        }
        else
        {
          player_cmd("N"); //  N.WAV to play this button is not applicable now
        }
      }
      else if (button == (i + '0') && power == 2)
      {
        player_cmd("N"); //  N.WAV to play this button is not applicable now
      }
    }
    if (button == 'b')
    {

      if (land != 0 && unit == 'n')
      {
        if (land > 10)
        {
          land = land / 10;
          place--;
          power--;
          lcd.setCursor(place, 1);
          lcd.print(F(" "));
          player_cmd("U2");  // U2.WAV to play you can not give more then 10 for bigha and acre
          unit = 'n';
        }
        else
        {
          lcd.setCursor(place, 1);
          lcd.print(F("     "));
          lcd.setCursor(place, 1);
          lcd.print(F("Bigha"));
          player_cmd("BG"); // BG.WAV to play bigha
          unit = 'b';
          land1 = land;
          land = 0;
          power = 0;
          place += 6;
          delay(1000);
          if (type == 'b' || irrigation == '1') player_cmd("BU");  // BU.WAV to play either you can add land size with katha unit also or press enter to know urea
          else player_cmd("BK"); // BK.WAV to play either you can add land size with katha unit also or press enter to know amount of leaves for color matching
        }
      }
      else player_cmd("N");  //  N.WAV to play this button is not applicable now
    }
    else if (button == 'k')
    {
      if (land != 0 && (unit == 'n' || unit == 'b'))
      {
        if (land > 19)
        {
          land = land / 10;
          place--;
          power--;
          lcd.setCursor(place, 1);
          lcd.print(F("      "));
          player_cmd("KT"); //  KT.WAV play if land size is more than twenty in katha then input land size using both bigha and katha
          if (land1 != 0) unit = 'b';
          else unit = 'n';
        }
        else if ((unit == 'b' || unit == 'n') && land != 0)
        {
          lcd.setCursor(place, 1);
          lcd.print(F("     "));
          lcd.setCursor(place, 1);
          lcd.print(F("Katha"));
          player_cmd("K"); //  K.WAV play katha
          unit = 'k';
          place += 6;
          delay(1000);
          if (type == 'b' || irrigation == '1') player_cmd("EU");  // EU.WAV to play now you can know urea by pressing enter
          else player_cmd("EK"); // EK.WAV to play now you can know total required leaf and match leaf color by pressing enter
        }
      }
      else player_cmd("N");   //  N.WAV to play this button is not applicable now
    }
    else if (button == 'a')
    {
      if (land != 0 && unit == 'n')
      {
        if (land > 10)
        {
          land = land / 10;
          place--;
          power--;
          lcd.setCursor(place, 1);
          lcd.print(F(" "));
          player_cmd("U2");  // U2.WAV to play you can not give more then 10 for bigha and acre
          unit = 'n';
        }
        else
        {
          lcd.setCursor(place, 1);
          lcd.print(F("     "));
          lcd.setCursor(place, 1);
          lcd.print(F("Acre"));
          player_cmd("E"); // E.WAV to play acre
          unit = 'a';
          land1 = land;
          land = 0;
          power = 0;
          place += 5;
          delay(1000);
          if (type == 'b' || irrigation == '1') player_cmd("AU");  // to play either you can add land size with decimal unit also or press enter to know urea
          else player_cmd("AK"); // to play either you can add land size with decimal unit also or press enter to know amount of leaves for color matching
        }
      }
      else player_cmd("N"); //  N.WAV to play this button is not applicable now
    }
    else if (button == 'd')
    {
      if (land != 0 && (unit == 'n' || unit == 'a'))
      {
        lcd.setCursor(place, 1);
        lcd.print(F("     "));
        lcd.setCursor(place, 1);
        lcd.print(F("Decimal"));
        player_cmd("D"); //  D.WAV play decimal
        unit = 'd';
        delay(1000);
        place += 8;
        if (type == 'b' || irrigation == '1') player_cmd("EU");  // EU.WAV to play now you can know urea by pressing enter
        else player_cmd("EK"); // EK.WAV to play now you can know total required leaf and match leaf color by pressing enter
      }
      else player_cmd("N");  //  N.WAV to play this button is not applicable now
    }
    else if (button == 'r') // back button to back if any wrong digit or unit is given or to back if any wrong type of crop is given
    {
      if (type != 'n' && place == 0 && steps == '4')    // place is used for landsize and unit input in second row of lcd, if place is 0 then we can go back to type selection again
      {
        type = 'n';
        steps = '3';
        if (crop == 'a')
        {
          player_cmd("P2"); //  P2.WAV to play previous type of crop is omitted
          lcd.clear();
          lcd.print(F("Ropa / Bona Aman: "));
          lcd.setCursor(0, 1);
          lcd.print(F("    < Ropa >"));
          delay(1000);
        }
        else if (crop == 'b')
        {
          player_cmd("P2"); //  P2.WAV to play previous type of crop is omitted
          lcd.clear();
          lcd.print(F("Ropa / Bona Boro: "));
          lcd.setCursor(0, 1);
          lcd.print(F("    < Ropa >"));
          delay(1000);
        }
        else if (crop == 'w')
        {
          player_cmd("P2"); //  P2.WAV to play previous type of crop is omitted
          lcd.clear();
          lcd.print(F("Bij Bona hoyece: "));
          lcd.setCursor(0, 1);
          lcd.print(F(" < Shomoimoto >"));
          delay(1000);
        }
        else if (crop == 'm')
        {
          player_cmd("P2"); //  P2.WAV to play previous type of crop is omitted
          lcd.clear();
          lcd.print(F("Urea Diben Bij: "));
          lcd.setCursor(0, 1);
          lcd.print(F(" < Bonar Age >"));
          delay(1000);
        }
      }
      else if (place > 0)     // place is not zero that means landsize and unit is given, then we have to redo if wrong unit or landsize is given
      {
        if (unit == 'a' && land == 0)
        {
          player_cmd("PA"); //  PA.WAV to play previous unit is omitted
          place -= 5;
          lcd.setCursor(place, 1);
          lcd.print(F("     "));
          land = land1;
          if (land > 9) power = 2;
          else power = 1;
          land1 = 0;
          unit = 'n';
        }
        else if (unit == 'b' && land == 0)
        {
          player_cmd("PA"); //  PA.WAV to play previous unit is omitted
          place -= 6;
          lcd.setCursor(place, 1);
          lcd.print(F("      "));
          land = land1;
          if (land > 9) power = 2;
          else power = 1;
          land1 = 0;
          unit = 'n';
        }
        else if (unit == 'd')
        {
          player_cmd("PA"); //  PA.WAV to play previous unit is omitted
          place -= 8;
          lcd.setCursor(place, 1);
          lcd.print(F("        "));
          if (land1 != 0) unit = 'a';
          else unit = 'n';
        }
        else if (unit == 'k')
        {
          player_cmd("PA"); //  PA.WAV to play previous unit is omitted
          place -= 6;
          lcd.setCursor(place, 1);
          lcd.print(F("      "));
          if (land1 != 0) unit = 'b';
          else unit = 'n';
        }
        else if (land != 0)
        {
          power--;
          place--;
          land = land / 10;
          lcd.setCursor(place, 1);
          if (land == 0)
          {
            player_cmd("PJ");   // PJ.WAV to play landsize is omitted
            lcd.print(" ");
          }
          else
          {
            lcd.setCursor(place, 1);
            lcd.print("  ");
            lcd.setCursor(place - 1, 1);
            player_num(land);   // play the minimized landsize
          }
        }
      }
    }
    else if (button == 'e' && (irrigation == '1' || type == 'b')) // enter button to know the amount of urea for wheat with first irrigation or for maize before sown
    {
      total_leaf = 1, count = total_leaf;
    }
    else if (button == 'e' && place == 0)
    {
      if (crop == 'a')
      {
        player_cmd("R1"); // R1.WAV to play rules of using LCC for aman
      }
      else if (crop == 'b')
      {
        player_cmd("R2");  // R2.WAV to play rules of using LCC for boro
      }
      else if (crop == 'w')
      {
        player_cmd("R3");  // R3.WAV to play rules of using LCC for wheat
      }
      else if (crop == 'm')
      {
        player_cmd("R4");  // R4.WAV to play rules of using LCC for maize
      }
    }
    else if (button == 'e' && ((land1 != 0 && land == 0) || (unit == 'k' || unit == 'd'))) // enter button to know the required number of leaves to match leaf color
    {
      if (land1 != 0) total_leaf = 10 + (land1 - 1) * 5;
      else total_leaf = 10;
      lcd.clear();
      lcd.print(F("  "));
      player_num(total_leaf);
      lcd.print(F(" ti Pathar "));
      lcd.setCursor(0, 1);
      lcd.print(F("   Rong Milan"));
      player_cmd("M1"); // M1.WAV to play take match leaf color with the colors given in the device
    }
    else if (button == 'c')
    {
      lcd.clear();
      lcd.print(F(" Shosho Bachai?"));
      lcd.setCursor(0, 1);
      lcd.print(F("    < Na >"));
      player_cmd("AA");   // AA.WAV to play is user want to reset the device in its initial stage
      reset = 'n';
    }
    else if ((button >= 'A' && button <= 'F') || button == '<' || button == '>')
    {
      player_cmd("N"); //  N.WAV to play this button is not applicable now
    }
  }

  else if (count != total_leaf) //for 3rd step(matching leaf color)
  {
    if (button >= 'A' && button <= 'F')
    {
      if (count < total_leaf )
      {
        count++;
        if (button == 'A') {
          avg_leaf_color = avg_leaf_color + 1;
          digit = 1;
        }
        else if (button == 'B') {
          avg_leaf_color = avg_leaf_color + 2;
          digit = 2;
        }
        else if (button == 'C') {
          avg_leaf_color = avg_leaf_color + 3;
          digit = 3;
        }
        else if (button == 'D') {
          avg_leaf_color = avg_leaf_color + 4;
          digit = 4;
        }
        else if (button == 'E') {
          avg_leaf_color = avg_leaf_color + 5;
          digit = 5;
        }
        else {
          avg_leaf_color = avg_leaf_color + 6;
          digit = 6;
        }

        lcd.clear();
        lcd.print(F("Pathar Poriman: "));
        lcd.setCursor(0, 1);
        lcd.print(F("       "));
        player_num(count);
        if (count == total_leaf)
        {
          player_cmd("R"); // R.WAV to play leaf color matching is completed
          lcd.clear();
          lcd.print(F("  Enter Button"));
          lcd.setCursor(0, 1);
          lcd.print(F("     Chapun"));
        }
      }
    }
    else if (button == 'r' && count != 0) // back button to go back if any wrong leaf color is matched
    {
      avg_leaf_color = avg_leaf_color - digit;
      count--;
      lcd.clear();
      lcd.print(F("Pathar Poriman: "));
      lcd.setCursor(0, 1);
      lcd.print(F("       "));
      player_num(count);
    }
    else if (button == 'c')
    {
      lcd.clear();
      lcd.print(F(" Shosho Bachai?"));
      lcd.setCursor(0, 1);
      lcd.print(F("    < Na >"));
      player_cmd("AA");   // AA.WAV to play is user want to reset the device in its initial stage
      reset = 'n';
    }
    else if (!(button >= 'A' && button <= 'F')  && button != 'n' && count == 0) // back button to go back if any wrong leaf color is matched
    {
      player_cmd("N");  //  N.WAV to play this button is not applicable now
    }
  }
  else    //last step(showing required urea fertilizer)
  {
    if (button == 'e') //enter button to show urea amount
    {
      if (digit != 10)          // this is so that if user press enter again and again he can know the amount again and again without changing avg_leaf_color of leaf color
      {
        avg_leaf_color /= total_leaf;

        lcd.clear();
        lcd.print(F("   5 Seconds"));
        lcd.setCursor(0, 1);
        lcd.print(F(" Opekkha Korun"));

        gprsSerial.begin(2400);
        Serial.begin(9600);

        gprsSerial.flush();
        Serial.flush();

        // attach or detach from GPRS service
        gprsSerial.println(F("AT+CGATT?"));
        toSerial();
        delay(1000);

        // bearer settings
        gprsSerial.println("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"");
        toSerial();
        delay(1000);


        // bearer settings
        gprsSerial.println("AT+SAPBR=3,1,\"APN\",\"GP-INTERNET\"");
        toSerial();
        delay(1000);


        //  bearer settings
        gprsSerial.println(F("AT+SAPBR=1,1"));
        toSerial();
        delay(1000);



        gprsSerial.println(F("AT+CIPGSMLOC=1,1"));
        char c = 't';
        bool find_zero = false;
        while (gprsSerial.available())
        {

          c = gprsSerial.read();
          if (c == '0') find_zero = true;
          if (find_zero == true) lat_long.concat(c);
          delay(100);
          if (lat_long.length() == 21) break;
        }
        Serial.print(lat_long);

        // initialize http service
        gprsSerial.println(F("AT+HTTPINIT"));
        toSerial();
        delay(1000);

        // set http param value with full url
        //    gprsSerial.println(F("AT+HTTPPARA=\"URL\",\"dayansiddik94.000webhostapp.com/w.php?crd=+CIPGSMLOC:0,91.852482,24.909982&cp=aman%20paddy&tp=transplanted&it=not%20applicable&ls=1%20acre%2010%20decimal&tl=10&alc=2.5&u=18%20kg\""));


        gprsSerial.print(F("AT+HTTPPARA=\"URL\",\"dayansiddik94.000webhostapp.com/w.php?"));  // set http param value with main url

        // sending coordinates to url
        gprsSerial.print(F("crd="));  // +CIPGSMLOC:0,91.852482,24.909982
        gprsSerial.print(lat_long);

        // sending crop to url
        if (crop == 'a') gprsSerial.print(F("&cp=Aman%20Paddy"));
        else if (crop == 'b') gprsSerial.print(F("&cp=Boro%20Paddy"));
        else if (crop == 'w') gprsSerial.print(F("&cp=Wheat"));
        else if (crop == 'm') gprsSerial.print(F("&cp=Maize"));

        // sending type of the crop data to url
        if (type == 't') gprsSerial.print(F("&tp=Transplanted"));
        else if (type == 's') gprsSerial.print(F("&tp=Seeded"));
        else if (type == 'p') gprsSerial.print(F("&tp=Timely%20Sown"));
        else if (type == 'l') gprsSerial.print(F("&tp=Lately%20Sown"));
        else gprsSerial.print(F("&tp=Not%20Applicable"));

        // sending irrigation time if crop is wheat data to url
        if (irrigation == '1') gprsSerial.print(F("&it=First%20Irrigation"));
        else if (irrigation == '2') gprsSerial.print(F("&it=Second%20Irrigation"));
        else gprsSerial.print(F("&it=Not%20Applicable"));

        // sending landsize data to url
        gprsSerial.print(F("&ls="));
        if (land1 != 0)
        {
          gprsSerial.print(land1);
          if (unit == 'b' || unit == 'k') gprsSerial.print(F("%20Bigha%20"));
          else if (unit == 'a' || unit == 'd') gprsSerial.print(F("%20Acre%20"));
        }

        if (land != 0)
        {
          gprsSerial.print(land);
          if (unit == 'k') gprsSerial.print(F("%20Katha"));
          else if (unit == 'd') gprsSerial.print(F("%20Decimal"));
        }

        // sending total leaf to match color if requires data to url
        gprsSerial.print(F("&tl="));
        if (total_leaf == 1) gprsSerial.print(F("0"));
        else gprsSerial.print(total_leaf);

        // sending average leaf color data to url
        gprsSerial.print(F("&alc="));
        gprsSerial.print(avg_leaf_color);
      }

      urea = 0;  // to start every press of enter with urea  = 0 and then measure urea again

      if ((type == 't' && avg_leaf_color < 3.5) || (type == 's' && avg_leaf_color < 3))
      {
        if ( crop == 'a')
        {
          if (land1 != 0)
          {
            if (unit == 'b' || unit == 'k') urea = land1 * 7500;
            else urea = land1 * 22730;
          }
          if (unit == 'k') urea = urea + land * 375;
          else if (unit == 'd') urea = urea + land * 230;
        }
        else
        {
          if (land1 != 0)
          {
            if (unit == 'b' || unit == 'k') urea = land1 * 9000;
            else urea = land1 * 27270;
          }
          if (unit == 'k') urea = urea + land * 450;
          else if (unit == 'd') urea = urea + land * 270;
        }
      }
      else if ((type == 'l' && avg_leaf_color < 4) || (type == 'p' && avg_leaf_color >= 4) || type == 'b' || (type == 'a' && avg_leaf_color < 5))
      {
        if (land1 != 0)
        {
          if (unit == 'b' || unit == 'k') urea = land1 * 8250;
          else urea = land1 * 25000;
        }
        if (unit == 'k') urea = urea + land * 410;
        else if (unit == 'd') urea = urea + land * 250;
      }
      else if (type == 'p' && avg_leaf_color < 4)
      {
        if (land1 != 0)
        {
          if (unit == 'b' || unit == 'k') urea = land1 * 13250;
          else urea = land1 * 40000;
        }
        if (unit == 'k') urea = urea + land * 660;
        else if (unit == 'd') urea = urea + land * 400;
      }
      else if (type == 'l' && avg_leaf_color >= 4)
      {
        if (land1 != 0)
        {
          if (unit == 'b' || unit == 'k') urea = land1 * 5000;
          else urea = land1 * 15000;
        }
        if (unit == 'k') urea = urea + land * 250;
        else if (unit == 'd') urea = urea + land * 150;
      }

      if (urea == 0)
      {
        gprsSerial.println(F("&u=Not%20Required\""));
        lcd.clear();
        if (crop == 'a')
        {
          lcd.print(F("   Amane"));
          player_cmd("J"); // J.WAV to play  no need of urea for aman
        }
        else if (crop == 'b')
        {
          lcd.print(F("   Boror"));
          player_cmd("V"); // V.WAV to play no need of urea for boro
        }
        else if (crop == 'm')
        {
          lcd.print(F(" Vuttar"));
          player_cmd("Q"); // Q.WAV to play no need of urea for maize
        }
        lcd.print(F(" Urea"));
        lcd.setCursor(0, 1);
        lcd.print(F("Proyojonio noi"));
        delay(3000);
        lcd.clear();
        lcd.print(F(" 5 din por abar"));
        lcd.setCursor(0, 1);
        lcd.print(F("  jachai korun"));
      }
      else
      {
        gprsSerial.print(F("&u="));
        lcd.clear();
        lcd.print(F("Urear Poriman:"));
        if (crop == 'a')
        {
          player_cmd("C"); // C.WAV to play urea for aman
        }

        else if (crop == 'b')
        {
          player_cmd("F"); // F.WAV to play urea for boro
        }
        else if (crop == 'w')
        {
          player_cmd("H"); // H.WAV to play urea for wheat
        }
        else if (crop == 'm')
        {
          player_cmd("I"); // I.WAV to play urea for maize
        }
        delay(2200);
        lcd.setCursor(0, 1);
        if (urea >= 40000)
        {
          gprsSerial.print(urea / 40000);
          player_num(urea / 40000);
          gprsSerial.print(F("%20MON%20"));
          lcd.print(F("MON "));
          player_cmd("O"); // O.WAV to play mon
          delay(600);
          urea = urea % 40000;
        }
        if (urea >= 1000)
        {
          gprsSerial.print(urea / 1000);
          player_num(urea / 1000);
          gprsSerial.print(F("%20KG%20"));
          lcd.print(F("KG "));
          player_cmd("Z"); // Z.WAV to play kg
          delay(600);
          urea = urea % 1000;
        }
        if (urea >= 100)
        {
          gprsSerial.print(urea / 100);
          player_num(urea / 100);
          player_cmd("Y"); // Y.WAV to play sho
          delay(400);
          urea = urea % 100;
          if (urea < 10)
          {
            gprsSerial.print(F("00%20GM"));
            lcd.print(F("00GM"));
            player_cmd("G"); // G.WAV to play gm
          }
        }
        if (urea < 100 && urea >= 10)
        {
          urea = urea / 10;
          urea = urea * 10;
          gprsSerial.print(urea);
          player_num(urea);
          gprsSerial.print(F("%20GM"));
          lcd.print(F("GM"));
          player_cmd("G"); // G.WAV to play gm
        }
        gprsSerial.println(F("\""));
        delay(1000);
        lcd.clear();
        lcd.print(F("10 din por abar"));
        lcd.setCursor(0, 1);
        lcd.print(F(" Jachai korun"));
        player_cmd("X"); // X.WAV to play recheck after 10 days
        delay(2800);
      }

      if (digit != 10)      // this is so that if user press enter again and again he can know the amount again and again but not send data to url again and again
      {
        digit = 10;

        toSerial();
        delay(1000);

        // set http action type 0 = GET, 1 = POST, 2 = HEAD
        gprsSerial.println(F("AT+HTTPACTION=0"));
        toSerial();
        delay(1000);
        gprsSerial.println(F("AT+HTTPTERM"));
        toSerial();
        delay(1000);
      }
    }
    else if (button == 'r') // back button to go back if any wrong leaf color is matched
    {
      if (digit != 10)
      {
        avg_leaf_color = avg_leaf_color - digit;
        count--;
        lcd.clear();
        lcd.print(F("Pathar Poriman:"));
        lcd.setCursor(0, 1);
        lcd.print(F("       "));
        player_num(count);
      }
      else
      {
        player_cmd("N");  //  N.WAV to play this button is not applicable now
      }
    }
    else if (button == 'c' && digit == 10) // Crop button to reset all settings to initial state
    {
      power = 0, place = 0, digit = 0, count = 0, total_leaf = 0;
      button = 'n', crop = 'n', type = 'n', unit = 'n', irrigation = 'n', steps = '1';
      avg_leaf_color = 0;
      urea = 0, land = 0, land1 = 0;
      player_cmd("P1"); // P1.WAV to play previous crop is omitted
      lcd.clear();
      lcd.print(F(" Shosho Bachai:"));
      lcd.setCursor(0, 1);
      lcd.print(F("    < Aman >"));
    }
    else if (button != 'n')
    {
      player_cmd("N");  //  N.WAV to play this button is not applicable now
    }
  }
  delay(200);
  button = 'n'; // make the pressed button none again
}


char readButtons(uint8_t pin)
// returns the button number pressed, or zero for none pressed
// int pin is the analog pin number to read
{
  char button = 'n';
  uint16_t value = 0;
  value = analogRead(pin); // get the analog value
  if (pin == 0)
  {
    if (value < 200) button = 'A';
    else if (value < 300) button = 'B';           // values for 10kohm resistor
    else if (value < 400) button = 'C';
    else if (value < 520) button = 'D';
    else if (value < 700) button = 'E';
  }
  else if (pin == 1)
  {
    if (value < 200) button = 'e';
    else if (value < 300) button = '>';
    else if (value < 400) button = 'c';     // values for 10kohm resistor
    else if (value < 520) button = '<';
    else if (value < 700) button = 'r';
  }
  else if (pin == 2)
  {
    if (value < 200) button = '1';
    else if (value < 300) button = '2';
    else if (value < 400) button = '3';     // values for 10kohm resistor
    else if (value < 520) button = '6';
    else if (value < 700) button = '5';
  }
  else if (pin == 3)
  {
    if (value < 200) button = '4';
    else if (value < 300) button = '7';
    else if (value < 400) button = '8';     // values for 10kohm resistor
    else if (value < 520) button = '9';
    else if (value < 700) button = '0';
  }
  else if (pin == 4)
  {
    if (value < 200) button = 'k';
    else if (value < 300) button = 'b';
    else if (value < 400) button = 'a';       // values for 10kohm resistor
    else if (value < 520) button = 'd';
    else if (value < 700) button = 'F';
  }
  return button;
}

void player_cmd(char* file)
{
  sprintf(wavFile, "%s.wav", file);
  tmrpcm.play(wavFile);
  delay(400);
}

void player_num(byte number)
{
  lcd.print(number);
  sprintf(wavFile, "%d.wav", number);
  tmrpcm.play(wavFile);
  delay(900);
}

void toSerial()
{
  while (gprsSerial.available())
  {
    Serial.write(gprsSerial.read());
  }
}
