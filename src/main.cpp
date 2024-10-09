#include "CONFIG.h"

// #include <esp_targets.h>
GButton btn_L(LEFT_BTN);
GButton btn_C(MIDDLE_BTN);
GButton btn_R(RIGTH_BTN);
GyverOLED<SSD1306_128x64, OLED_BUFFER> oled;

// int files_count = 0;
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
void print_addr(int char_select)
{
  // strtoul(ADDR, NULL, 16);
  if (char_select >= 0)
    oled.line(13 + char_select * 6, 39, 7 + 12 + char_select * 6, 39);
  // oled.roundRect(13+char_select*5,30, 7+12+char_select*5, 30+5);
  oled.setCursorXY(2, 30);
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
  btn_C.setClickTimeout(100);
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
  oled.line(0, 12, 127, 12);
  oled.roundRect(0, rect_pos * 12 + 14, 127, 14 + 12 + 12 * rect_pos, OLED_STROKE);
  // oled.setCursor(1, select * (-1));
  oled.setScale(1);

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
    oled.roundRect(0, 28, 63, 38, OLED_STROKE);
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

void setup()
{
  EEPROM.begin(1000);
  EEPROM.get(0, ADDR);
  init_btns();
  Wire.setClock(800000L);
  oled.init();
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
    ESPFlasherInit(true, &Serial); // sets up Serial communication to wifi module, with debug messages, to Print Class of your choice
    // ESPFlasherInit(true);     //sets up communication to wifi module, sets printing debug statements to Serial
    // ESPFlasherInit();          //sets up communication to wifi module, no debug messages
    // esp_loader_error_t err = ESPFlasherConnect();
    // if (err != ESP_LOADER_SUCCESS)
    // {
    // }
    // else
    // {
    //   Serial.println("ESP8266");
    //   ESPFlashBin("/firmware8266.bin", 0x00000);
    // }
  }
}

void loop()
{
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
        uint8_t err = flasing(buf, strtoul(ADDR, NULL, 16));
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
      page = 1;
      cursor = 2;
      Serial.print("page ");
      Serial.println(page);
      print_page_1(cursor);
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
  }
  if (btn_L.isSingle())
  {
    if (page == 0)
    {
      select_file--;
      if (select_file < 0)
        select_file = 0;
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
    // else if(page == 2)
  }
}
