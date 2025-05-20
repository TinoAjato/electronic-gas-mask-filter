#include "EEPROMTest.h"
#include <EEPROM.h>

void EEPROMTest::runAllTests() {
  serial.println("Запуск тестов EEPROM");

  testConstStorage();
  testCurrentTimeStorage();
  testCRCValidation();

  serial.println("Тесты завершены");
}

void EEPROMTest::testConstStorage() {
  serial.println("Константы");

  eeprom.saveMaxOperatingTime(123456);
  eeprom.saveMaxReplacementTime(654321);

  uint32_t opTime = eeprom.getMaxOperatingTime();
  uint32_t repTime = eeprom.getMaxReplacementTime();

  serial.print("  Ожидается 123456: "); serial.println(opTime);
  serial.print("  Ожидается 654321: "); serial.println(repTime);

  if (opTime != 123456 || repTime != 654321)
    serial.println("Ошибка в сохранении констант");
  else
    serial.println("Успешно");
}

void EEPROMTest::testCurrentTimeStorage() {
  serial.println("Текущая наработка");

  eeprom.saveCurrentOperatingTime(42);
  uint32_t val = eeprom.getCurrentOperatingTime();

  serial.print("  Ожидается 42: "); serial.println(val);

  if (val != 42)
    serial.println("Ошибка в сохранении текущей наработки");
  else
    serial.println("Успешно");
}

void EEPROMTest::testCRCValidation() {
  serial.println("Повреждение CRC");

  // Сохраняем нормальное значение
  eeprom.saveCurrentOperatingTime(99);

  // Повреждаем контрольную сумму
  EEPROM.update(12, 0x00); // CRC байт 1
  EEPROM.update(13, 0x00); // CRC байт 2

  uint32_t restored = eeprom.getCurrentOperatingTime();

  serial.print("  После повреждения CRC: "); serial.println(restored);
  if (restored != 0xFFFFFFFF)
    serial.println("Ошибка: данные не сброшены при неверной CRC");
  else
    serial.println("Успешно: данные сброшены из-за CRC");
}