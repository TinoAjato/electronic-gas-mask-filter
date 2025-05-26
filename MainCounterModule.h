#ifndef MAINCOUNTERMODULE_H
#define MAINCOUNTERMODULE_H

#include "Button.h"
#include "MinimalEEPROM.h"

// Возможные уровни работы устройства
enum OperatingLevel {
  LEVEL_NONE = 0,     // Не работаем (кнопка не нажата)
  LEVEL_OPERATING,    // Основная работа (MaxOperatingTime)
  LEVEL_REPLACEMENT,  // Время замены фильтра (MaxReplacementTime)
  LEVEL_BLOCKED       // Устройство заблокировано до сброса
};

class MainCounterModule {
public:
  // Конструктор принимает: кнопку запуска, объект EEPROM и пины светодиодов/пьезо
  MainCounterModule(Button& btn, MinimalEEPROM& eeprom, uint8_t greenLed, uint8_t redLed, uint8_t buzzerPin);

  // Инициализация пинов
  void begin();

  // Обновление логики, вызывается в каждом loop()
  void update();

private:
  Button& mainButton;
  MinimalEEPROM& eeprom;

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