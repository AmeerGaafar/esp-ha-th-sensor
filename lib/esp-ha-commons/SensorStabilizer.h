#ifndef ESP_HA_COMMONS_SENSOR_STABILIZER_H
#define ESP_HA_COMMONS_SENSOR_STABILIZER_H

#include <TimerUtils.h>

#define ACCUMULATED_READING_COUNT 6
#define NANVAL -999999999999.0f
#define TIME_THRESHOLD 5*60*1000

class SensorStabilizer {
 public:
  explicit SensorStabilizer(const String &name);
  void reset();
  float stableReading();
  void ingest(float reading);  
  int bufferSize();
  bool isReadingRecent();
  float latestStableReading() const;
  uint32_t latestStableReadingSince() const;
 private:
  int indexOfMax();
  int indexOfMin();
  float _readings[ACCUMULATED_READING_COUNT] = {NANVAL, NANVAL, NANVAL, NANVAL, NANVAL, NANVAL};
  int _current=0;
  float _lastReportedReading=NANVAL;
  uint32_t  _lastReportedReadingTS=0;
  TimerMs _latestReadingTS = TimerMs();
  uint32_t _timeThreshold = TIME_THRESHOLD;
  String _name;
};

#endif //ESP_HA_COMMONS_SENSOR_STABILIZER_H
