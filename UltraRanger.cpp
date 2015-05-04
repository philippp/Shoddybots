#include "UltraRanger.h"

UltraRanger::UltraRanger() {
  last_readings_ = new Average<uint32_t>(SAMPLE_DEPTH);
}

UltraRanger::~UltraRanger() {
  delete last_readings_;
}

uint16_t UltraRanger::GetRangeInInches() {
  last_readings_->push((pulseIn(10, HIGH) << 6) >> 6);
  return (last_readings_->mode() / (MICROSECONDS_PER_INCH));
}

