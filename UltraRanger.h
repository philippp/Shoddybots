#ifndef UltraRanger_h
#define UltraRanger_h

#include "Average.h"

#define SAMPLE_DEPTH 4
#define ULTRA_AN_PIN 0
#define MICROSECONDS_PER_INCH 147
#include "Arduino.h"

class UltraRanger {
 public:
  UltraRanger();
  ~UltraRanger();
  uint16_t GetRangeInInches();
  
 private:
  Average<uint32_t>* last_readings_;
  uint32_t reading_index_ = 0;
};

#endif  // UltraRanger_h
