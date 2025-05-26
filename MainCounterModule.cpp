#include "src/MainCounterModule.h"

MainCounterModule::MainCounterModule(Button& counterButton, MinimalEEPROM& eeprom, uint8_t greenLed, uint8_t redLed, uint8_t buzzer, IndicationModule& indicationModule)
  : counterButton(counterButton), eeprom(eeprom), pinGreen(greenLed), pinRed(redLed), pinBuzzer(buzzer),
    currentLevel(OperatingLevel::LEVEL_OPERATING), sessionActive(false), memoryCurrentOperatingTime(0),
    delayInProgress(false), delayStartMillis(0), sessionStartMillis(0), lastEepromSaveMillis(0), indicationModule(indicationModule) {}

void MainCounterModule::begin() {
  indicationModule.ALL_OFF();

  pinMode(pinGreen, OUTPUT);
  pinMode(pinRed, OUTPUT);
  pinMode(pinBuzzer, OUTPUT);

  // Инициализируем уровень и восстановленное время
  memoryCurrentOperatingTime = eeprom.getCurrentOperatingTime();

  // Читаем уровень из EEPROM
  currentLevel = eeprom.getOperatingLevel();

  Serial.print("MemoryCurrentOperatingTime: ");
  Serial.println(memoryCurrentOperatingTime);
  Serial.print("CurrentLevel: ");
  Serial.println((int)currentLevel);
}

void MainCounterModule::update() {
  counterButton.update();

  // Читаем уровень из EEPROM
  currentLevel = eeprom.getOperatingLevel();

  if (counterButton.isPressed()) {
    if (!sessionActive) {
      startSession();
    }
    updateSession();
  } else {
    if (sessionActive) {
      stopSession();
    }

    // Если заблокировано — продолжаем пищать и гореть
    if (currentLevel == OperatingLevel::LEVEL_BLOCKED) {
      //setLedAndBuzzer(false, true, true);
      indicationModule.GREEN_OFF();
      indicationModule.BEEP_OFF();
      indicationModule.RED_ON(0);
    } else {
      //setLedAndBuzzer(false, false, false);
      indicationModule.ALL_OFF();
    }
  }
}

void MainCounterModule::startSession() {

  Serial.println("MainCounterModule::startSession");

  sessionActive = true;
  delayInProgress = true;
  delayStartMillis = millis();

  // Подаем индикацию старта: зелёный + писк
  //setLedAndBuzzer(true, false, true);
  indicationModule.GREEN_ON(1000);
  indicationModule.BEEP_ON(1000);
}

void MainCounterModule::stopSession() {

  Serial.println("MainCounterModule::stopSession");

  sessionActive = false;
  delayInProgress = false;

  // Вычисляем итоговое время за сессию
  uint32_t sessionDuration = (millis() - sessionStartMillis) / 1000;
  memoryCurrentOperatingTime += sessionDuration;
  eeprom.saveCurrentOperatingTime(memoryCurrentOperatingTime);

  // Переход в BLOCKED, если мы были на замене фильтра
  if (currentLevel == OperatingLevel::LEVEL_REPLACEMENT) {
    currentLevel = OperatingLevel::LEVEL_BLOCKED;
  }

  eeprom.saveOperatingLevel(currentLevel);

  // Свет и звук гасим
  //setLedAndBuzzer(false, false, false);
  indicationModule.ALL_OFF();
}

void MainCounterModule::updateSession() {
  if (delayInProgress) {
    if (millis() - delayStartMillis >= 1500) {
      delayInProgress = false;
      sessionStartMillis = millis();
      lastEepromSaveMillis = sessionStartMillis;

      indicationModule.ALL_OFF();
      // indicationModule.GREEN_ON(0);

    } else {
      return;  // ждём завершения задержки
    }
  }

  // Сколько прошло времени за текущую сессию
  uint32_t elapsed = (millis() - sessionStartMillis) / 1000;
  uint32_t currentTime = memoryCurrentOperatingTime + elapsed;

  // Сохраняем в EEPROM каждые 30 секунд
  if (millis() - lastEepromSaveMillis >= 30000) {
    eeprom.saveCurrentOperatingTime(currentTime);
    lastEepromSaveMillis = millis();
  }

  // Проверка уровней
  if (currentLevel == OperatingLevel::LEVEL_OPERATING && currentTime >= eeprom.getMaxOperatingTime()) {
    currentLevel = OperatingLevel::LEVEL_REPLACEMENT;
    eeprom.saveOperatingLevel(currentLevel);
  }
  if (currentLevel == OperatingLevel::LEVEL_REPLACEMENT && currentTime >= (eeprom.getMaxOperatingTime() + eeprom.getMaxReplacementTime())) {
    currentLevel = OperatingLevel::LEVEL_BLOCKED;
    eeprom.saveOperatingLevel(currentLevel);
  }

  // Управление индикацией по текущему уровню
  switch (currentLevel) {
    case OperatingLevel::LEVEL_OPERATING:
      //setLedAndBuzzer(true, false, false);
      indicationModule.BEEP_OFF();
      indicationModule.RED_OFF();
      indicationModule.GREEN_ON(0);
      break;
    case OperatingLevel::LEVEL_REPLACEMENT:
      //blinkRedAndBeep();
      indicationModule.GREEN_OFF();
      indicationModule.BEEP_BLINK_FOREVER(500);
      indicationModule.RED_BLINK_FOREVER(500);
      break;
    case OperatingLevel::LEVEL_BLOCKED:
      //setLedAndBuzzer(false, true, true);
      indicationModule.GREEN_OFF();
      indicationModule.BEEP_ON(0);
      indicationModule.RED_ON(0);
      break;
    default:
      //setLedAndBuzzer(false, false, false);
      indicationModule.ALL_OFF();
      break;
  }
}
/*
void MainCounterModule::setLedAndBuzzer(bool greenOn, bool redOn, bool buzzerOn) {
  digitalWrite(pinGreen, greenOn ? HIGH : LOW);
  digitalWrite(pinRed, redOn ? HIGH : LOW);
  digitalWrite(pinBuzzer, buzzerOn ? HIGH : LOW);
}

void MainCounterModule::blinkRedAndBeep() {
  bool state = (millis() / 500) % 2 == 0;
  digitalWrite(pinRed, state ? HIGH : LOW);
  digitalWrite(pinBuzzer, state ? HIGH : LOW);
}
*/