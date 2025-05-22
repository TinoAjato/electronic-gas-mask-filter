#include "Button.h"
#include "MinimalEEPROM.h"
#include "ConfigurationMode.h"

//******* Работа с памятью EEPROM *******
MinimalEEPROM eeprom;

//******* МОДУЛЬ КОНФИГУРАЦИИ *******
// Кнопка - конфигурация, сброс
Button configButton(3, 1500, 50);
// Массив пинов кнопок DIP-переключателя
uint8_t buttonPinsDIP[] = { 7, 6, 5, 4 };
// Обьявляем модуль конфигурации с пинами для зелёного, красного, зуммера и массив для DIP-переключателя
ConfigurationMode configurationModule(configButton, eeprom, 11, 12, 13, buttonPinsDIP);

//******* ГЛАВНЫЙ МОДУЛЬ *******
// Кнопка - старт счетчика
Button mainButton(2);

void setup() {
  Serial.begin(9600);
  while (!Serial)
    ;  // Ожидание USB-соединения

  // Инициализация модуля конфигурации
  configurationModule.begin();
}

void loop() {

  // Обрабатываем конфигурацию
  configurationModule.update();
  if (configurationModule.isActive()) {
    return;  // корректно выходим в окно в режиме конфигурации
  }

  // еще не придумал


  mainButton.update();
  if (mainButton.isClick()) {
    Serial.print("Максимальное время работы в секундах: ");
    uint32_t val = eeprom.getMaxOperatingTime();
    Serial.print(val);
    Serial.print("; В минутах: ");
    Serial.println((uint32_t)val / 60);

    Serial.print("Максимальное время замены фильтра в секундах: ");
    val = eeprom.getMaxReplacementTime();
    Serial.print(val);
    Serial.print("; В минутах: ");
    Serial.println((uint32_t)val / 60);

    Serial.print("Текущее накопленное время работы в секундах: ");
    val = eeprom.getCurrentOperatingTime();
    Serial.print(val);
    Serial.print("; В минутах: ");
    Serial.println((uint32_t)val / 60);
  }
}