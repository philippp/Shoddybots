// IR Reader Module, adapted from https://learn.adafruit.com/ir-sensor/using-an-ir-sensor

#ifndef IRReader_h
#define IRReader_h

#include "Arduino.h"

// Digital pin #8 is the same as Pin B0 see
// http://arduino.cc/en/Hacking/PinMapping168 for the 'raw' pin mapping
#define IRpin_PIN PINB
#define IRpin 0
 
// The maximum pulse we'll listen for: (MAXPULSE * RESOLUTION) microseconds.
#define MAXPULSE 1500
 
// The base unit of timing resolution, we use this for the initial capture
// cycle and then round down by bitshifting for easy matching.
#define RESOLUTION 20

// Cycle timing info.
struct Cycle {
  uint32_t low;
  uint32_t high;
};

// Remote control buttons we are listening for.
enum RemoteButton {
  Up,
  Down,
  Left,
  Right,
  Select,
  Home,
  Back,
  Forward,
  Pause,
};

class IRReader {
  public:
    IRReader();

    // Listen for a remote control button press. Returns true and sets the pointer
    // if a button was detected, returns false if no button was detected / timeout occurred.
    bool GetButton(RemoteButton* button);
    
  private:  
    // Measures a single low/high cycle. Returns false on timeout, true if a cycle was detected.
    bool GetCycle(Cycle* cycle);

    // Comparison that allows for +/- 1 mismatches.
    bool Compare(int a, int b);

    // Given an array of cycles, tells us which button matches. Returns true if a
    // button matched, false if no button matched.
    bool MatchButton(Cycle (&signals_burst)[50], RemoteButton* matched_button);

    // How many cycles have we seen since GetButton() was called?
    int32_t cycle_count_;

};

#endif // IRReader_h
