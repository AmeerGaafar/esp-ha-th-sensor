#ifndef ESP_HA_COMMONS_TIMERUTILS_H
#define ESP_HA_COMMONS_TIMERUTILS_H

#include <Arduino.h>

String timeSinceOrNever(uint32_t start);
String timeSince(uint32_t start);
String msToString(uint32_t msecs);

class TimerMs {
 public:
  TimerMs();
  void reset();
  uint32_t elapsed() const;
 private:
  uint32_t start;
};

#endif //ESP_HA_COMMONS_TIMERUTILS_H
