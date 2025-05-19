#include "Button.h"

Button btn(A1);  // Кнопка на пине A1

void setup() {
  Serial.begin(9600);
}

void loop() {

  btn.update();

  if (btn.isClick()) {
    Serial.println("жмякнули кнопку");
  }

  if (btn.isLongPress()) {
    Serial.println("долго уерживаем кнопку");
  }
}