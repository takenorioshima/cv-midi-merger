#include <MIDI.h>

MIDI_CREATE_DEFAULT_INSTANCE();

const int gateInPin = A0;
const int cvInPin = A1;

int gateInValue = 0;
int cvInValue = 0;
int gateInPrevValue;
int cvInPrevValue;

void setup() {
  MIDI.begin(MIDI_CHANNEL_OMNI);
}

void loop() {
  gateInValue = map(analogRead(gateInPin), 0, 1023, 0, 127);
  cvInValue = map(analogRead(cvInPin), 0, 1023, 0, 127);
  
  if( gateInValue != gateInPrevValue ){
    MIDI.sendControlChange(45, gateInValue, 1);
    gateInPrevValue = gateInValue;
  }

  if( cvInValue != cvInPrevValue ){
    MIDI.sendControlChange(46, cvInValue, 1);
    cvInPrevValue = cvInValue;
  }

  MIDI.read();
}
