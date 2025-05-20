#include "Button.h"
#include "MinimalEEPROM.h"

MinimalEEPROM eeprom;     //Работа с памятью EEPROM
Button configButton(A1);  // Кнопка на пине A1 - конфигурация, сброс
Button mainButton(A0);    // Кнопка на пине A0 - старт счетчика

void setup() {
  Serial.begin(9600);
}

void loop() {

  configButton.update();

  if (configButton.isClick()) {
    Serial.println(eeprom.getMaxReplacementTime());
  }

  if (configButton.isLongPress()) {
    Serial.println(eeprom.getMaxOperatingTime());
  }
}