#include "ConfigurationMode.h"

ConfigurationMode::ConfigurationMode(Button &configBtn, MinimalEEPROM &eepromObj,
                                     uint8_t greenPin, uint8_t redPin, uint8_t buzzer)
  : button(configBtn), eeprom(eepromObj),
    ledGreenPin(greenPin), ledRedPin(redPin), buzzerPin(buzzer),
    configModeActive(false), level(1),
    lastBlinkTime(0), ledState(false) {
}

void ConfigurationMode::begin() {
  // Настроим пины как выходы
  pinMode(ledGreenPin, OUTPUT);
  pinMode(ledRedPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  // Установим их в LOW (выключено)
  digitalWrite(ledGreenPin, LOW);
  digitalWrite(ledRedPin, LOW);
  digitalWrite(buzzerPin, LOW);

  // Настроим пины DIP как входы с подтяжкой
  pinMode(A2, INPUT_PULLUP);
  pinMode(A3, INPUT_PULLUP);
  pinMode(A4, INPUT_PULLUP);
  pinMode(A5, INPUT_PULLUP);

  // Обнулим состояние режима и таймера мигания
  configModeActive = false;
  level = 1;
  ledState = false;
  lastBlinkTime = millis();
}

void ConfigurationMode::update() {
  // Сначала обновляем состояние кнопки
  button.update();

  if (!configModeActive) {
    // --- Внешний режим (не в конфиге) ---
    if (button.isLongPress()) {
      // Вход в режим конфигурации
      configModeActive = true;
      level = 1;
      // Сбросить мигание при входе
      ledState = false;
      lastBlinkTime = millis();
    }//для отладки заккоментил
     /*else if (button.isClick()) {
      // Одиночный клик вне режима конфигурации -> сброс CurrentOperatingTime
      eeprom.saveCurrentOperatingTime(0);
    }*/
    // Внешний режим, просто выходим
    return;
  }

  // --- Режим конфигурации активен ---
  // Проверяем выход из режима (долгое нажатие)
  if (button.isLongPress()) {
    // Сохраняем значение текущего уровня (если != 0) и выходим
    uint8_t dip = readDIP();
    saveCurrentLevelValue(dip);
    // Отключаем индикацию
    digitalWrite(ledGreenPin, LOW);
    digitalWrite(ledRedPin, LOW);
    // Звук поворотника
    digitalWrite(buzzerPin, LOW);
    //noTone(buzzerPin);
    configModeActive = false;
    return;
  }

  // Проверяем переход на другой уровень (клик)
  if (button.isClick()) {
    // Сохраняем текущее значение (если != 0)
    uint8_t dip = readDIP();
    saveCurrentLevelValue(dip);
    // Переключаем уровень 1<->2
    level = (level == 1) ? 2 : 1;
    // Сброс мигания для нового уровня
    ledState = false;
    lastBlinkTime = millis();
  }

  // Обновляем мигание светодиода и звук зуммера
  unsigned long currentTime = millis();
  const unsigned long blinkInterval = 500;  // период мигания (мс)
  if (currentTime - lastBlinkTime >= blinkInterval) {
    // Время переключить состояние светодиода
    lastBlinkTime = currentTime;
    ledState = !ledState;
    // Включаем соответствующий светодиод и пищалку
    if (level == 1) {
      // Уровень 1: зелёный LED
      digitalWrite(ledGreenPin, ledState ? HIGH : LOW);
      digitalWrite(ledRedPin, LOW);
    } else {
      // Уровень 2: красный LED
      digitalWrite(ledRedPin, ledState ? HIGH : LOW);
      digitalWrite(ledGreenPin, LOW);
    }
    // Зуммер в такт миганию LED
    // Звук поворотника
    digitalWrite(buzzerPin, ledState ? HIGH : LOW);
    /*
    if(ledState) {
      tone(buzzerPin, 500);
    }else {
      noTone(buzzerPin);
    }*/
  }
}

// Включен ли режим конфигурирования
bool ConfigurationMode::isActive() const {
  return configModeActive;
}

// Приватный метод: чтение 4-битного DIP (A2..A5)
// Используем INPUT_PULLUP: LOW (0) = переключатель замкнут (вкл), HIGH (1) = разомкнут (выкл).
uint8_t ConfigurationMode::readDIP() {
  uint8_t value = 0;
  // Бит 3 -> A2, бит 2 -> A3, бит 1 -> A4, бит 0 -> A5
  if (digitalRead(A5) == LOW) value |= 1;  // переключатель замкнут
  if (digitalRead(A4) == LOW) value |= 2;
  if (digitalRead(A3) == LOW) value |= 4;
  if (digitalRead(A2) == LOW) value |= 8;
  // Полученное число от 0 до 15. Если 0, считаем, что пользователь не задал значение.
  return value;
}

// Приватный метод: сохранение параметра в зависимости от уровня
void ConfigurationMode::saveCurrentLevelValue(uint8_t dipValue) {
  if (dipValue == 0) {
    // Если DIP=0 (все выключены), ничего не сохраняем
    return;
  }
  if (level == 1) {
    // Level 1: время работы (минуты -> секунды)
    uint32_t seconds = (uint32_t)dipValue * 60;
    eeprom.saveMaxOperatingTime(seconds);
  } else {
    // Level 2: время замены (каждое значение *5 сек)
    uint32_t seconds = (uint32_t)dipValue * 5;
    eeprom.saveMaxReplacementTime(seconds);
  }
}