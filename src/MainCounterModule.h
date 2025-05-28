#ifndef MAINCOUNTERMODULE_H
#define MAINCOUNTERMODULE_H

#include <Arduino.h>
#include "Button.h"
#include "MinimalEEPROM.h"
#include "OperatingLevel.h"
#include "IndicationModule.h"

class MainCounterModule {
public:
  MainCounterModule(Button& counterButton, MinimalEEPROM& eeprom, IndicationModule& indicationModule);
  void begin();
  void update();

private:
  //******* Внешние зависимости *******
  Button& counterButton;
  MinimalEEPROM& eeprom;
  IndicationModule& indicationModule;

  //******* Константы *******
  static constexpr uint32_t EEPROM_SAVE_INTERVAL_MS = 30000;
  static constexpr uint32_t START_BEEP_DURATION_MS = 200;

  //******* Состояние *******
  /*
   * Локальная переменная текущего времени работы (в секундах)
   */
  uint32_t memoryCurrentOperatingTime;
  /*
   * Время старта текущей сессии (millis)
   */
  uint32_t sessionStartMillis;
  /*
   * Время последнего сохранения в EEPROM
   */
  uint32_t lastEepromSaveMillis;

  /*
   * Индикатор запуска BEEP-а 1 раз, после отжатия counterButton
   */
  bool oneBeepFlag;

  //******* Менеджер сессии *******
  class SessionManager {
  public:
    void start(uint32_t now);
    uint32_t stop(uint32_t now);
    uint32_t elapsed(uint32_t now) const;
    bool isActive() const;

  private:
    uint32_t sessionStart = 0;
    /*
     * Индикатор активной сессии (counterButton удерживается)
     */
    bool active = false;
  };

  //******* Внутренние экземпляры *******
  SessionManager sessionManager;

  //******* Методы *******
  void handlePressed();
  void handleReleased();
  void updateIndication();
  void updateOperatingLevel(uint32_t totalTime, uint32_t maxOperating, uint32_t maxReplacement);
};

#endif