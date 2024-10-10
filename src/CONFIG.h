//Libs
#include <ESPSerialFlasher.h>
#include <SD.h>
#include <GyverOLED.h>
#include <Wire.h>
#include <GyverButton.h>
#include <EEPROM.h>
#include <driver/adc.h>
#include <esp_adc_cal.h>

float Q = 0.125;  // Процессный шум (чем больше, тем быстрее реагирует на изменения)
float R = 4;      // Измерительный шум (чем меньше, тем больше доверие к измерениям)
float P = 1;      // Оценка ошибки
float K = 0.0;    // Коэффициент Калмана
float U_hat = 0.0; // Предыдущее состояние (оценка напряжения)

float KalmanFilter(float U) {
  // Шаг предсказания: предсказанное значение остаётся равным предыдущему
  // P предсказанное (ошибка предсказания)
  P = P + Q;

  // Шаг обновления: вычисляем коэффициент Калмана
  K = P / (P + R);

  // Обновляем оценку напряжения с учётом измеренного значения
  U_hat = U_hat + K * (U - U_hat);

  // Обновляем ошибку P
  P = (1 - K) * P;

  // Возвращаем скорректированное значение
  return U_hat;
}

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

