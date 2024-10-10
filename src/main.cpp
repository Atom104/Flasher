#include "CONFIG.h"

// #include <esp_targets.h>
GButton btn_L(LEFT_BTN);
GButton btn_C(MIDDLE_BTN);
GButton btn_R(RIGTH_BTN);
GyverOLED<SSD1306_128x64, OLED_BUFFER> oled;

// int files_count = 0;

uint8_t bat_level;
uint8_t last_bat_level;
int select_file = 0;
int max_select_file = 0;
int page = 0;
String files[2000];
uint8_t chars[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
int cursor = 2; // 2 - на кнопке запуск, 1 - на адрес, 0 - на кнопке назад
String str;
char ADDR[9] = "00010000";
int select_char_in_array = 0;
int CHAR_select = -1;

void IRAM_ATTR interrupt()
{
  btn_L.tick();
  btn_C.tick();
  btn_R.tick();
}

int x_dop = 32;
void print_addr(int char_select)
{
  // strtoul(ADDR, NULL, 16);
  if (char_select >= 0)
    oled.line(x_dop + 13 + char_select * 6, 39, x_dop + 7 + 12 + char_select * 6, 39);
  // oled.roundRect(13+char_select*5,30, 7+12+char_select*5, 30+5);
  oled.setCursorXY(2 + x_dop, 30);
  oled.print("0x");
  oled.print(ADDR);
}

void tick_flash(int prg)
{
  // Serial.print("Prpgress...");
  if (prg % 10 == 0)
  {
    oled.clear(64, 5, 127, 12);
    oled.setCursorXY(73, 5);
    oled.print(prg);
    oled.rect(0, 15, map(prg, 0, 100, 0, 127), 30, OLED_FILL);
    oled.update();
  }
  Serial.println(prg);
}

uint8_t flasing(const char *file_name, size_t addr)
{
  esp_loader_error_t err = ESPFlasherConnect();
  if (err != ESP_LOADER_SUCCESS)
  {
    return CONNECT_ERROR;
  }
  else
  {
    // Serial.println("ESP8266");
    ESPFlashBin(file_name, addr);
    return CONNECT_SUCCES;
  }
}

void init_btns()
{
  pinMode(LEFT_BTN, INPUT_PULLUP);
  pinMode(MIDDLE_BTN, INPUT_PULLUP);
  pinMode(RIGTH_BTN, INPUT_PULLUP);
  btn_L.setDebounce(50); // настройка антидребезга (по умолчанию 80 мс)
  btn_L.setTimeout(300); // настройка таймаута на удержание (по умолчанию 500 мс)
  btn_L.setClickTimeout(50);
  btn_C.setDebounce(50); // настройка антидребезга (по умолчанию 80 мс)
  btn_C.setTimeout(300); // настройка таймаута на удержание (по умолчанию 500 мс)
  btn_C.setClickTimeout(250);
  btn_R.setDebounce(50); // настройка антидребезга (по умолчанию 80 мс)
  btn_R.setTimeout(300); // настройка таймаута на удержание (по умолчанию 500 мс)
  btn_R.setClickTimeout(50);
}

// int last_start_list = -1;
void print_files(int select)
{
  oled.invertDisplay(false);
  int y_pos = 0;
  oled.clear();
  int rect_pos = 0;
  if (select > 3)
    rect_pos = (select) % 4;
  else
    rect_pos = select;
  oled.setScale(1);

  oled.rect(126, 3, 127, 5, OLED_FILL);
  oled.rect(114, 1, 126, 7, OLED_STROKE);
  oled.rect(115, 2, bat_level + 115, 6, OLED_FILL);

  oled.setCursorXY(4, 3);
  oled.print("i");

  oled.line(0, 12, 127, 12);
  if (select > -1)
    oled.roundRect(0, rect_pos * 12 + 14, 127, 14 + 12 + 12 * rect_pos, OLED_STROKE);
  else
  {
    oled.roundRect(0, 0, 11, 11, OLED_STROKE);
  }
  // oled.setCursor(1, select * (-1));

  int start_list = 4 * ((select) / 4);
  int end_list;
  if (max_select_file - select < 4 && max_select_file - start_list < 4)
    end_list = max_select_file;
  else
    end_list = start_list + 4;

  for (int i = start_list; i < end_list; i++)
  {
    oled.setCursorXY(1, y_pos + 2 + 14);
    y_pos += 12;
    oled.print(files[i]);
  }

  oled.update();
}

bool Auto_mode = 0;
void print_page_1(int select)
{
  str = "/" + files[select_file];
  oled.clear();
  if (select == 0)
  {
    oled.roundRect(0, 0, 9, 10, OLED_STROKE);
  }
  if (select == 2)
  {
    oled.roundRect(38, 48, 89, 62, OLED_STROKE);
  }
  if (select == 1)
  {
    oled.roundRect(0 + x_dop, 28, 63 + x_dop, 38, OLED_STROKE);
  }
  print_addr(CHAR_select);
  oled.setCursorXY(50, 52);
  oled.print("START");
  oled.setCursorXY(2, 2);
  oled.print("<");
  oled.setCursorXY(10, 1);
  oled.print(str);
  oled.update();
}
int info_select = 0;
void print_info()
{
  oled.clear();
  if (info_select == 0)
  {
    oled.roundRect(0, 0, 9, 10, OLED_STROKE);
  }
  else if (info_select == 1)
  {
    oled.roundRect(0, 13, 76, 13 + 10, OLED_STROKE);
  }
  else if (info_select == 2)
  {
    oled.roundRect(0, 27, 63, 27 + 10, OLED_STROKE);
  }
  else if (info_select == 3)
  {
    oled.roundRect(0, 41, 40, 41 + 10, OLED_STROKE);
  }
  oled.setCursorXY(2, 2);
  oled.print("<");
  oled.setCursorXY(2, 29);
  oled.print("Board info");
  oled.setCursorXY(2, 43);
  oled.print("EARASE");
  oled.setCursorXY(2, 15);
  oled.print("Auto mode");
  oled.roundRect(63, 15, 63 + 12, 21, OLED_STROKE);
  if (Auto_mode)
    oled.roundRect(64 + 5, 16, 64 + 10, 20, OLED_FILL);
  else
    oled.roundRect(64, 16, 64 + 5, 20, OLED_FILL);
  oled.update();
}

void setup()
{
  attachInterrupt(1, interrupt, CHANGE);
  pinMode(BAT_LEVEL_PIN, INPUT);
  EEPROM.begin(1000);
  EEPROM.get(0, ADDR);
  EEPROM.get(10, Auto_mode);
  init_btns();
  oled.init();
  Wire.setClock(800000L);
  oled.clear();
  oled.flipV(true);
  oled.flipH(true);
  oled.textMode(BUF_ADD);
  oled.setCursorXY(19, 24);
  oled.setScale(2);
  oled.invertDisplay(true);
  oled.print("FLASHER");
  oled.update();
  Serial.begin(115200); // Start communication With IDE to see whats going on
  delay(1000);          // wait 5 seconds before atarting
  if (SD.begin(5))      // Must begin SD CARD before trying to flash Wifi module, will fail if not connected
  {
    File root = SD.open("/");
    File file = root.openNextFile();
    max_select_file = 0;
    while (file)
    {
      files[max_select_file] = file.name();
      max_select_file++;
      file = root.openNextFile();
    }
    print_files(select_file);
    // ESPFlasherInit(true, &Serial);
  }
}

void loop()
{
  // Serial.print(bat_level);
  // Serial.print("    ");
  // Serial.print(analogRead(BAT_LEVEL_PIN));
  // Serial.print("    ");
  // Serial.println(map(analogRead(BAT_LEVEL_PIN), 3250, 3500, 0, 100));

  btn_L.tick();
  btn_C.tick();
  btn_R.tick();
  if (btn_C.isSingle())
  {
    // Serial.println("btn_c click ");
    if (page == 1)
    {
      if (cursor == 2)
      {
        oled.clear();
        oled.setScale(1);
        oled.setCursorXY(5, 5);
        oled.print("Flashing...");
        oled.rect(0, 15, 127, 30, OLED_STROKE);
        oled.update();
        char buf[255];
        str.toCharArray(buf, 255);
        Serial.print("0x");
        Serial.println(strtoul(ADDR, NULL, 16), HEX);
        uint8_t err;
        if (Auto_mode)
        {
          ESPFlasherInit(true, &Serial);
          // ESPFlasherConnect();
          err = flasing("/bootloader.bin", 0x1000);
          oled.clear();
          oled.setCursorXY(5, 5);
          oled.print("Flashing...");
          oled.rect(0, 15, 127, 30, OLED_STROKE);
          ESPFlasherInit(true, &Serial);
          // ESPFlasherConnect();
          err = flasing("/partitions.bin", 0x8000);
          oled.clear();
          oled.setCursorXY(5, 5);
          oled.print("Flashing...");
          oled.rect(0, 15, 127, 30, OLED_STROKE);
        }
        ESPFlasherInit(true, &Serial);
        // ESPFlasherConnect();
        err = flasing(buf, strtoul(ADDR, NULL, 16));
        if (err == CONNECT_ERROR)
        {
          Serial.println("Connect error");
          // oled.init();
          oled.clear();
          oled.setScale(1);
          oled.setCursorXY(20, 27);
          oled.print("Connect error!");
          oled.update();
        }
        else
        {
          oled.clear();
          oled.setScale(1);
          oled.setCursorXY(40, 27);
          oled.print("SUCCESS");
          oled.update();
          delay(500);
          print_page_1(cursor);
        }
      }
      else if (cursor == 0)
      {
        page = 0;
        print_files(select_file);
      }
      else if (cursor == 1)
      {
        CHAR_select++;
        for (int i = 0; i < 16; i++)
        {
          if (chars[i] == ADDR[CHAR_select])
          {
            select_char_in_array = i;
            break;
          }
        }

        if (CHAR_select == 8)
        {
          CHAR_select = -1;
          EEPROM.put(0, ADDR);
          EEPROM.commit();
        }
        if (CHAR_select == -1)
          print_page_1(cursor);
        else
          print_page_1(-1);
      }
    }
    else if (page == 0)
    {
      if (select_file > -1)
      {
        page = 1;
        cursor = 2;
        Serial.print("page ");
        Serial.println(page);
        print_page_1(cursor);
      }
      else if (select_file == -1)
      {
        page = 3;
        print_info();
      }
    }
    else if (page == 3)
    {
      if (info_select == 0)
      {
        page = 0;
        print_files(select_file);
      }
      else if (info_select == 1)
      {
        Auto_mode = !Auto_mode;
        EEPROM.put(10, Auto_mode);
        EEPROM.commit();
        print_info();
      }
      else if (info_select == 3)
      {
        oled.clear();
        oled.setCursorXY(29, 27);
        oled.print("ERASE FLASH!");
        oled.update();
        ESPFlasherInit(true, &Serial);
        esp_loader_error_t err = ESPFlasherConnect();
        if (err == ESP_LOADER_SUCCESS)
        {
          Erase_flash();
          oled.clear();
          oled.setCursorXY(40, 27);
          oled.print("SUCCESS!");
          oled.update();
          delay(1000);
        }
        else
        {
          Serial.println("Connect error");
          // oled.init();
          oled.clear();
          oled.setScale(1);
          oled.setCursorXY(20, 27);
          oled.print("Connect error!");
          oled.update();
          delay(1500);
        }
        print_info();
      }
    }
  }

  if (btn_R.isSingle())
  {
    if (page == 0)
    {
      select_file++;
      if (select_file >= max_select_file)
        select_file = max_select_file - 1;
      print_files(select_file);
    }
    else if (page == 1)
    {
      if (CHAR_select == -1)
      {
        cursor++;
        if (cursor > 2)
          cursor = 2;
        print_page_1(cursor);
      }
      else
      {
        select_char_in_array++;
        if (select_char_in_array > 15)
          select_char_in_array = 15;
        ADDR[CHAR_select] = chars[select_char_in_array];
        print_page_1(-1);
      }
    }
    else if (page == 3)
    {
      info_select++;
      if (info_select > 3)
        info_select = 3;
      print_info();
    }
  }

  if (btn_L.isSingle())
  {
    if (page == 0)
    {
      select_file--;
      if (select_file < -1)
        select_file = -1;
      print_files(select_file);
    }
    else if (page == 1)
    {
      // page = 0;
      // print_files(select_file);
      if (CHAR_select == -1)
      {
        cursor--;
        if (cursor < 0)
          cursor = 0;
        print_page_1(cursor);
      }
      else
      {
        select_char_in_array--;
        if (select_char_in_array < 0)
          select_char_in_array = 0;
        ADDR[CHAR_select] = chars[select_char_in_array];
        print_page_1(-1);
      }
    }
    else if (page == 3)
    {
      info_select--;
      if (info_select < 0)
        info_select = 0;
      print_info();
    }
  }

  if (btn_C.isDouble())
  {
    if (page == 1)
    {
      if (cursor == 1)
      {
        CHAR_select--;
        print_page_1(-1);
      }
    }
  }
  if (!digitalRead(LEFT_BTN) && !digitalRead(RIGTH_BTN))
  {
    oled.clear();
    oled.setCursorXY(40, 20);
    oled.print("EEPROM");
    oled.setCursorXY(40, 20 + 14);
    oled.print("CLEAN!");
    oled.update();
    for (int i = 0; i < 8; i++)
    {
      ADDR[i]='0';
      EEPROM.put(0, ADDR);
    }
    Auto_mode = 0;
    EEPROM.put(10, Auto_mode);
    // for (int i = 0; i < 100; i++)
    // {
    //   EEPROM.put(i, 0);
    // }
    EEPROM.commit();
    delay(1500);
    if (page == 0)
      print_files(select_file);
    else if (page == 1)
      print_page_1(cursor);
    else if (page == 3)
      print_info();
  }

  bat_level = constrain(KalmanFilter(map(analogRead(BAT_LEVEL_PIN), 3250, 3550, 0, 10)), 0, 10);
  // Serial.print(bat_level);
  // Serial.print("    ");
  // Serial.print(analogRead(BAT_LEVEL_PIN));
  // Serial.print("    ");
  // Serial.println(map(analogRead(BAT_LEVEL_PIN), 3250, 3500, 0, 100));
  if (page == 0)
  {
    if (last_bat_level != bat_level)
    {
      print_files(select_file);
      last_bat_level = bat_level;
    }
  }
}
