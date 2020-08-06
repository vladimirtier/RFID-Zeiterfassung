# RFID-Zeiterfassung
Time tracking project based on Arduino Uno
As a finished project we get an RFID time recording terminal for 7 cards with a data memory on the SD card and RTC (Real Time Clock).

The working time should be saved on the SD card. To do this we use the following:
1. Arduino Uno
2. RFID module - which reads the code from the ID card. The ID card with a number that was saved during production, namely UID (Unique Identifier)
3. This number is assigned to a person.
4. If you press one of three buttons that you have to choose from ("Coming" "Going" "Info"), you have 7 s to insert the ID card.
5. If the card has already been saved in the database, the time and name appear on the LCD display
6. At the same time, the log file is saved on the SD card. This file contains the following information: date, time, name, input / output
