#include <TimerUtils.h>
String uint64ToString(uint64_t input) {
  String result = "";
  
  uint8_t base = 10;
  result.reserve(16);
  do {
    char c = input % base;
    input /= base;

    if (c < 10)
      c += '0';
    else
      c += 'A' - 10;
    result = c + result;
  } while (input);
  return result;
}

String msToString(uint32_t msecs) {
  uint32_t totalseconds = msecs / 1000;
  if (totalseconds == 0) return "now";

  // Note: uint32_t can only hold up to 45 days, so uint8_t is safe.
  uint8_t days = totalseconds / (60 * 60 * 24);
  uint8_t hours = (totalseconds / (60 * 60)) % 24;
  uint8_t minutes = (totalseconds / 60) % 60;
  uint8_t seconds = totalseconds % 60;

  String result = "";
  if (days)
    result += uint64ToString(days) + ' ' + String((days > 1) ? "days": "day");
  if (hours) {
    if (result.length()) result += ' ';
    result += uint64ToString(hours) + ' ' + String((hours > 1) ? "hours": "hour");
  }
  if (minutes) {
    if (result.length()) result += ' ';
    result += uint64ToString(minutes) + ' ' + String((minutes > 1) ? "minutes" : "minute");
  }
  if (seconds) {
    if (result.length()) result += ' ';
    result += uint64ToString(seconds) + ' ' + String((seconds > 1) ? "seconds" : "second");
  }
  return result;
}

String timeSince(uint32_t start){
return timeSinceOrNever(start+1);
}

String timeSinceOrNever(uint32_t start) {
  if (start == 0)
    return "Never";
  uint32_t diff;
  uint32_t now = millis();
  if (start < now)
    diff = now - start;
  else
    diff = UINT32_MAX - start + now;
  if (diff<1000) return "Just now";
  else return msToString(diff) + " ago";
}

// Timer Class
TimerMs::TimerMs(){
    start = millis();
}

void TimerMs::reset(){
    start = millis();
}

uint32_t TimerMs::elapsed() const{
    uint32_t now = millis();
    if (start <= now) return now - start; 
    else return UINT32_MAX - start + now;
}
