#ifndef MAINCOUNTERMODULE_H
#define MAINCOUNTERMODULE_H

#include <Arduino.h>
#include "Button.h"
#include "MinimalEEPROM.h"
#include "OperatingLevel.h"
#include "IndicationModule.h"

class MainCounterModule {
public:
  // Конструктор принимает: кнопку запуска, объект EEPROM, пины светодиодов/пьезо и объект индикации
  MainCounterModule(Button& counterButton, MinimalEEPROM& eeprom, uint8_t greenLed, uint8_t redLed, uint8_t buzzerPin, IndicationModule& indicationModule);

  // Инициализация пинов
  void begin();

  // Обновление логики, вызывается в каждом loop()
  void update();

private:
  Button& counterButton;
  MinimalEEPROM& eeprom;
  IndicationModule& indicationModule;  // Объект работы индикицией

  uint8_t pinGreen;
  uint8_t pinRed;
  uint8_t pinBuzzer;

  // Текущее состояние уровня (MaxOperatingTime / MaxReplacementTime / BLOCKED)
  OperatingLevel currentLevel;

  // Время старта текущей сессии (millis)
  uint32_t sessionStartMillis;

  // Время последнего сохранения в EEPROM
  uint32_t lastEepromSaveMillis;

  // Индикатор активной сессии (mainButton удерживается)
  bool sessionActive;

  // Локальная переменная текущего времени работы (в секундах)
  uint32_t memoryCurrentOperatingTime;

  // Флаги и переменные для неблокирующей задержки старта
  bool delayInProgress;
  uint32_t delayStartMillis;

  // Запуск новой сессии
  void startSession();

  // Завершение сессии (отпускание кнопки)
  void stopSession();

  // Основной обработчик сессии (выполняется при активной кнопке)
  void updateSession();

  // Управление светодиодами и пищалкой
  void setLedAndBuzzer(bool greenOn, bool redOn, bool buzzerOn);

  // Моргание красного + писк
  void blinkRedAndBeep();
};

#endif