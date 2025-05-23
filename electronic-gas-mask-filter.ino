#include "Button.h"
#include "MinimalEEPROM.h"
#include "ConfigurationMode.h"
#include "MainOperation.h"
#include "IndicationModule.h"

//******* Работа с памятью EEPROM *******
MinimalEEPROM eeprom;

//******* МОДУЛЬ КОНФИГУРАЦИИ *******
// Кнопка - конфигурация, сброс
Button configButton(3, 1500, 50);
// Массив пинов кнопок DIP-переключателя
uint8_t buttonPinsDIP[] = { 7, 6, 5, 4 };
// Обьявляем модуль конфигурации с пинами для зелёного, красного, зуммера и массив для DIP-переключателя
ConfigurationMode configurationModule(configButton, eeprom, 11, 12, 13, buttonPinsDIP);

//******* ГЛАВНЫЙ МОДУЛЬ *******
// Кнопка - старт счетчика
Button mainButton(2);
// Объявляем модуль счетчика
MainOperation mainModule(mainButton, eeprom, 11, 12, 13);

IndicationModule indication(11, 12, 13);
int mode = -1;

void setup() {
  Serial.begin(9600);
  while (!Serial)
    ;  // Ожидание USB-соединения

  indication.begin();

  /*
  // Инициализация модуля конфигурации
  configurationModule.begin();
  // Инициализация модуля счетчика
  mainModule.begin();




  Serial.print("Максимальное время работы в секундах: ");
  uint32_t val = eeprom.getMaxOperatingTime();
  Serial.print(val);
  Serial.print("; В минутах: ");
  Serial.println((uint32_t)val / 60);

  Serial.print("Максимальное время замены фильтра в секундах: ");
  val = eeprom.getMaxReplacementTime();
  Serial.print(val);
  Serial.print("; В минутах: ");
  Serial.println((uint32_t)val / 60);

  Serial.print("Текущее накопленное время работы в секундах: ");
  val = eeprom.getCurrentOperatingTime();
  Serial.print(val);
  Serial.print("; В минутах: ");
  Serial.println((uint32_t)val / 60);*/
}

void loop() {
  // Обновление состояния кнопки и индикаторов
  mainButton.update();
  indication.update();

  // Проверка на одиночное нажатие кнопки
  if (mainButton.isClick()) {
    mode++;
    if (mode > 11) mode = 0;

    // Отключаем все индикаторы перед запуском нового режима
    indication.GREEN_OFF();
    indication.RED_OFF();
    indication.BEEP_OFF();

    // Выбор действия по текущему режиму
    switch (mode) {
      case 0:
        // Зелёный светодиод горит 2 секунды
        indication.GREEN_ON(2000);
        Serial.println("Зелёный светодиод горит 2 секунды");
        break;
      case 1:
        // Красный светодиод горит 2 секунды
        indication.RED_ON(2000);
        Serial.println("Красный светодиод горит 2 секунды");
        break;
      case 2:
        // Пищалка пищит 2 секунды без мигания
        indication.BEEP_ON(2000);
        Serial.println("Пищалка пищит 2 секунды без мигания");
        break;
      case 3:
        // Зелёный мигает 3 секунды с интервалом 500 мс
        indication.GREEN_BLINK(3000, 500);
        Serial.println("Зелёный мигает 3 секунды с интервалом 500 мс");
        break;
      case 4:
        // Красный мигает 3 секунды с интервалом 250 мс
        indication.RED_BLINK(3000, 250);
        Serial.println("Красный мигает 3 секунды с интервалом 250 мс");
        break;
      case 5:
        // Пищалка мигает 3 секунды с интервалом 100 мс
        indication.BEEP_BLINK(3000, 100);
        Serial.println("Пищалка мигает 3 секунды с интервалом 100 мс");
        break;
      case 6:
        // Бесконечное мигание зелёным с интервалом 300 мс
        indication.GREEN_BLINK_FOREVER(300);
        Serial.println("Бесконечное мигание зелёным с интервалом 300 мс");
        break;
      case 7:
        // Бесконечное мигание красным с интервалом 500 мс
        indication.RED_BLINK_FOREVER(500);
        Serial.println("Бесконечное мигание красным с интервалом 500 мс");
        break;
      case 8:
        // Бесконечный писк с интервалом 150 мс
        indication.BEEP_BLINK_FOREVER(150);
        Serial.println("Бесконечный писк с интервалом 150 мс");
        break;
      case 9:
        // Бесконечное мигание зелёным с интервалом 300 мс
        indication.GREEN_BLINK(0, 300);
        Serial.println("Бесконечное мигание зелёным с интервалом 300 мс");
        break;
      case 10:
        // Бесконечное мигание красным с интервалом 500 мс
        indication.RED_BLINK(0, 500);
        Serial.println("Бесконечное мигание красным с интервалом 500 мс");
        break;
      case 11:
        // Бесконечный писк с интервалом 150 мс
        indication.BEEP_BLINK(0, 150);
        Serial.println("Бесконечный писк с интервалом 150 мс");
        break;
    }
  }

  /*// Обрабатываем конфигурацию
  configurationModule.update();
  if (configurationModule.isActive()) {
    return;  // корректно выходим в окно в режиме конфигурации
  }

  // Обрабатываем счетчик
  mainModule.update();
*/
  /*
  mainButton.update();
  if (mainButton.isClick()) {
    Serial.print("Максимальное время работы в секундах: ");
    uint32_t val = eeprom.getMaxOperatingTime();
    Serial.print(val);
    Serial.print("; В минутах: ");
    Serial.println((uint32_t)val / 60);

    Serial.print("Максимальное время замены фильтра в секундах: ");
    val = eeprom.getMaxReplacementTime();
    Serial.print(val);
    Serial.print("; В минутах: ");
    Serial.println((uint32_t)val / 60);

    Serial.print("Текущее накопленное время работы в секундах: ");
    val = eeprom.getCurrentOperatingTime();
    Serial.print(val);
    Serial.print("; В минутах: ");
    Serial.println((uint32_t)val / 60);
  }
  */
}