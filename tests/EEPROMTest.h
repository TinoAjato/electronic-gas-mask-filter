#ifndef EEPROM_TEST_H
#define EEPROM_TEST_H

#include "MinimalEEPROM.h"

class EEPROMTest {
public:
  EEPROMTest(MinimalEEPROM& eeprom, HardwareSerial& serial)
    : eeprom(eeprom), serial(serial) {}

  void runAllTests();

private:
  MinimalEEPROM& eeprom;
  HardwareSerial& serial;

  void testConstStorage();
  void testCurrentTimeStorage();
  void testCRCValidation();
};

#endif