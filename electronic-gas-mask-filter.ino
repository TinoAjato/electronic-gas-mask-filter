#include "Button.h"
#include "MinimalEEPROM.h"
#include "ConfigurationMode.h"

MinimalEEPROM eeprom;               //Работа с памятью EEPROM
Button configButton(A1, 1500, 50);  // Кнопка на пине A1 - конфигурация, сброс
//Button mainButton(A0);    // Кнопка на пине A0 - старт счетчика

// Массив пинов кнопок DIP-переключателя
uint8_t buttonPinsDIP[] = { A2, A3, A4, A5 };
// Обьявляем модуль конфигурации с пинами для зелёного 8, красного 9, зуммера 10 и массив для DIP-переключателя
ConfigurationMode configMode(configButton, eeprom, 8, 9, 10, buttonPinsDIP);

void setup() {
  Serial.begin(9600);
  while (!Serial)
    ;  // Ожидание USB-соединения

  // Инициализация модуля конфигурации
  configMode.begin();

  Serial.println("setup -> end");
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