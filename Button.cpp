#include "src/Button.h"

Button::Button(uint8_t pin, uint16_t holdTime, uint16_t debounce)
  : pin(pin), holdTime(holdTime), debounce(debounce) {

  pinMode(pin, INPUT_PULLUP);     // Настраиваем пин как вход с подтяжкой к питанию
  rawState = !digitalRead(pin);  // Считываем начальное состояние (инвертировано)
  stableState = rawState;        // Начальное стабильное состояние
  lastStableState = rawState;    // Начальное предыдущее состояние
  lastDebounceTime = millis();    // Фиксируем время старта
}

void Button::update() {

  bool reading = !digitalRead(pin);  // Считываем текущее состояние (инвертировано)
  unsigned long now = millis();       // Текущее время

  // Если состояние изменилось — сбрасываем таймер дебаунса
  if (reading != rawState) {
    rawState = reading;
    lastDebounceTime = now;
  }

  // Если прошло достаточно времени после изменения — принимаем новое состояние
  if ((now - lastDebounceTime) > debounce) {
    if (rawState != stableState) {
      lastStableState = stableState;  // Запоминаем старое состояние
      stableState = rawState;         // Применяем новое отфильтрованное состояние

      if (stableState) {
        // Если кнопка нажата (переход в нажатое состояние)
        pressStartTime = now;  // Фиксируем момент начала нажатия
        pressEdge = true;      // Устанавливаем флаг нажатия
      } else {
        // Если кнопка отпущена (переход в не нажатое состояние)
        releaseEdge = true;  // Устанавливаем флаг отпускания

        // Если удержание не было длинным — считаем это кликом
        if (!longPressHandled && (now - pressStartTime < holdTime)) {
          clickFlag = true;
        }

        longPressHandled = false;  // Сброс обработки долгого нажатия
      }
    }
  }

  // Если кнопка удерживается достаточно долго и это ещё не обработано
  if (stableState && !longPressHandled && (now - pressStartTime >= holdTime)) {
    longPressFlag = true;     // Устанавливаем флаг долгого нажатия
    longPressHandled = true;  // Не даём флагу сработать повторно
  }
}

// Текущее стабильное состояние (удерживается)
bool Button::isPressed() const {
  return stableState;
}

// Срабатывает один раз при нажатии
bool Button::wasPressed() {
  bool tmp = pressEdge;
  pressEdge = false;
  return tmp;
}

// Срабатывает один раз при отпускании
bool Button::wasReleased() {
  bool tmp = releaseEdge;
  releaseEdge = false;
  return tmp;
}

// Срабатывает один раз при быстром нажатии и отпускании
bool Button::isClick() {
  bool tmp = clickFlag;
  clickFlag = false;
  return tmp;
}

// Срабатывает один раз при долгом удержании кнопки
bool Button::isLongPress() {
  bool tmp = longPressFlag;
  longPressFlag = false;
  return tmp;
}