void isrButtonWakeUp(void) {
  detachInterrupt(digitalPinToInterrupt(BUTTON_PIN));
  if (millis() - _v_lastInterruptTime > (unsigned long)DEBOUNCE_DELAY*MILLISEC) {
    _v_buttonMillis = millis();
    attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), isrButtonWakeUpCheck, RISING);
  }
  _v_lastInterruptTime = millis();
}

void isrButtonWakeUpCheck() {
  detachInterrupt(digitalPinToInterrupt(BUTTON_PIN));
  if (millis() - _v_lastInterruptTime > (unsigned long)DEBOUNCE_DELAY*MILLISEC) {
    if (millis() - _v_buttonMillis < (unsigned long)LONG_PUSH*MILLISEC) enterSleep();
  }
  _v_lastInterruptTime = millis();
}

void isrButtonSleep(void) {
  isrButtonSleepCheck();
  return;
  if (millis() - _v_lastInterruptTime > (unsigned long)DEBOUNCE_DELAY*MILLISEC) {
    detachInterrupt(digitalPinToInterrupt(BUTTON_PIN));
    _v_buttonMillis = millis();
    attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), isrButtonSleepCheck, RISING);
  }
  _v_lastInterruptTime = millis();
}

void isrButtonSleepCheck(void) {
  if (millis() - _v_lastInterruptTime > (unsigned long)DEBOUNCE_DELAY*MILLISEC) {
    detachInterrupt(digitalPinToInterrupt(BUTTON_PIN));
    if (millis() - _v_buttonMillis < (unsigned long)LONG_PUSH*MILLISEC) {
      //setBuzzer(LONG_CHIRP);
      //delay(LONG_CHIRP*MILLISEC);
      enterSleep();
    } else {
      attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), isrButtonSleep, FALLING);
      _v_hold = !_v_hold;
      //setBuzzer(SHORT_CHIRP);
    }
  }
  _v_lastInterruptTime = millis();
}

void isrButton(void) {
  if (millis() - _v_lastInterruptTime > (unsigned long)DEBOUNCE_DELAY*MILLISEC) {
    _v_hold = !_v_hold;
  }
  _v_lastInterruptTime = millis();
}
