#ifndef BUTTON_H
#define BUTTON_H

#include <Arduino.h>

class Button {
public:
  // Конструктор: принимает пин, длительность долгого нажатия (в мс), и задержку дребезга
  Button(uint8_t pin, uint16_t holdTime = 1500, uint16_t debounce = 50);

  // Обновление состояния кнопки, нужно вызывать каждый цикл loop()
  void update();

  // Возвращает текущее (фильтрованное) состояние: true — нажата
  bool isPressed() const;

  // Срабатывает один раз в момент нажатия (фронт сигнала)
  bool wasPressed();

  // Срабатывает один раз в момент отпускания (спад сигнала)
  bool wasReleased();

  // Срабатывает один раз после быстрого нажатия и отпускания (клик)
  bool isClick();

  // Срабатывает один раз при удержании кнопки дольше holdTime
  bool isLongPress();

private:
  uint8_t _pin;        // Пин, к которому подключена кнопка
  uint16_t _holdTime;  // Время, после которого нажатие считается долгим
  uint16_t _debounce;  // Задержка фильтрации дребезга

  // Состояния
  bool _rawState = false;         // Сырое (нефильтрованное пиво :) ) состояние
  bool _stableState = false;      // Стабильное (фильтрованное пиво :( ) состояние
  bool _lastStableState = false;  // Предыдущее стабильное состояние

  // Временные метки
  unsigned long _lastDebounceTime = 0;  // Время последнего изменения состояния
  unsigned long _pressStartTime = 0;    // Время начала нажатия

  // Флаги-события (одноразовые сигналы)
  bool _pressEdge = false;         // Фронт: кнопка только что нажата
  bool _releaseEdge = false;       // Спад: кнопка только что отпущена
  bool _clickFlag = false;         // Флаг клика
  bool _longPressFlag = false;     // Флаг долгого нажатия
  bool _longPressHandled = false;  // Указывает, было ли уже обработано долгое нажатие
};

#endif