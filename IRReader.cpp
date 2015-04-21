#include "IRReader.h"
#include "roku_pulse_widths.h"
//#define DEBUG 1
bool IRReader::Compare(int a, int b) {
  return ((a == b) || (a + 1 == b) || (a - 1 == b));
}

IRReader::IRReader() {};

bool IRReader::GetButton(RemoteButton* button) {
  bool pulse_continued = true;
  Cycle signals_burst[50];
  Cycle cycle;
  cycle_count_ = 0;
  while (pulse_continued) {
    pulse_continued = GetCycle(&cycle);
    if (pulse_continued) {
        signals_burst[cycle_count_] = cycle;
    }
  }
#ifdef DEBUG
  Serial.print(cycle_count_, DEC);
  Serial.println(F(" cycles."));
  for(uint8_t i = 0; i < cycle_count_; ++i) {
    Serial.print(signals_burst[i].low, DEC);
    Serial.print(F(", "));
    Serial.print(signals_burst[i].high, DEC);
    Serial.println(" ");
  } 
#endif  // DEBUG
  return MatchButton(signals_burst, button);

}

bool IRReader::GetCycle(Cycle* cycle) {
  uint16_t highpulse = 0;  // How long was the pulse high, in RESOLUTION ms
  uint16_t lowpulse = 0;   // ... low
  cycle->high = 0;         // How long was the pulse high, after we round it down.
  cycle->low = 0;          // How long was the pulse low, after we round it down.
  while (IRpin_PIN & (1 << IRpin)) {
     // pin is still HIGH
     // count off another few microseconds
     highpulse++;
     delayMicroseconds(RESOLUTION);
 
     // If the pulse is too long, we 'timed out' - either nothing
     // was received or the code is finished, so print what
     // we've grabbed so far, and then reset
     if (highpulse >= MAXPULSE) {
       return false;
     }
  }
  // we didn't time out so lets stash the reading. We divide by 8 to avoid mismatching
  // on the low bits.
  cycle->high = highpulse >> 3;
  // same as above
  while (! (IRpin_PIN & _BV(IRpin))) {
     // pin is still LOW
     lowpulse++;
     delayMicroseconds(RESOLUTION);
     if (lowpulse >= MAXPULSE) {
       return false;
     }
  }
  // Again we divide by 8 to avoid mismatching on the low bits.
  cycle->low = lowpulse >> 3;
  cycle_count_ += 1;
  return true;
}

bool IRReader::MatchButton(Cycle (&signals_burst)[50], RemoteButton* matched_button) {
  // Bitmap of buttons that match the signal. With every cycle, we clear the bits of the
  // buttons whose expected pulse widths don't match what we're seeing.
  uint32_t possible_buttons = ((1 << Pause+1) - 1);
  // Fast-forward/skip through the signal until we see the start sequence.
  bool fast_forward = true;
  // The index of the signals we are comparing to. This is only incremented after
  // we see the start sequence
  uint32_t match_cycle_index = 0;
  for (uint32_t i = 0; i < cycle_count_; ++i) {
    const Cycle& match_cycle = signals_burst[i];
#ifdef DEBUG
    Serial.print("low=");
    Serial.print(match_cycle.low, DEC);
    Serial.print(" high=");
    Serial.print(match_cycle.high, DEC);
    Serial.print(" match_index=");     
    Serial.print(match_cycle_index);
    Serial.print(" array_index=");     
    Serial.println(2 * match_cycle_index + 1, DEC);
#endif  // DEBUG
    if (Compare(match_cycle.high, START_SEQUENCE[1])) {
      fast_forward = false;
      continue;
    }
    if (fast_forward) {
      continue;
    }
    uint32_t expected_width = WIDTHS_UP[2 * match_cycle_index + 1];
    if (!Compare(match_cycle.high, expected_width)) {
        possible_buttons &= ~(1 << Up);
    }
    expected_width = WIDTHS_DOWN[2 * match_cycle_index + 1];
    if (!Compare(match_cycle.high, expected_width)) {
        possible_buttons &= ~(1 << Down);
    }
    expected_width = WIDTHS_LEFT[2 * match_cycle_index + 1];
    if (!Compare(match_cycle.high, expected_width)) {
        possible_buttons &= ~(1 << Left);
    }
    expected_width = WIDTHS_RIGHT[2 * match_cycle_index + 1];
    if (!Compare(match_cycle.high, expected_width)) {
        possible_buttons &= ~(1 << Right);
    }
    expected_width = WIDTHS_SELECT[2 * match_cycle_index + 1];
    if (!Compare(match_cycle.high, expected_width)) {
        possible_buttons &= ~(1 << Select);
    }
    expected_width = WIDTHS_HOME[2 * match_cycle_index + 1];
    if (!Compare(match_cycle.high, expected_width)) {
        possible_buttons &= ~(1 << Home);
    }
    expected_width = WIDTHS_BACK[2 * match_cycle_index + 1];
    if (!Compare(match_cycle.high, expected_width)) {
        possible_buttons &= ~(1 << Back);
    }
    expected_width = WIDTHS_PAUSE[2 * match_cycle_index + 1];
    if (!Compare(match_cycle.high, expected_width)) {
        possible_buttons &= ~(1 << Pause);
    }
    expected_width = WIDTHS_FORWARD[2 * match_cycle_index + 1];
    if (!Compare(match_cycle.high, expected_width)) {
        possible_buttons &= ~(1 << Forward);
    }
    ++match_cycle_index;
#ifdef DEBUG
    Serial.print(possible_buttons, BIN);
    Serial.println(" remaining button map");
#endif  // DEBUG
  } 

  for (uint32_t i = 0; i <= Pause; ++i) {
    if (possible_buttons == (1 << i)) {
      *matched_button = (RemoteButton)i;
      return true;
    }
  }
  return false;
}

