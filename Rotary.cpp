#include "Rotary.h"

Rotary::Rotary(uint8_t pinLeft, uint8_t pinRight, uint8_t pinButton)
{
  _pinLeft = pinLeft;
  _pinRight = pinRight;
  _pinButton = pinButton;

  _longPressTime = BUTTON_LONG_PRESS;
  _currPos = 0;
  _rotateState = ROTATE_NONE;
}
//----------------------------------
void Rotary::init() {
  pinMode(_pinLeft, INPUT_PULLUP);
  pinMode(_pinRight, INPUT_PULLUP);
  pinMode(_pinButton, INPUT_PULLUP);

  _prevAB = digitalRead(_pinLeft) << 1;
  _prevAB |= digitalRead(_pinRight);
  _buttonEvent = digitalRead(_pinButton) << 1;
}
//----------------------------------
void Rotary::read() {
  if (micros() - _lastRead < 500) return; // no read in 1 mili second
  _lastRead = micros();

  // Read A-B Encoder
  _currAB = digitalRead(_pinLeft) << 1;
  _currAB |= digitalRead(_pinRight);
  if (_currAB != _prevAB) {
    switch (((_prevAB << 2) | _currAB)) {
      case CW_8:
        _currPos++; _rotateState = ROTATE_RIGHT;
        break;
      case CWC_4:
        _currPos--; _rotateState = ROTATE_LEFT;
        break;
    }
    _prevAB = _currAB;
  } else _rotateState = ROTATE_NONE;

  // Read Button
  _buttonEvent |= digitalRead(_pinButton);
  _buttonEvent &= 0b0011; // 2 bit

  if (_buttonEvent == BUTTON_STATE_DOWN) _lastPressed = millis();
  else if (_buttonEvent == BUTTON_STATE_RELEASED) _pressTime = millis() - _lastPressed;

  _buttonState = (_buttonEvent != BUTTON_STATE_RELEASED) ? _buttonEvent :
                 ((_state == ROTARY_STATE_HOLD_LEFT) || (_state == ROTARY_STATE_HOLD_RIGHT)) ? BUTTON_STATE_IDLE :
                 (_pressTime >= _longPressTime) ? BUTTON_STATE_LONG :
                 (_pressTime >= BUTTON_SHORT_PRESS) ? BUTTON_STATE_SHORT : BUTTON_STATE_IDLE; // chong nhieu phim bam

  _buttonEvent <<= 1;
}
//----------------------------------
uint8_t Rotary::getState() {
  if ((_buttonState == BUTTON_STATE_DOWN) | (_buttonState == BUTTON_STATE_HOLDING))
    _state = (_rotateState == ROTATE_LEFT) ? ROTARY_STATE_HOLD_LEFT :
             (_rotateState == ROTATE_RIGHT) ? ROTARY_STATE_HOLD_RIGHT : _state;
  else {
    _state = (_rotateState == ROTATE_LEFT) ? ROTARY_STATE_LEFT :
             (_rotateState == ROTATE_RIGHT) ? ROTARY_STATE_RIGHT :
             (_buttonState == BUTTON_STATE_SHORT) ?  ROTARY_STATE_SHORT :
             (_buttonState == BUTTON_STATE_LONG) ?  ROTARY_STATE_LONG : ROTARY_STATE_IDLE;
  }
  return _state;
}
//----------------------------------
uint8_t Rotary::getButtonState() {
  return _buttonState;
}
//----------------------------------
int32_t Rotary::getPos()
{
  return _currPos;
}
//----------------------------------
uint32_t Rotary::getPressTime() {
  return _pressTime;
}
//----------------------------------
void Rotary::setLongPress(uint32_t pressTime) {
  _longPressTime = pressTime;
}
//----------------------------------
void Rotary::setPos(int32_t pos) {
  _currPos = pos;
}
//----------------------------------
bool Rotary::isHolding(uint32_t holdTime) {
  _pressTime = 0;
  _lastRead = millis();
  while (!digitalRead(_pinButton)) {
    if (millis() - _lastRead >= holdTime) return true;
  }
  return false;
}
//------------------------------------
