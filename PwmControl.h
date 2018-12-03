#ifndef PwmControl_H
#define PwmControl_H

class PwmControl
{
  public:
    PwmControl(uint8_t pinPWM, int16_t minValue, int16_t maxValue, uint16_t pwmFreq) {
      _pinPWM = pinPWM;
      _minValue = minValue;
      _maxValue = maxValue;
      _pwmFreq = pwmFreq;
    }
    // ---------------------------------------------------------------------------
    void init() { analogWriteFreq(_pwmFreq); }
    // ---------------------------------------------------------------------------
    void setValue(int16_t value) {
      if (_value == value) return;
      _value = constrain(value, _minValue, _maxValue);
      analogWrite(_pinPWM, map(_value, _minValue, _maxValue, 0, PWMRANGE));
    }
    // ---------------------------------------------------------------------------
    int16_t getValue() { return _value; }
    float getDuty() { return (_value - _minValue) * 1.0 / (_maxValue - _minValue); }
    void setFrequence(uint16_t pwmFreq) { analogWriteFreq(pwmFreq); }
    void inc(int16_t step = 1) { setValue(_value + step); }
    void dec(int16_t step = 1) { setValue(_value - step); }

  private:
    uint8_t _pinPWM;
    int16_t _value;
    int16_t _minValue;
    int16_t _maxValue;
    uint16_t _pwmFreq;
};

#endif
