#include "Button.h"
#include "MinimalEEPROM.h"
#include "ConfigurationMode.h"

MinimalEEPROM eeprom;                       //Работа с памятью EEPROM
Button configButton(A1, Serial, 1500, 50);  // Кнопка на пине A1 - конфигурация, сброс
//Button mainButton(A0);    // Кнопка на пине A0 - старт счетчика

// Обьявляем модуль конфигурации с пинами для зелёного 8, красного 9 и зуммера 10
// НУЖНО БУДЕТ УКАЗАТЬ ПИНЫ ДЛЯ DIP ПЕРЕКЛЮЧАТЕЛЯ
ConfigurationMode configMode(configButton, eeprom, 8, 9, 10);

void setup() {
  Serial.begin(9600);
  while (!Serial)
    ;  // Ожидание USB-соединения

  Serial.println("Сетап старт");

  // Инициализация модуля конфигурации
  configMode.begin();


  Serial.println("Сетап стоп");
}

void loop() {

  configMode.update();
  if (configMode.isActive()) {
    return;  // корректно выходим в окно в режиме конфигурации
  }

  // еще не придумал


  if (configButton.isClick()) {
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