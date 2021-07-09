#include <Arduino.h>
#include <SensorStabilizer.h>
#include <logging.h>

SensorStabilizer::SensorStabilizer(const String &name){
  this->_name=name;
  reset();
}

int SensorStabilizer::bufferSize() {
  return sizeof(_readings) / sizeof(_readings[0]);
}

void SensorStabilizer::reset(){
  for (int i=0;i<bufferSize();i++) _readings[i]=NANVAL;
  _latestReadingTS.reset();
    _current=0;
}

float SensorStabilizer::stableReading() {
  int imax=indexOfMax();
  int imin=indexOfMin();
  float sum=0.0f;
  float readingCount=0.0f;
  float result=NANVAL;
  
    for (int i=0;i<bufferSize();i++){
        if (i!=imax && i!=imin) {
          sum+=_readings[i];
          readingCount++;
        }
    }

    result=sum/readingCount;
  
    String s=String("{");
    for (int i=0;i<bufferSize();i++) {
      s+=String(_readings[i]);
      if (i<bufferSize()-1) s+=",";
    }
    s+="}";
    LOG_INFO("%s Reading Requested: %s -> %.2f", _name.c_str(), s.c_str(), result)
    _lastReportedReading=result;
    _lastReportedReadingTS=millis();
  return result;
}

bool SensorStabilizer::isReadingRecent() {
  return _latestReadingTS.elapsed() < _timeThreshold;
}

float SensorStabilizer::latestStableReading() const {
    return _lastReportedReading;
}

uint32_t SensorStabilizer::latestStableReadingSince() const {
    return _lastReportedReadingTS;
}

int SensorStabilizer::indexOfMax(){
int r=0;
  for (int i=1;i<bufferSize();i++){
    if (_readings[i] > _readings[r]) r=i;
  }
return r;
}

int SensorStabilizer::indexOfMin(){
int r=0;
  for (int i=1;i<bufferSize();i++){
    if (_readings[i] < _readings[r]) r=i;
  }
return r;
}
  
void SensorStabilizer::ingest(float reading){
    _readings[_current]=reading;
  _current++;
  if (_current >= bufferSize()) _current=0;

  _latestReadingTS.reset();

  // set all unset _readings to this value, better than nothing
  for (int i=0;i<bufferSize();i++) {if (_readings[i] == NANVAL) _readings[i]=reading;}
}
