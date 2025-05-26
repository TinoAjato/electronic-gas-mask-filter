#ifndef CONFIGURATIONMODULE_H
#define CONFIGURATIONMODULE_H

#include <Arduino.h>
#include "Button.h"
#include "MinimalEEPROM.h"
#include "IndicationModule.h"

#define MAX_PINS_SIZE 4  // Размер массива пинов DIP-переключателя

// Управляет режимом настройки параметров:
// - MaxOperatingTime (в секундах, диапазон 1–15 минут)
// - MaxReplacementTime (в секундах, диапазон 5–75 секунд с шагом 5).
class ConfigurationModule {
public:
  // Конструктор: принимает ссылку на объект кнопки, объект EEPROM, массив пинов DIP-переключателя и объект индикации
  ConfigurationModule(Button &configurationButton, MinimalEEPROM &eeprom, const uint8_t (&pins)[MAX_PINS_SIZE], IndicationModule &indicationModule);

  // установка режимов пинов
  void begin();

  // Обновляет состояние кнопки и обрабатывает режим конфигурации
  void update();

  // Включен ли режим конфигурирования
  bool isActive() const;

private:
  Button &configurationButton;         // Кнопка конфигурации
  MinimalEEPROM &eeprom;               // Объект работы с EEPROM
  IndicationModule &indicationModule;  // Объект работы индикицией
  uint8_t pins[MAX_PINS_SIZE];          // Пины DIP-переключателя

  /**
    * Период мигания (мс)
    */
  static const int BLINK_INTERVAL = 500;

  bool configModeActive;  // Флаг: включен ли режим конфигурирования
  uint8_t level;          // Текущий уровень (1 или 2)

  // Читает значение DIP, возвращает число 0–15
  uint8_t readDIP();

  // Сохраняет параметр EEPROM в зависимости от текущего уровня и считанного DIP:
  // Level 1 -> MaxOperatingTime
  // Level 2 -> MaxReplacementTime
  void saveCurrentLevelValue(uint8_t dipValue);
};

#endif