#include <LiquidCrystal.h>    // LCD-Load library
#include <SPI.h>              // SPI-Load library
#include <MFRC522.h>          // RFID-Load library
#include <Wire.h>             // This library allows you to communicate with I2C devices
#include "RTClib.h"           // This library allows you to control and use the RTC (Real Time Clock)
#include <SD.h>               // The SD(Secure Digital Memory Card)library allows for reading from and writing to SD cards
#include <EEPROM.h>           // This library allows EEPROM to use


//*******************************************  Employees assignment  **************************************************//

#define Wordlenght 9             // What Length is name
//To add the employee, fill the next lines
#define Person 4                 // number of employees  "4"

#define Person1 0x865F37BB       // The card code its defined here and corresponds to the person
#define Person2 0x70EE8115
#define Person3 0x566DFB82       // Example
#define Person4 0xD61EF22B
//#define Person5 0x00000000     // Example

unsigned long int Cards[Person] = {Person1, Person2, Person3, Person4}; //fill the next person Number


//To add the employeer's name, fill the next lines
#define employees1 "Person1"    // The card corresponds to the employee. Instead of "PersonX" write name of employee.
#define employees2 "Person2"
#define employees3 "Person3"
#define employees4 "Person4"
//#define employeesX "PersonX"   // Example

char employee[][Wordlenght] = {employees1, employees2, employees3, employees4}; //add new employeesX

int present[Person];             // Array for save "Person are present at work"

//*********************************************************************************************************************//

File logFile;
MFRC522 mfrc522(8, 9);                // RFID-Recipients name Slave Select(SDA) at Pin8 and RST at pin 9
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);  // This line defines which pins of the microcontroller board are used for the LCD
RTC_DS1307 rtc;

// set up variables using the SD utility library functions:
Sd2Card card;

const int chipSelect = 10;            //SPI Slave Select Pin10 (SD Card)

//necessary variable**************************************************
int CARD;
int come = 0;
int leave = 0;
int monat = 0;
unsigned long int readCod = 0;
int registration = 0;
int personPresent = 0;
int set_hour = 0;
int set_minuts = 0;
int set_month = 0;
int set_day = 0;
int address;                        // variable for save EEPROM address
byte value;                         // variable for save EEPROM value
int premission_setup;
//********************************************************************

void zeit();                       // prototype for time function
void zeit_setup();                 // prototype for time-setup function
void Card_check();
void log_out_check();

void setup()
{
    lcd.begin(16, 4);             // Im Setup wird angegeben, wie viele Zeichen und Zeilen verwendet werden. Hier: 16 Zeichen in 2 Zeilen.
    Serial.begin(9600);           // Serielle Verbindung starten (Monitor)
    SPI.begin();                  // SPI-Verbindung aufbauen
    mfrc522.PCD_Init();           // Initialisierung des RFID-Empfängers
    rtc.begin();
    DDRC = 0x08;                  // select PIN state "in" "out" for button end LED

    //************* Debug EEPROM memory *******************//
/*
   while (address < 10) {
      value = EEPROM.read(address);            // reading EEPROM byte
      Serial.print("Address: ");
      Serial.print(String(address));           // address number
      Serial.print(", value: ");
      Serial.println(String(value));           // value in address
      address++;
      delay(100);
   }
*/
    //********************************************************//
}

void loop()
{
    zeit();                                     // function call
    DateTime now = rtc.now();                   // time determination for RTC
    Card_check();

    if (PINC & 0x01)                            // Request button if it was pressed PINC == Arduino PIN A
        {
            come = 1;
        }
    else if (PINC & 0x02)
        {
            leave = 1;
        }
    else if (PINC & 0x04)
        {
            lcd.setCursor(0, 1);
            lcd.print("INFO");
            lcd.setCursor(0, 2);
            lcd.print("Noch nicht");
            lcd.setCursor(0, 3);
            lcd.print("Programmiert");
            delay(1500);
            lcd.clear();
         }

  // ------- reset employees data who have not logged out ------- //
  log_out_check();

  if ((come == 1) || (leave == 1))
  {
        lcd.setCursor(0, 1);
        lcd.print("Bitte anmelden");
        PORTC |= 0x08;                    //LED on

        // --- waiting period for loop "registration" --- //

        int registration_time = now.second();
        // lcd.print(now.second()); //Debug
        int waiting_time = registration_time + 10; //waiting time = 10s.
        if (registration_time >= 50)
            {
                waiting_time = waiting_time - 60;
            }


    // ************************************** ### Main funktion ### ********************************** //
        while (registration != 1)                  // it runs while "registration" is not equal "1"
            {
                SD.begin (chipSelect);             // start SD and allows "reading from" and "writing to" SD cards
                DateTime now = rtc.now();

                  //Debug waiting period
                  /*  lcd.setCursor(0, 2);
                      lcd.print(now.second());
                  */
                  if (now.second() == waiting_time)
                    {
                        registration = 1;
                    }
                 else
                    {
                        //*************************** Month file name choice and write ***************************//
                        if (now.month() != monat)
                        {
                          char filename[] = "Monat00.CSV";
                          switch (now.month())
                          {
                            case 1: filename[5] = 0 + '0'; filename[6] = 1 + '0'; monat = 1; break;
                            case 2: filename[5] = 0 + '0'; filename[6] = 2 + '0'; monat = 2; break;
                            case 3: filename[5] = 0 + '0'; filename[6] = 3 + '0'; monat = 3; break;
                            case 4: filename[5] = 0 + '0'; filename[6] = 4 + '0'; monat = 4; break;
                            case 5: filename[5] = 0 + '0'; filename[6] = 5 + '0'; monat = 5; break;
                            case 6: filename[5] = 0 + '0'; filename[6] = 6 + '0'; monat = 6; break;
                            case 7: filename[5] = 0 + '0'; filename[6] = 7 + '0'; monat = 7; break;
                            case 8: filename[5] = 0 + '0'; filename[6] = 8 + '0'; monat = 8; break;
                            case 9: filename[5] = 0 + '0'; filename[6] = 9 + '0'; monat = 9; break;
                            case 10: filename[5] = 1 + '0'; filename[6] = 0 + '0'; monat = 10; break;
                            case 11: filename[5] = 1 + '0'; filename[6] = 1 + '0'; monat = 11; break;
                            case 12: filename[5] = 1 + '0'; filename[6] = 2 + '0'; monat = 12; break;
                            default : break;
                          }
                          if (SD.exists(filename))
                          {
                            Serial.println("file exists.");         // Check status
                          } else {
                            Serial.println("file not exists.");     // Check status
                          }
                         //*****************************************************************************************//

                          // Open or create file
                          logFile = SD.open(filename, FILE_WRITE);
                          logFile.print ("Datum   ");
                          logFile.print(" ");
                          logFile.print ("Zeit  ");
                          logFile.print("Anmeldung");
                          logFile.print (" ");
                          logFile.print ("Name   ");
                          logFile.println();
                          logFile.flush();
                        }
                        if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial() )
                        {
                            Serial.print("Gelesene UID:");                              // Show "Gelesene UID:" on Serial Monitor
                             for (int i = 0; i < mfrc522.uid.size; i++)                 // reads UID byt after byt in "for" loop
                                {
                                    Serial.print(mfrc522.uid.uidByte[i], HEX);          // Show uid nummer on Serial Monitor
                                    readCod = (readCod << 8) + mfrc522.uid.uidByte[i];  // converting an array of numbers to one number
                                 }

                          // **************** It reads new card code in to assign ******************//
                          //lcd.setCursor(0, 3);
                          //lcd.print(readCod, HEX);
                          //delay(10000);
                          //*************************************************************************//

                          int a; //temp variable
                          for (int i = 0; i < Person; i++)
                          {
                            if (Cards[i] == readCod)                                    // COMPARISON READ AND SAVED CODE
                            {
                              a = i;
                              if ((present[a] == 1 && come == 1) || (present[a] != 1 && leave == 1))    // status select
                              {
                                lcd.setCursor(0, 1);
                                lcd.print("Falsche        ");
                                lcd.setCursor(0, 2);
                                lcd.print("Anmeldung");
                                delay(1000);
                              }
                              else
                              {
                                value = EEPROM.read(i);        // read saved number in EEPROM address
                                /* Debug check
                                Serial.println(i);
                                Serial.print("/");
                                Serial.println(employee[i]);   // assigned name
                                Serial.print("Address: ");     // display the received data on the port monitor
                                Serial.print(String(i));
                                Serial.print(", value: ");
                                Serial.println(String(value));
                                */
                                if (come == 1)
                                {
                                  present[a] = 1;
                                }
                                else if (leave == 1)
                                {
                                  present[a] = 0;
                                }

                                zeit();
                                lcd.setCursor(0, 2);
                                if (come == 1)
                                {
                                  lcd.print ("Hallo  "); //greeting on LCD
                                      if (value != 0)
                                      {
                                         lcd.setCursor(0, 3);
                                         lcd.print ("Melden Sie sich"); // reminds, if someone hasn't "log out"
                                         EEPROM.update(i, 0);           // set value "0" this means jet not "log on"
                                         delay(2500);
                                         lcd.setCursor(0, 3);
                                         lcd.print ("               ");
                                      }
                                }
                                else if (leave == 1) {
                                  lcd.print ("Auf wiedersehen "); //goodbye on LCD
                                }
                                lcd.setCursor(0, 3);
                                lcd.print(employee[i]);
                                delay(2000);
                                DateTime now = rtc.now();
                                logFile.print(now.day(), DEC);
                                logFile.print('/');
                                logFile.print(now.month(), DEC);
                                logFile.print('/');
                                logFile.print(now.year(), DEC);
                                // logFile.print(',');
                                logFile.print(' ');
                                logFile.print(now.hour(), DEC);
                                logFile.print(':');
                                logFile.print(now.minute(), DEC);
                                // logFile.print (",");
                                logFile.print(' ');
                                if (come == 1) {
                                  logFile.print ("Kommen    ");
                                  EEPROM.update(i, 1);               //set value "1" this means "log on"
                                }
                                else if (leave == 1) {
                                  logFile.print ("Gehen     ");
                                  EEPROM.update(i, 0);               //set value "0" this means "log on"
                                }

                                //logFile.print (",");
                                logFile.print(' ');
                                logFile.print (employee[i]);
                                logFile.println();
                                logFile.flush();
                              }
                            }
                          }    lcd.clear();
                          // Versetzt die gelesene Karte in einen Ruhemodus, um nach anderen Karten suchen zu können.
                          mfrc522.PICC_HaltA();
                          registration = 1;
                        } //Serial.println();
          }//else second
        }
        lcd.clear();
        come = 0;
        leave = 0;
        registration = 0;
  }
  else
  {
    PORTC &= ~(1 << PC3); //LED off
   }
}

void zeit()
{

      zeit_setup();

      //*********************** First time setting *********************//
      /*
      RTC - When starting the device for the first time, you need delete  "//" befor "rtc.adjust", then
      Program the device with the correct time and then set "//" again and program
      (Time setting year, month, day, hours, minutes)
      */

      //rtc.adjust(DateTime(2020, 06, 27, 14, 33));
     //****************************************************************//
      DateTime now = rtc.now(); //time determination for RTC
      delay(1000);              //wait one Second

      lcd.setCursor(0, 0);      //setCursor specifies where the text should start. In this case the third character in the first row.
      if (now.hour() < 10)
        {
            lcd.print('0');
        }
      lcd.print(now.hour());    //The time should now be displayed here "hour"

      lcd.print(":");
      if (now.minute() < 10)
        {
            lcd.print('0');
        }
      lcd.print (now.minute()); //The time should now be displayed here "minute"
      lcd.print(" ");
      lcd.setCursor(6, 0);      // Der nächste Text soll nun beim zweiten Zeichen in der zweiten Reihe beginnen.
      if (now.day() < 10)
        {
            lcd.print('0');
        }
      lcd.print(now.day());     // The date should now be displayed here
      lcd.print("/");
      if (now.month() < 10)
        {
            lcd.print('0');
        }
      lcd.print(now.month());   // The Month should now be displayed here
      lcd.print("/");
      lcd.print(now.year());    // The Month should now be displayed here
  }


// ######################## Time Setup Funktion ############################//
void zeit_setup()
{
      DateTime now = rtc.now();
      //***************arrange wait time for time setup************//
      int registration_time = now.second();
      int waiting_time = registration_time + 7; //waiting time = 7s.
        if (registration_time >= 50)
        {
          waiting_time = waiting_time - 60;
        }
      //**********************************************************//
      int x = 0; //exit variable for while loop

      //***********premission_setup************//
      for (int i = 0; i < Person; i++)
      {
          if (present[i] == 1)
            {
                premission_setup = 1 ;
            }
      }
      //lcd.setCursor(0, 3);            //Debug for Setup premission
      //lcd.print(premission_setup);
     //***************************************//

      while ((PINC & 0x04 && x != 1))
      {
        if(premission_setup == 1)
        {
          x=1;
          break;;
        }
        if (PINC & 0x01)
        {
           for (int i=0; i<1; i++)
           {
             set_hour++;
             delay(1000);
             lcd.setCursor(0, 0);
             lcd.print(set_hour+1);
             if(set_hour > 23)
             {
               lcd.clear();
               set_hour=0;
             }
           }

        }
        else if (PINC & 0x02)
        {
           for (int i=0; i<1; i++)
           {
             set_minuts++;
             delay(500);
             lcd.setCursor(0, 0);
             lcd.print(set_minuts+1);
             if(set_minuts > 60)
             {
               lcd.clear();
               set_minuts=0;
             }
           }
        }
        if ((PINC & 0x04) && (now.second() > waiting_time) )
        {

          DateTime now = rtc.now();
          x=1;
          break;
        }
        else
        {
           rtc.adjust(DateTime(now.year(), now.month(), now.day(), set_hour, set_minuts));//RTC - einstellung Yahr, Monat, Tag, Stunden, Minuten
        }
      }
      premission_setup = 0;

}
//'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''//

//############################# Card Check Funktion #############################//
void Card_check()
{
  if (!card.init(SPI_HALF_SPEED, chipSelect))                   // here it is checked whether SD card is available
        {
            lcd.setCursor(0, 2);
            lcd.print("Check SD-Card.");                          // display error message Without SD-card
        }
  else
        {
            //     lcd.print("SD-Card is present.");              // debug
        }

}

void log_out_check()
{
  DateTime now = rtc.now();
  if (now.hour() == 20 && now.minute() == 00 && now.second() == 00 )
        {

             // if logout was forgotten - reset present
            for (int i = 0; i < Person; i++)                     // The for loop checks whether employees are "logged out".
                {
                    value = EEPROM.read(i);                      // read value "0" log out, "1" log on
                    if (value != 0)                              // if value is not equal to "0", info is saved in the data logger
                        {
                            logFile.print(now.day(), DEC);
                            logFile.print('/');
                            logFile.print(now.month(), DEC);
                            logFile.print('/');
                            logFile.print(now.year(), DEC);
                            // logFile.print(',');
                            logFile.print(' ');
                            logFile.print(17, DEC);
                            logFile.print(':');
                            logFile.print(01, DEC);
                            // logFile.print (",");
                            logFile.print(' ');
                            logFile.print ("Fehler    ");
                            logFile.print(' ');
                            logFile.print (employee[i]);
                            logFile.println();
                            logFile.flush();
                            present[i] = 0;
                        }
                  }
            }
}
//'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''//

