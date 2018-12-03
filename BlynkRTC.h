#ifndef BlynkRTC_h
#define BlynkRTC_h

#include <Blynk/BlynkTemplates.h>


#define DEAULT_SYNC_INTERVAL 300 // 300s = 5 minute
#define LEAP_YEAR(Y)     ( ((1970+Y)>0) && !((1970+Y)%4) && ( ((1970+Y)%100) || !((1970+Y)%400) ) )


class BlynkRTC
  : public BlynkSingleton<BlynkRTC>
{
  public:
    BlynkRTC() {}
    //------------------------------------
    void begin(uint32_t syncIntervel = DEAULT_SYNC_INTERVAL) {
      _isSyncOK = false;
      _syncInterval = syncIntervel;
      Blynk.sendInternal("rtc", "sync");
    }
    //------------------------------------
    void handle() {
      if (!_isSyncOK) return;

      uint32_t currMillis = millis();
      if (currMillis - _prevMillis >= 1000) {
        _prevMillis = currMillis;
        _now++;
        parseTime();

        if (currMillis >= _syncNextTime) {
          _syncNextTime = currMillis + _syncInterval * 1000;
          Blynk.sendInternal("rtc", "sync");
        }
      }
    }
    //------------------------------------
    void syncTime(uint32_t blynkTime) {
      _now = blynkTime;
      _prevMillis = millis();
      _syncNextTime = _prevMillis + _syncInterval * 1000;

      parseTime();
      _isSyncOK = true;
    };
    //------------------------------------
    char* getDateTimeStr() {
      return _strDateTime;
    };
    //------------------------------------
    char* getTimeStr() {
      return _strDateTime + 11;
    };
    //------------------------------------
    uint32_t now() {
      return _now;
    }
    uint32_t time() {
      return _now;
    }
    uint16_t year() {
      return _yyyy;
    }
    uint8_t month() {
      return _mm;
    }
    uint8_t day() {
      return _dd;
    }
    uint8_t hour() {
      return _HH;
    }
    uint8_t minute() {
      return _MM;
    }
    uint8_t second() {
      return _SS;
    }

  private:
    void parseTime() {
      uint32_t timeParts = _now;

      // Parse Time
      _SS = timeParts % 60;
      timeParts /= 60; // now it is minutes
      _MM = timeParts % 60;
      timeParts /= 60; // now it is hours
      _HH = timeParts % 24;
      timeParts /= 24; // now it is days;
      _time = _now - timeParts;

      // Parse Year
      uint8_t yearOffset = 0; //offset from 1970
      unsigned long dayOffset = 0;
      while ((unsigned)(dayOffset += (LEAP_YEAR(yearOffset) ? 366 : 365)) <= timeParts) {
        yearOffset++;
      }
      _yyyy = 1970 + yearOffset;

      dayOffset -= LEAP_YEAR(yearOffset) ? 366 : 365;
      timeParts -= dayOffset; // days in this year, starting at 0

      // Parse Month/Day
      uint8_t month, monthLength;
      for (month = 1; month <= 12; month++) {
        monthLength = _monthDays[month - 1];
        if ((month == 2) && (LEAP_YEAR(yearOffset))) monthLength++;

        if (timeParts >= monthLength) timeParts -= monthLength;
        else break;
      }
      _mm = month;
      _dd = timeParts + 1;     // day of month
      updateDateTimeStr();
    }

    void updateDateTimeStr() {
      _strDateTime[0] = _dd / 10 + '0';
      _strDateTime[1] = _dd % 10 + '0';
      _strDateTime[3] = _mm / 10 + '0';
      _strDateTime[4] = _mm % 10 + '0';
      itoa(_yyyy, _strDateTime + 6, 10);

      _strDateTime[10] = ' ';

      _strDateTime[11] = _HH / 10 + '0';
      _strDateTime[12] = _HH % 10 + '0';
      _strDateTime[14] = _MM / 10 + '0';
      _strDateTime[15] = _MM % 10 + '0';
      _strDateTime[17] = _SS / 10 + '0';
      _strDateTime[18] = _SS % 10 + '0';
    }

    uint32_t _now, _time;
    uint16_t _yyyy;
    uint8_t _mm, _dd, _HH, _MM, _SS;
    bool _isSyncOK;
    uint32_t _syncInterval;
    uint32_t _syncNextTime;
    uint32_t _prevMillis;
    char _strDateTime[20] = {'0', '0', '-', '0', '0', '-', '2', '0', '0', '0', ' ', '0', '0', ':', '0', '0', ':', '0', '0', '\0'};
    const uint8_t _monthDays[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
};
BlynkRTC rtc;

inline

BLYNK_WRITE(InternalPinRTC) {
  rtc.syncTime(param.asLong());
  BLYNK_LOG2(BLYNK_F("Time sync: "), rtc.getDateTimeStr());
}

#endif
