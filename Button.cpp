#include "Button.h"

Button::Button(uint8_t pin, HardwareSerial& serial, uint16_t holdTime, uint16_t debounce)
  : _pin(pin), _holdTime(holdTime), _debounce(debounce),serial(serial) {

  pinMode(_pin, INPUT_PULLUP);     // Настраиваем пин как вход с подтяжкой к питанию
  _rawState = !digitalRead(_pin);  // Считываем начальное состояние (инвертировано)
  _stableState = _rawState;        // Начальное стабильное состояние
  _lastStableState = _rawState;    // Начальное предыдущее состояние
  _lastDebounceTime = millis();    // Фиксируем время старта
}

void Button::update() {
  
  bool reading = !digitalRead(_pin);  // Считываем текущее состояние (инвертировано)
  unsigned long now = millis();       // Текущее время

  // Если состояние изменилось — сбрасываем таймер дебаунса
  if (reading != _rawState) {
    _rawState = reading;
    _lastDebounceTime = now;
  }

  // Если прошло достаточно времени после изменения — принимаем новое состояние
  if ((now - _lastDebounceTime) > _debounce) {
    if (_rawState != _stableState) {
      _lastStableState = _stableState;  // Запоминаем старое состояние
      _stableState = _rawState;         // Применяем новое отфильтрованное состояние

      if (_stableState) {
        // Если кнопка нажата (переход в нажатое состояние)
        _pressStartTime = now;  // Фиксируем момент начала нажатия
        _pressEdge = true;      // Устанавливаем флаг нажатия
      } else {
        // Если кнопка отпущена (переход в не нажатое состояние)
        _releaseEdge = true;  // Устанавливаем флаг отпускания

        // Если удержание не было длинным — считаем это кликом
        if (!_longPressHandled && (now - _pressStartTime < _holdTime)) {
          _clickFlag = true;
        }

        _longPressHandled = false;  // Сброс обработки долгого нажатия
      }
    }
  }

  // Если кнопка удерживается достаточно долго и это ещё не обработано
  if (_stableState && !_longPressHandled && (now - _pressStartTime >= _holdTime)) {
    _longPressFlag = true;     // Устанавливаем флаг долгого нажатия
    _longPressHandled = true;  // Не даём флагу сработать повторно
  }
}

// Текущее стабильное состояние (удерживается)
bool Button::isPressed() const {
  return _stableState;
}

// Срабатывает один раз при нажатии
bool Button::wasPressed() {
  bool tmp = _pressEdge;
  _pressEdge = false;
  return tmp;
}

// Срабатывает один раз при отпускании
bool Button::wasReleased() {
  bool tmp = _releaseEdge;
  _releaseEdge = false;
  return tmp;
}

// Срабатывает один раз при быстром нажатии и отпускании
bool Button::isClick() {
  bool tmp = _clickFlag;
  _clickFlag = false;
  return tmp;
}

// Срабатывает один раз при долгом удержании кнопки
bool Button::isLongPress() {
  bool tmp = _longPressFlag;
  _longPressFlag = false;
  return tmp;
}