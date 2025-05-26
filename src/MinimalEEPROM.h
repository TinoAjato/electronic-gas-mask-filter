#ifndef MINIMAL_EEPROM_H
#define MINIMAL_EEPROM_H

#include <Arduino.h>
#include <EEPROM.h>
#include "OperatingLevel.h"

// Класс для минимальной работы с EEPROM
class MinimalEEPROM {
public:
  void begin();

  // Сохраняем максимальное время работы (в секундах)
  void saveMaxOperatingTime(uint32_t seconds);

  // Сохраняем максимальное время замены фильтра (в секундах)
  void saveMaxReplacementTime(uint32_t seconds);

  // Сохраняем текущее накопленное время работы (в секундах) + контрольная сумма
  void saveCurrentOperatingTime(uint32_t seconds);

  // Сохраняем уровень работы
  void saveOperatingLevel(OperatingLevel level);

  // Получаем максимальное время работы (в секундах)
  uint32_t getMaxOperatingTime();

  // Получаем максимальное время до замены фильтра (в секундах)
  uint32_t getMaxReplacementTime();

  // Получаем текущее накопленное время работы (в секундах)
  // Если контрольная сумма не совпала — возвращает большое число
  uint32_t getCurrentOperatingTime();

  // Получаем  уровень работы
  OperatingLevel getOperatingLevel();

private:
  // Смещения в EEPROM, по которым хранятся значения:
  static const int ADDR_MAX_OPERATING = 0;      // 4 байта
  static const int ADDR_MAX_REPLACEMENT = 4;    // 4 байта
  static const int ADDR_CURRENT_TIME = 8;       // 4 байта
  static const int ADDR_CURRENT_TIME_CRC = 12;  // 2 байта CRC
  static const int ADDR_OPERATING_LEVEL = 14;   // 1 байт

  // CRC-16 по алгоритму Modbus (полином 0xA001), для проверки корректности данных
  uint16_t crc16(const uint8_t* data, size_t length);
};

#endif