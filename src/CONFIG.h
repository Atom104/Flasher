//Libs
#include <ESPSerialFlasher.h>
#include <SD.h>
#include <GyverOLED.h>
#include <Wire.h>
#include <GyverButton.h>
#include <EEPROM.h>

//pinout
#define LEFT_BTN 25
#define MIDDLE_BTN 33
#define RIGTH_BTN 32
#define BAT_LEVEL_PIN 34

//errors code
#define CONNECT_SUCCES 0
#define CONNECT_ERROR 1

#define MENU_PAGE 0
#define SETTINGS_PAGE 1
#define ADDRES_PAGE 2

