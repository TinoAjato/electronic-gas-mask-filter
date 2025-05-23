#include "HardwareSerial.h"
#include "ConfigurationMode.h"

ConfigurationMode::ConfigurationMode(Button &configBtn, MinimalEEPROM &eepromObj,
                                     uint8_t greenPin, uint8_t redPin, uint8_t buzzer, const uint8_t (&pins)[MAX_PINS_SIZE])
  : button(configBtn), eeprom(eepromObj), ledGreenPin(greenPin), ledRedPin(redPin), buzzerPin(buzzer),
    configModeActive(false), level(1), lastBlinkTime(0), ledState(false) {
  // Копируем массив пинов DIP-переключателя
  memcpy(this->pins, pins, sizeof(this->pins));
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
  //noTone(buzzerPin);

  // Настроим пины DIP-переключателя как входы с подтяжкой
  for (uint8_t i = 0; i < MAX_PINS_SIZE; i++) {
    pinMode(pins[i], INPUT_PULLUP);
  }

  // Обнулим состояние режима и таймера мигания
  configModeActive = false;
  level = 1;
  ledState = false;
  lastBlinkTime = millis();
}

void ConfigurationMode::update() {
  // Сначала обновляем состояние кнопки конфигурации
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
    } else if (button.isClick()) {
      // Одиночный клик вне режима конфигурации -> сброс CurrentOperatingTime
      Serial.println("saveCurrentOperatingTime(0)");
      eeprom.saveCurrentOperatingTime(0);
    }

    // Внешний режим, просто выходим
    return;
  }

  // --- Режим конфигурации активен ---
  // Проверяем выход из режима (долгое нажатие)
  if (button.isLongPress()) {
    // Сохраняем значение текущего уровня (если != 0) и выходим
    uint8_t dip = readDIP();
    saveCurrentLevelValue(dip);

    // Сбрасываем CurrentOperatingTime
    eeprom.saveCurrentOperatingTime(0);

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

  if (currentTime - lastBlinkTime >= BLINK_INTERVAL) {
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
      tone(buzzerPin, 900);
    }else {
      noTone(buzzerPin);
    }
    */
  }
}

// Включен ли режим конфигурирования
bool ConfigurationMode::isActive() const {
  return configModeActive;
}

// Используем INPUT_PULLUP: LOW (0) = переключатель замкнут (вкл), HIGH (1) = разомкнут (выкл)
uint8_t ConfigurationMode::readDIP() {
  uint8_t value = 0;
  // Читаем пины чтобы младший бит соответствовал последнему переключателю
  for (int i = 0; i < MAX_PINS_SIZE; i++) {
    if (digitalRead(pins[i]) == LOW) {
      value |= (1 << (MAX_PINS_SIZE - 1 - i));  // A2 → бит 3, A5 → бит 0
    }
  }
  // Полученное число от 0 до 15. Если 0, считаем, что пользователь не задал значение
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
    uint32_t seconds = (uint32_t)dipValue * 5;
    eeprom.saveMaxOperatingTime(seconds);
  } else {
    // Level 2: время замены (каждое значение *5 сек)
    uint32_t seconds = (uint32_t)dipValue * 5;
    eeprom.saveMaxReplacementTime(seconds);
  }
}