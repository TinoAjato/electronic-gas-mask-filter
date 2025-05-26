#ifndef INDICATIONMODULE_H
#define INDICATIONMODULE_H

#include <Arduino.h>

/**
 * Класс IndicationModule управляет тремя индикаторами:
 * - зелёный светодиод
 * - красный светодиод
 * - пьезопищалка
 * 
 * Позволяет включать индикаторы на фиксированное время (one-shot),
 * а также запускать мигания с заданным периодом и длительностью
 * 
 * Метод update() должен вызываться в основном цикле loop() для обновления состояния
 */
class IndicationModule {
public:
  /**
     * @param gLed - пин зелёного светодиода
     * @param rLed - пин красного светодиода
     * @param buzzer - пин пьезопищалки
     */
  IndicationModule(uint8_t gLed, uint8_t rLed, uint8_t buzzer);

  /**
     * Инициализация пинов и начальное выключение индикаторов
     * Вызывать в setup()
     */
  void begin();

  void GREEN_ON(unsigned int durationMs);
  void GREEN_OFF();
  void GREEN_BLINK(unsigned int durationMs, unsigned int periodMs);
  void GREEN_BLINK_FOREVER(unsigned int periodMs);

  void RED_ON(unsigned int durationMs);
  void RED_OFF();
  void RED_BLINK(unsigned int durationMs, unsigned int periodMs);
  void RED_BLINK_FOREVER(unsigned int periodMs);

  void BEEP_ON(unsigned int durationMs);
  void BEEP_OFF();
  void BEEP_BLINK(unsigned int durationMs, unsigned int periodMs);
  void BEEP_BLINK_FOREVER(unsigned int periodMs);

  void ALL_OFF();

  /**
     * Метод update() должен вызываться в loop()
     */
  void update();

private:
  uint8_t greenPin;                // Пин зелёного светодиода
  uint8_t redPin;                  // Пин красного светодиода
  uint8_t buzzerPin;               // Пин пьезопищалки

  /**
    *Частота звука пищалки в герцах
    */
  static constexpr uint16_t BUZZER_FREQUENCY = 950;

  /**
     * Структура для хранения состояния однократного включения (one-shot)
     */
  struct OneShotState {
    bool active;              // Активно ли состояние (включен индикатор)
    unsigned long startTime;  // Время начала включения (ms)
    unsigned int duration;    // Длительность включения (ms)

    OneShotState(bool a = false, unsigned long s = 0, unsigned int d = 0)
      : active(a), startTime(s), duration(d) {}
  };

  /**
     * Структура для хранения состояния мигания (blink)
     */
  struct BlinkState {
    bool active;                   // Активно ли мигание
    bool isOn;                     // Текущее состояние индикатора (включен/выключен)
    unsigned long startTime;       // Время запуска мигания
    unsigned long lastToggleTime;  // Время последнего переключения состояния
    unsigned long endTime;         // Время окончания мигания (ms, 0 - бесконечное)
    unsigned int period;           // Период мигания (ms)

    BlinkState(bool a = false, bool on = false, unsigned long s = 0, unsigned long l = 0, unsigned long e = 0, unsigned int p = 0)
      : active(a), isOn(on), startTime(s), lastToggleTime(l), endTime(e), period(p) {}
  };

  /**
    *Текущее состояние однократного включения зеленого/красного/пьезопищалки
    */
  OneShotState greenState, redState, buzzerState;

  /**
    *Текущее состояние мигания зеленого/красного/пьезопищалки
    */
  BlinkState greenBlink, redBlink, buzzerBlink;

  // Управление пищалкой через tone/noTone
  void buzzerOn();
  void buzzerOff();
};

#endif  // INDICATIONMODULE_H