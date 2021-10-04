#pragma once

#include "Pulse.h"
#include "PulseByte.h"

// Pilot tone: before each block is a sequence of 8063 (header) or 3223 (data) pulses, each of length 2168 T-states.
// Sync pulses: the pilot tone is followed by two sync pulses of 667 and 735 T-states resp.
class PulsePreamble {

  Pulse _pilot;
  Pulse _sync1;
  Pulse _sync2;
  PulseByte _marker;
public:
  // 0x00 for header, 0xff for data
  void reset(int marker) {
    _pilot.reset(marker ? 3223 : 8063, 2168);
    _sync1.reset(1, 667);
    _sync2.reset(1, 735);
    _marker.reset(marker);
  }
  
  void advance(int *tstates, bool *pstate) {
    if (end()) return;
    _pilot.advance(tstates, pstate);
    _sync1.advance(tstates, pstate);
    _sync2.advance(tstates, pstate);
    _marker.advance(tstates, pstate);
  }

  bool end() {
    return _marker.end();
  }
};
