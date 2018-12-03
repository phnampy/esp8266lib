#ifndef ROTARY_H
#define ROTARY_H

#include <Arduino.h>

// Lib for Input_PullUp Rotary Encoder
// namhp.pyn@gmail.com

// Rotate right
#define CW_E 0b1110
#define CW_8 0b1000
#define CW_1 0b0001
#define CW_7 0b0111

// Rotate left
#define CWC_D 0b1101
#define CWC_4 0b0100
#define CWC_2 0b0010
#define CWC_B 0b1011

// Rotate State
#define ROTATE_NONE   0
#define ROTATE_LEFT   1
#define ROTATE_RIGHT  2

// Button State (PrevState + CurrState)
#define BUTTON_STATE_IDLE       0b0011 // 3
#define BUTTON_STATE_DOWN       0b0010 // 2
#define BUTTON_STATE_HOLDING    0b0000 // 0
#define BUTTON_STATE_RELEASED   0b0001 // 1
#define BUTTON_STATE_SHORT      0b0100 // 4
#define BUTTON_STATE_LONG       0b0101 // 5

#define BUTTON_SHORT_PRESS        50      // chong nhieu
#define BUTTON_LONG_PRESS         3000
#define BUTTON_READ_INTERVAL      10      // ms

// State when BUTTON is release
#define ROTARY_STATE_IDLE         0
#define ROTARY_STATE_LEFT         1
#define ROTARY_STATE_RIGHT        2
#define ROTARY_STATE_SHORT        3
#define ROTARY_STATE_LONG         4
// State when BUTTON is pressing
#define ROTARY_STATE_HOLDING      5
#define ROTARY_STATE_HOLD_LEFT    6
#define ROTARY_STATE_HOLD_RIGHT   7

class Rotary {
  public:
    Rotary(uint8_t pinLeft, uint8_t pinRight, uint8_t pinButton);

    void init();
    void read();
    int32_t getPos();
    uint8_t getButtonState();
    uint8_t getState();
    uint32_t getPressTime();
    void setPos(int32_t pos);
    void setLongPress(uint32_t pressTime);
    bool isHolding(uint32_t holdTime = BUTTON_LONG_PRESS);

  private:

    uint8_t _pinLeft;
    uint8_t _pinRight;
    uint8_t _pinButton;

    int32_t _currPos;
    uint8_t _buttonState;
    uint8_t _rotateState;
    uint8_t _state;

    uint32_t _lastRead;
    uint32_t _lastPressed;
    uint32_t _pressTime;
    uint32_t _longPressTime;

    byte _prevAB; // 2 bit -> prevA and prevB
    byte _currAB; // 2 bit -> currA and currB
    byte _buttonEvent; // 2 bit -> prevState and currState
};

#endif
