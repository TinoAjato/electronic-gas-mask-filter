#include "MainOperation.h"
#include <Arduino.h>

MainOperation::MainOperation(Button& btn, MinimalEEPROM& eep, uint8_t greenLed, uint8_t redLed, uint8_t buzzer)
  : mainButton(btn), eeprom(eep), pinGreen(greenLed), pinRed(redLed), pinBuzzer(buzzer),
    currentLevel(LEVEL_NONE), sessionActive(false), memoryCurrentOperatingTime(0),
    delayInProgress(false), delayStartMillis(0), sessionStartMillis(0), lastEepromSaveMillis(0) {}

void MainOperation::begin() {
  pinMode(pinGreen, OUTPUT);
  pinMode(pinRed, OUTPUT);
  pinMode(pinBuzzer, OUTPUT);

  // Инициализируем уровень и восстановленное время
  memoryCurrentOperatingTime = eeprom.getCurrentOperatingTime();

  // Заглушка — читаем уровень из EEPROM, когда будет реализовано
  currentLevel = LEVEL_OPERATING;  // по умолчанию стартуем с первого уровня

  Serial.println("MainOperation::begin");
  Serial.print("MemoryCurrentOperatingTime: ");
  Serial.println(memoryCurrentOperatingTime);
  Serial.print("CurrentLevel: ");
  Serial.println(currentLevel);
}

void MainOperation::update() {
  mainButton.update();

  if (mainButton.isPressed()) {
    if (!sessionActive) {
      startSession();
    }
    updateSession();
  } else {
    if (sessionActive) {
      stopSession();
    }
    /*// Если заблокировано — продолжаем пищать и гореть
    if (currentLevel == LEVEL_BLOCKED) {
      setLedAndBuzzer(false, true, true);
    } else {
      setLedAndBuzzer(false, false, false);
    }*/
  }
}

void MainOperation::startSession() {

  Serial.println("MainOperation::startSession");

  sessionActive = true;
  delayInProgress = true;
  delayStartMillis = millis();

  // Подаем индикацию старта: зелёный + писк
  setLedAndBuzzer(true, false, true);
}

void MainOperation::stopSession() {

  Serial.println("MainOperation::stopSession");

  sessionActive = false;
  delayInProgress = false;

  // Вычисляем итоговое время за сессию
  uint32_t sessionDuration = (millis() - sessionStartMillis) / 1000;
  memoryCurrentOperatingTime += sessionDuration;
  eeprom.saveCurrentOperatingTime(memoryCurrentOperatingTime);

  // Переход в BLOCKED, если мы были на замене фильтра
  if (currentLevel == LEVEL_REPLACEMENT) {
    currentLevel = LEVEL_BLOCKED;
  }

  // Свет и звук гасим
  setLedAndBuzzer(false, false, false);
}

void MainOperation::updateSession() {
  if (delayInProgress) {
    if (millis() - delayStartMillis >= 1500) {
      delayInProgress = false;
      sessionStartMillis = millis();
      lastEepromSaveMillis = sessionStartMillis;
      setLedAndBuzzer(true, false, false);  // зелёный, без звука
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
  if (currentLevel == LEVEL_OPERATING && currentTime >= eeprom.getMaxOperatingTime()) {
    currentLevel = LEVEL_REPLACEMENT;
  }
  if (currentLevel == LEVEL_REPLACEMENT && currentTime >= (eeprom.getMaxOperatingTime() + eeprom.getMaxReplacementTime())) {
    currentLevel = LEVEL_BLOCKED;
  }

  // Управление индикацией по текущему уровню
  switch (currentLevel) {
    case LEVEL_OPERATING:
      setLedAndBuzzer(true, false, false);
      break;
    case LEVEL_REPLACEMENT:
      blinkRedAndBeep();
      break;
    case LEVEL_BLOCKED:
      setLedAndBuzzer(false, true, true);
      break;
    default:
      setLedAndBuzzer(false, false, false);
      break;
  }
}

void MainOperation::setLedAndBuzzer(bool greenOn, bool redOn, bool buzzerOn) {
  digitalWrite(pinGreen, greenOn ? HIGH : LOW);
  digitalWrite(pinRed, redOn ? HIGH : LOW);
  digitalWrite(pinBuzzer, buzzerOn ? HIGH : LOW);
}

void MainOperation::blinkRedAndBeep() {
  bool state = (millis() / 500) % 2 == 0;
  digitalWrite(pinRed, state ? HIGH : LOW);
  digitalWrite(pinBuzzer, state ? HIGH : LOW);
}