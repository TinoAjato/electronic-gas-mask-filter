#include "src/Button.h"
#include "src/MinimalEEPROM.h"
#include "src/ConfigurationModule.h"
#include "src/MainCounterModule.h"
#include "src/IndicationModule.h"

//******* Работа с памятью EEPROM *******
MinimalEEPROM eeprom;

//******* МОДУЛЬ ИНДИКАЦИИ *******
// Объявляем модуль индикации со светодиодами (green, red) и пищалкой (buzzer)
IndicationModule indicationModule(11, 12, 13);

//******* МОДУЛЬ КОНФИГУРАЦИИ *******
// Кнопка - конфигурация, сброс
Button configurationButton(3, 1500, 50);
// Массив пинов DIP-переключателя
uint8_t DIPPins[] = { 7, 6, 5, 4 };
// Обьявляем модуль конфигурации
ConfigurationModule configurationModule(configurationButton, eeprom, DIPPins, indicationModule);

//******* ГЛАВНЫЙ МОДУЛЬ *******
// Кнопка - старт счетчика
Button counterButton(2);
// Объявляем модуль счетчика
MainCounterModule mainCounterModule(counterButton, eeprom, indicationModule);

void setup() {
  // Инициализация модуля индикации
  indicationModule.begin();
  // Инициализация модуля конфигурации
  configurationModule.begin();
  // Инициализация модуля счетчика
  mainCounterModule.begin();
}

void loop() {
  // Обновление состояния индикаторов
  indicationModule.update();

  // Обрабатываем конфигурацию
  configurationModule.update();
  if (configurationModule.isActive()) {
    return;  // корректно выходим в окно в режиме конфигурации
  }

  // Обрабатываем счетчик
  mainCounterModule.update();
}