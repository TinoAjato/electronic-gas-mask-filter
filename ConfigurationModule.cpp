#include "HardwareSerial.h"
#include "src/ConfigurationModule.h"

ConfigurationModule::ConfigurationModule(Button &configurationButton, MinimalEEPROM &eeprom, const uint8_t (&pins)[MAX_PINS_SIZE], IndicationModule &indicationModule)
  : configurationButton(configurationButton), eeprom(eeprom), configModeActive(false), level(1), indicationModule(indicationModule) {
  // Копируем массив пинов DIP-переключателя
  memcpy(this->pins, pins, sizeof(this->pins));
}

void ConfigurationModule::begin() {
  // Настроим пины DIP-переключателя как входы с подтяжкой
  for (uint8_t i = 0; i < MAX_PINS_SIZE; i++) {
    pinMode(pins[i], INPUT_PULLUP);
  }
  // Обнулим состояние режима и таймера мигания
  configModeActive = false;
  level = 1;
}

void ConfigurationModule::update() {
  // Сначала обновляем состояние кнопки конфигурации
  configurationButton.update();

  if (!configModeActive) {
    // --- Внешний режим (не в конфиге) ---
    if (configurationButton.isLongPress()) {
      // Вход в режим конфигурации
      configModeActive = true;
      level = 1;

      // Выключаем всю индикацию
      indicationModule.ALL_OFF();

    } else if (configurationButton.isClick()) {
      // Одиночный клик вне режима конфигурации -> сброс CurrentOperatingTime и OperatingLevel
      eeprom.saveCurrentOperatingTime(0);
      eeprom.saveOperatingLevel(OperatingLevel::LEVEL_OPERATING);
    }

    // Внешний режим, просто выходим
    return;
  }

  // --- Режим конфигурации активен ---
  // Проверяем выход из режима (долгое нажатие)
  if (configurationButton.isLongPress()) {
    // Сохраняем значение текущего уровня (если != 0) и выходим
    uint8_t dip = readDIP();
    saveCurrentLevelValue(dip);

    // Сбрасываем CurrentOperatingTime
    eeprom.saveCurrentOperatingTime(0);

    // Выключаем всю индикацию
    indicationModule.ALL_OFF();

    configModeActive = false;
    return;
  }

  // Проверяем переход на другой уровень (клик)
  if (configurationButton.isClick()) {
    // Сохраняем текущее значение (если != 0)
    uint8_t dip = readDIP();
    saveCurrentLevelValue(dip);
    // Переключаем уровень 1<->2
    level = (level == 1) ? 2 : 1;

    // Выключаем всю индикацию
    indicationModule.ALL_OFF();
  }

  // Включаем соответствующий светодиод и пищалку
  if (level == 1) {
    // Уровень 1: зелёный LED
    indicationModule.GREEN_BLINK_FOREVER(BLINK_INTERVAL);
    indicationModule.BEEP_BLINK_FOREVER(BLINK_INTERVAL);
  } else {
    // Уровень 2: красный LED
    indicationModule.RED_BLINK_FOREVER(BLINK_INTERVAL);
    indicationModule.BEEP_BLINK_FOREVER(BLINK_INTERVAL);
  }
}

// Включен ли режим конфигурирования
bool ConfigurationModule::isActive() const {
  return configModeActive;
}

// Используем INPUT_PULLUP: LOW (0) = переключатель замкнут (вкл), HIGH (1) = разомкнут (выкл)
uint8_t ConfigurationModule::readDIP() {
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
void ConfigurationModule::saveCurrentLevelValue(uint8_t dipValue) {
  if (dipValue == 0) {
    // Если DIP=0 (все выключены), ничего не сохраняем
    return;
  }
  if (level == 1) {
    // Level 1: время работы (минуты -> секунды) ------>>>>>>> ЗАМЕНИТЬ НА *60
    uint32_t seconds = (uint32_t)dipValue * 5;
    eeprom.saveMaxOperatingTime(seconds);
  } else {
    // Level 2: время замены (каждое значение *5 сек)
    uint32_t seconds = (uint32_t)dipValue * 5;
    eeprom.saveMaxReplacementTime(seconds);
  }
}