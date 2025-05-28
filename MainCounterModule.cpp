#include "src/MainCounterModule.h"

//******* Конструктор *******
MainCounterModule::MainCounterModule(Button& counterButton, MinimalEEPROM& eeprom, IndicationModule& indicationModule)
  : counterButton(counterButton),
    eeprom(eeprom),
    indicationModule(indicationModule),
    oneBeepFlag(false),
    memoryCurrentOperatingTime(0),
    sessionStartMillis(0),
    lastEepromSaveMillis(0) {
}

//******* Инициализация модуля *******
void MainCounterModule::begin() {
  // Гасим индикацию
  indicationModule.ALL_OFF();
  // Получаем стартовое записанное время из EEPROM
  memoryCurrentOperatingTime = eeprom.getCurrentOperatingTime();
}

//******* Главный цикл *******
void MainCounterModule::update() {
  // Всегда обновляем кнопку
  counterButton.update();

  // Кнопка удерживается
  if (counterButton.isPressed()) {
    // Если кнопка удерживается
    handlePressed();
  } else {
    // Если кнопка отжата
    handleReleased();
  }

  // Обновление индикации (светодиоды, пищалка)
  updateIndication();
}

//******* Обработка удержания кнопки *******
void MainCounterModule::handlePressed() {
  uint32_t now = millis();

  // Если сессия неактивна, задаем стартовые значения
  if (!sessionManager.isActive()) {
    // Запускаем сессию
    sessionManager.start(now);
    // Даем мелкий BEEP для обозначения старта
    indicationModule.BEEP_ON(START_BEEP_DURATION_MS);  // ДРУГАЯ ЧАСТОТА ИЛИ ЗАБИТЬ?

    // Получаем стартовое записанное время из EEPROM
    memoryCurrentOperatingTime = eeprom.getCurrentOperatingTime();

    //Ставим true, чтобы потом сработал 1 BEEP по отжатию counterButton
    oneBeepFlag = true;
  }

  // Обновляем время: получаем разницу со старта сессии до текущего момента + стартовое время из EEPROM
  uint32_t totalElapsed = memoryCurrentOperatingTime + sessionManager.elapsed(now);

  // Сохраняем каждые 30 секунд на всякий случай (хотя, если подумать, то как бы и нафиг надо)
  if (now - lastEepromSaveMillis >= EEPROM_SAVE_INTERVAL_MS) {
    eeprom.saveCurrentOperatingTime(totalElapsed);
    lastEepromSaveMillis = now;
  }

  // Обновляем уровень работы устройства (основная работа / замена фильтра / заблокировано до сброса)
  // передаем разницу времени и данные из EEPROM: максимальное время работы и максимальное время до замены фильтра
  updateOperatingLevel(
    totalElapsed,
    eeprom.getMaxOperatingTime(),
    eeprom.getMaxReplacementTime());
}

//******* Обработка отпускания кнопки *******
void MainCounterModule::handleReleased() {
  // Если сессия активна, записываем в EEPROM нужные данные
  if (sessionManager.isActive()) {
    uint32_t now = millis();
    // Останавливаем сессию и получем время работы сессии
    uint32_t sessionTime = sessionManager.stop(now);
    // Получаем время работы сессии с учетом стартового времени из EEPROM
    memoryCurrentOperatingTime += sessionTime;

    // Если уровень был на замене фильтра — переходим в режим "заблокировано до сброса"
    if (eeprom.getOperatingLevel() == OperatingLevel::LEVEL_REPLACEMENT) {
      // Пишем в память, что режим "заблокировано до сброса"
      eeprom.saveOperatingLevel(OperatingLevel::LEVEL_BLOCKED);
      // Пишем в память время (время основной работы + время замены фильтра), чтобы фильтр был ГАРАНТИРОВАНО в режиме "заблокировано до сброса"
      eeprom.saveCurrentOperatingTime(eeprom.getMaxOperatingTime() + eeprom.getMaxReplacementTime());
    } else {
      // Иначе, если уровень НЕ "заблокировано до сброса", то пишем просто время работы сессии с учетом стартового времени из EEPROM
      eeprom.saveCurrentOperatingTime(memoryCurrentOperatingTime);
    }
  }
}

//******* Управление индикацией *******
void MainCounterModule::updateIndication() {
  // Получаем уровень работы устройства
  switch (eeprom.getOperatingLevel()) {
    case OperatingLevel::LEVEL_OPERATING:
      // Основная работа - красный индикатор выключен (причем BEEP мы не трогаем, тут аккуратней нужно быть, кстати), зеленый горит постоянно
      indicationModule.RED_OFF();
      indicationModule.GREEN_ON(0);
      break;
    case OperatingLevel::LEVEL_REPLACEMENT:
      // Замена фильтра - зеленый индикатор выключен, BEEP и красный светодиод мигает
      indicationModule.GREEN_OFF();
      indicationModule.RED_BLINK_FOREVER(500);
      indicationModule.BEEP_BLINK_FOREVER(500);
      break;
    case OperatingLevel::LEVEL_BLOCKED:
      // Заблокировано до сброса - зеленый индикатор выключен, красный светодиод горит постоянно, BEEP работает 3 секунды
      indicationModule.GREEN_OFF();
      indicationModule.RED_ON(0);
      // Гарантия, что BEEP будет запущен 1 раз, при отжатии кнопки counterButton
      if (oneBeepFlag) {
        oneBeepFlag = false;
        indicationModule.BEEP_ON(3000);  //ДРУГАЯ ЧАСТОТА ИЛИ ЗАБИТЬ?
      }
      break;
    default:
      // Если в EEPROM записано черте что, то гасим всё
      indicationModule.ALL_OFF();
      break;
  }
}

//******* Управление уровнями *******
void MainCounterModule::updateOperatingLevel(uint32_t totalTime, uint32_t maxOperating, uint32_t maxReplacement) {
  if (totalTime < maxOperating) {
    // Если время работы устройства (с учетом стартового значения) меньше времени основной работы, ТО уровень: Основная работа
    eeprom.saveOperatingLevel(OperatingLevel::LEVEL_OPERATING);
  } else if (totalTime < maxOperating + maxReplacement) {
    // Если время работы устройства (с учетом стартового значения) меньше времени основной работы + время замены фильтра, ТО уровень: Время замены фильтра
    eeprom.saveOperatingLevel(OperatingLevel::LEVEL_REPLACEMENT);
  } else {
    // Устройство заблокировано до сброса
    eeprom.saveOperatingLevel(OperatingLevel::LEVEL_BLOCKED);
  }
}

//******* SessionManager *******
void MainCounterModule::SessionManager::start(uint32_t now) {
  sessionStart = now;
  active = true;
}
uint32_t MainCounterModule::SessionManager::stop(uint32_t now) {
  active = false;
  return (now - sessionStart) / 1000;
}
uint32_t MainCounterModule::SessionManager::elapsed(uint32_t now) const {
  return (now - sessionStart) / 1000;
}
bool MainCounterModule::SessionManager::isActive() const {
  return active;
}
