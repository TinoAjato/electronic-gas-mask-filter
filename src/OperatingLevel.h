#ifndef OPERATING_LEVEL_H
#define OPERATING_LEVEL_H

#include <stdint.h>

// Возможные уровни работы устройства
enum class OperatingLevel : uint8_t {
  LEVEL_OPERATING = 1,    // Основная работа (MaxOperatingTime)
  LEVEL_REPLACEMENT = 2,  // Время замены фильтра (MaxReplacementTime)
  LEVEL_BLOCKED = 3       // Устройство заблокировано до сброса
};

#endif