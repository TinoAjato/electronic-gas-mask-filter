#include "IndicationModule.h"

IndicationModule::IndicationModule(uint8_t gLed, uint8_t rLed, uint8_t buzzer)
  : greenPin(gLed), redPin(rLed), buzzerPin(buzzer) {}

void IndicationModule::begin() {
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  // Выключаем все индикаторы по умолчанию
  digitalWrite(redPin, LOW);
  digitalWrite(greenPin, LOW);
  buzzerOff();
}

/**
 * Включение пищалки
 */
void IndicationModule::buzzerOn() {
  tone(buzzerPin, BUZZER_FREQUENCY);  // Запуск звука с фиксированной частотой
}
/**
 * Выключение пищалки
 */
void IndicationModule::buzzerOff() {
  noTone(buzzerPin);  // Остановка звука
}

/* ******************************* */

/**
 * Однократное включение зелёного светодиода на durationMs миллисекунд
 * Если durationMs == 0, включение бесконечное
 * Останавливает мигание зелёного светодиода, если оно было
 */
void IndicationModule::GREEN_ON(unsigned int durationMs) {

  if (greenState.active && greenState.duration == durationMs) {
    return;
  }

  greenState = OneShotState(true, millis(), durationMs);
  greenBlink.active = false;  // Отключаем мигание
  digitalWrite(greenPin, HIGH);
}
/**
 * Включение зелёного светодиода и остановка мигания
 */
void IndicationModule::GREEN_OFF() {
  greenState.active = false;
  greenBlink.active = false;
  digitalWrite(greenPin, LOW);
}
/**
 * Мигает зелёным светодиодом в течение durationMs миллисекунд с периодом periodMs
 * Если durationMs == 0, мигание бесконечное
 */
void IndicationModule::GREEN_BLINK(unsigned int durationMs, unsigned int periodMs) {
  if (durationMs <= 0) {
    GREEN_BLINK_FOREVER(periodMs);
    return;
  }

  greenBlink = BlinkState(true, false, millis(), millis(), millis() + durationMs, periodMs);
  greenState.active = false;
  digitalWrite(greenPin, LOW);
}
/**
 * Запуск бесконечного мигания зелёного светодиода с периодом periodMs
 */
void IndicationModule::GREEN_BLINK_FOREVER(unsigned int periodMs) {

  if (greenBlink.active && greenBlink.period == periodMs) {
    return;
  }

  greenBlink = BlinkState(true, false, millis(), millis(), 0, periodMs);
  greenState.active = false;
  digitalWrite(greenPin, LOW);
}

/* ******************************* */

/**
 * Однократное включение красного светодиода на durationMs миллисекунд
 * Если durationMs == 0, включение бесконечное
 * Останавливает мигание красного светодиода, если оно было
 */
void IndicationModule::RED_ON(unsigned int durationMs) {

  if (redState.active && redState.duration == durationMs) {
    return;
  }

  redState = OneShotState(true, millis(), durationMs);
  redBlink.active = false;  // Отключаем мигание
  digitalWrite(redPin, HIGH);
}
/**
 * Включение красного светодиода и остановка мигания
 */
void IndicationModule::RED_OFF() {
  redState.active = false;
  redBlink.active = false;
  digitalWrite(redPin, LOW);
}
/**
 * Мигает красным светодиодом в течение durationMs миллисекунд с периодом periodMs
 * Если durationMs == 0, мигание бесконечное
 */
void IndicationModule::RED_BLINK(unsigned int durationMs, unsigned int periodMs) {
  if (durationMs <= 0) {
    RED_BLINK_FOREVER(periodMs);
    return;
  }

  redBlink = BlinkState(true, false, millis(), millis(), millis() + durationMs, periodMs);
  redState.active = false;  // Выключаем однократное включение
  digitalWrite(redPin, LOW);
}
/**
 * Запуск бесконечного мигания красного светодиода с периодом periodMs
 */
void IndicationModule::RED_BLINK_FOREVER(unsigned int periodMs) {

  if (redBlink.active && redBlink.period == periodMs) {
    return;
  }

  redBlink = BlinkState(true, false, millis(), millis(), 0, periodMs);
  redState.active = false;
  digitalWrite(redPin, LOW);
}

/* ******************************* */

/**
 * Однократное включение пищалки на durationMs миллисекунд
 * Если durationMs == 0, включение бесконечное
 * Останавливает мигание пищалки, если оно было
 */
void IndicationModule::BEEP_ON(unsigned int durationMs) {

  if (buzzerState.active && buzzerState.duration == durationMs) {
    return;
  }

  buzzerState = OneShotState(true, millis(), durationMs);
  buzzerBlink.active = false;  // Отключаем мигание
  buzzerOn();
}
/**
 * Выключение пищалки и остановка мигания
 */
void IndicationModule::BEEP_OFF() {
  buzzerState.active = false;
  buzzerBlink.active = false;
  buzzerOff();
}
/**
 * Мигает пищалкой в течение durationMs миллисекунд с периодом periodMs
 * Если durationMs == 0, мигание бесконечное
 */
void IndicationModule::BEEP_BLINK(unsigned int durationMs, unsigned int periodMs) {
  if (durationMs <= 0) {
    BEEP_BLINK_FOREVER(periodMs);
    return;
  }

  buzzerBlink = BlinkState(true, false, millis(), millis(), millis() + durationMs, periodMs);
  buzzerState.active = false;
  buzzerOff();  // Гарантируем, что пищалка изначально выключена
}
/**
 * Запуск бесконечного мигания пищалкой с периодом periodMs и частотой звука buzzerFrequency
 */
void IndicationModule::BEEP_BLINK_FOREVER(unsigned int periodMs) {

  if (buzzerBlink.active && buzzerBlink.period == periodMs) {
    return;
  }

  buzzerBlink = BlinkState(true, false, millis(), millis(), 0, periodMs);
  buzzerState.active = false;
  buzzerOff();
}

/* ******************************* */

/**
 * Выключение всего
 */
void IndicationModule::ALL_OFF() {
  GREEN_OFF();
  RED_OFF();
  BEEP_OFF();
}

/* ******************************* */
/**
 * Метод update() должен вызываться в основном цикле loop()
 * Отвечает за обновление состояний однократных включений и миганий
 */
void IndicationModule::update() {
  unsigned long now = millis();

  // Обработка однократных включений
  if (redState.active && redState.duration > 0 && now - redState.startTime >= redState.duration) {
    digitalWrite(redPin, LOW);
    redState.active = false;
  }

  if (greenState.active && greenState.duration > 0 && now - greenState.startTime >= greenState.duration) {
    digitalWrite(greenPin, LOW);
    greenState.active = false;
  }

  if (buzzerState.active && buzzerState.duration > 0 && now - buzzerState.startTime >= buzzerState.duration) {
    buzzerOff();
    buzzerState.active = false;
  }

  // Обработка миганий для всех индикаторов
  struct {
    BlinkState &state;                    // Состояние мигания
    uint8_t pin;                          // Пин управления
    void (IndicationModule::*onFunc)();   // Функция включения (для пищалки tone)
    void (IndicationModule::*offFunc)();  // Функция выключения (для пищалки noTone)
  } blinkers[] = {
    { redBlink, redPin, nullptr, nullptr },
    { greenBlink, greenPin, nullptr, nullptr },
    { buzzerBlink, buzzerPin, &IndicationModule::buzzerOn, &IndicationModule::buzzerOff }
  };

  // Проходим по каждому мигающему элементу
  for (auto &blinker : blinkers) {
    BlinkState &state = blinker.state;

    // Если мигание не активно — пропускаем
    if (!state.active) continue;

    // Если установлен конец времени мигания, проверяем не истёк ли он
    if (state.endTime > 0 && now >= state.endTime) {
      state.active = false;
      state.isOn = false;

      // Выключаем индикатор корректным методом
      if (blinker.onFunc && blinker.offFunc) {
        (this->*blinker.offFunc)();
      } else {
        digitalWrite(blinker.pin, LOW);
      }
      continue;
    }

    // Проверяем, пора ли переключить состояние мигания (включить/выключить)
    if (now - state.lastToggleTime >= state.period) {
      state.isOn = !state.isOn;  // Переключаем состояние
      state.lastToggleTime = now;

      // Включаем или выключаем индикатор корректным методом
      if (blinker.onFunc && blinker.offFunc) {
        if (state.isOn)
          (this->*blinker.onFunc)();
        else
          (this->*blinker.offFunc)();
      } else {
        digitalWrite(blinker.pin, state.isOn ? HIGH : LOW);
      }
    }
  }
}