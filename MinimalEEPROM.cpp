#include "HardwareSerial.h"
#include "MinimalEEPROM.h"

// Заглушка
void MinimalEEPROM::begin() {
  // инициализациz EEPROM
}

// Вычисление CRC-16 (алгоритм Modbus)
// Используется для проверки целостности записанных в EEPROM данных
uint16_t MinimalEEPROM::crc16(const uint8_t* data, size_t length) {
  uint16_t crc = 0xFFFF;  // Начальное значение CRC
  for (size_t i = 0; i < length; i++) {
    crc ^= data[i];  // XOR байта с младшим байтом CRC
    for (int j = 0; j < 8; j++) {
      if (crc & 1) {
        crc = (crc >> 1) ^ 0xA001;  // Если младший бит установлен — сдвигаем и XOR с полиномом
      } else {
        crc >>= 1;  // Иначе просто сдвиг
      }
    }
  }
  return crc;
}

// Сохраняем "максимальное время работы"
// Только если значение изменилось, чтобы избежать лишнего износа EEPROM
void MinimalEEPROM::saveMaxOperatingTime(uint32_t seconds) {
  uint32_t existing;
  EEPROM.get(ADDR_MAX_OPERATING, existing);
  if (existing != seconds) {
    EEPROM.put(ADDR_MAX_OPERATING, seconds);  // Запись 4 байтов
  }
}

// Сохраняем "максимальное время до замены фильтра"
void MinimalEEPROM::saveMaxReplacementTime(uint32_t seconds) {
  uint32_t existing;
  EEPROM.get(ADDR_MAX_REPLACEMENT, existing);
  if (existing != seconds) {
    EEPROM.put(ADDR_MAX_REPLACEMENT, seconds);
  }
}

// Сохраняем "текущее накопленное время работы"
// Вместе с CRC16 для защиты от глюков
void MinimalEEPROM::saveCurrentOperatingTime(uint32_t seconds) {
  uint32_t existing;
  EEPROM.get(ADDR_CURRENT_TIME, existing);

  // Записываем только при изменении, чтобы не изнашивать EEPROM
  if (existing != seconds) {
    EEPROM.put(ADDR_CURRENT_TIME, seconds);                     // Сохраняем значение
    uint16_t crc = crc16((uint8_t*)&seconds, sizeof(seconds));  // Вычисляем CRC
    EEPROM.put(ADDR_CURRENT_TIME_CRC, crc);                     // Записываем CRC после данных
  }
}

// Чтение "максимального времени работы"
uint32_t MinimalEEPROM::getMaxOperatingTime() {
  uint32_t val;
  EEPROM.get(ADDR_MAX_OPERATING, val);
  return val;
}

// Чтение "максимального времени до замены фильтра"
uint32_t MinimalEEPROM::getMaxReplacementTime() {
  uint32_t val;
  EEPROM.get(ADDR_MAX_REPLACEMENT, val);
  return val;
}

// Чтение "текущего времени работы"
// Если контрольная сумма не совпадает — возвращаем большое число
uint32_t MinimalEEPROM::getCurrentOperatingTime() {
  uint32_t val;
  uint16_t stored_crc;

  // Читаем значение и CRC
  EEPROM.get(ADDR_CURRENT_TIME, val);
  EEPROM.get(ADDR_CURRENT_TIME_CRC, stored_crc);

  // Считаем CRC от прочитанного значения
  uint16_t computed_crc = crc16((uint8_t*)&val, sizeof(val));

  Serial.print("CURRENT_TIME");
  Serial.println(val);
  Serial.print("computed_crc == stored_crc? -> ");
  Serial.println(computed_crc == stored_crc);

  // Если CRC совпало — возвращаем значение, иначе — сброс
  if (computed_crc == stored_crc) {
    return val;
  } else {
    return 0xFFFFFFFF;  // Повреждение или данные были некорректно записаны
  }
}