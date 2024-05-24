#include <MIDI.h>
#include <ResponsiveAnalogRead.h>

MIDI_CREATE_DEFAULT_INSTANCE();

const int gateInPin = A0;
const int cvInPin = A1;

ResponsiveAnalogRead analogGateIn(gateInPin, true);
ResponsiveAnalogRead analogCvIn(cvInPin, true);

int gateInValue = 0;
int cvInValue = 0;
int gateInPrevValue;
int cvInPrevValue;

void setup() {
  MIDI.begin(MIDI_CHANNEL_OMNI);
}

void loop() {
  analogGateIn.update();
  analogCvIn.update();

  // Gate input.
  if(analogGateIn.hasChanged()) {
    gateInValue = (analogGateIn.getValue() < 512) ? 0 :127;
    if( gateInValue != gateInPrevValue ){
      MIDI.sendControlChange(46, gateInValue, 1);
      gateInPrevValue = gateInValue;
    }
  }

  // CV input.
  if(analogCvIn.hasChanged()) {
    cvInValue = map(analogCvIn.getValue(), 0, 1023, 0, 127);
    if( cvInValue != cvInPrevValue ){
      MIDI.sendControlChange(47, cvInValue, 1);
      cvInPrevValue = cvInValue;
    }
  }

  MIDI.read();
}
