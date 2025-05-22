#ifndef CONFIGURATIONMODE_H
#define CONFIGURATIONMODE_H

#include <Arduino.h>
#include "Button.h"
#include "MinimalEEPROM.h"

#define MAX_PINS_SIZE 4  // Размер массива пинов DIP-переключателя

// Управляет режимом настройки параметров:
// - MaxOperatingTime (в секундах, диапазон 1–15 минут)
// - MaxReplacementTime (в секундах, диапазон 5–75 секунд с шагом 5).
class ConfigurationMode {
public:
  // Конструктор: принимает ссылку на объект кнопки, объект EEPROM,
  // а также номера пинов зелёного светодиода, красного светодиода, пищалки и пинов DIP-переключателя
  ConfigurationMode(Button &configBtn, MinimalEEPROM &eeprom,
                    uint8_t greenLedPin, uint8_t redLedPin, uint8_t buzzerPin, const uint8_t (&pins)[MAX_PINS_SIZE]);

  // установка режимов пинов
  void begin();

  // Обновляет состояние кнопки и обрабатывает режим конфигурации
  void update();

  // Включен ли режим конфигурирования
  bool isActive() const;

private:
  Button &button;               // Кнопка конфигурации
  MinimalEEPROM &eeprom;        // Объект работы с EEPROM
  uint8_t ledGreenPin;          // Пин зелёного светодиода
  uint8_t ledRedPin;            // Пин красного светодиода
  uint8_t buzzerPin;            // Пин пищалки
  uint8_t pins[MAX_PINS_SIZE];  // Пины DIP-переключателя

  // Период мигания (мс)
  static const int BLINK_INTERVAL = 500;

  bool configModeActive;  // Флаг: включен ли режим конфигурирования
  uint8_t level;          // Текущий уровень (1 или 2)

  unsigned long lastBlinkTime;  // Время последней смены состояния светодиода (для мигания)
  bool ledState;                // Текущее состояние светодиода (включен/выключен)

  // Читает значение DIP, возвращает число 0–15
  uint8_t readDIP();

  // Сохраняет параметр EEPROM в зависимости от текущего уровня и считанного DIP:
  // Level 1 -> MaxOperatingTime
  // Level 2 -> MaxReplacementTime
  void saveCurrentLevelValue(uint8_t dipValue);
};

#endif  // CONFIGURATIONMODE_H