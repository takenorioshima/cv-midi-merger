#include <MIDI.h>
#include <ResponsiveAnalogRead.h>
#include <SoftwareSerial.h>

SoftwareSerial softSerial(2, 3);  // RX, TX
MIDI_CREATE_INSTANCE(HardwareSerial, Serial, midiA);
MIDI_CREATE_INSTANCE(SoftwareSerial, softSerial, midiB);

const int gateInPin = A1;
const int cvInPin = A0;

ResponsiveAnalogRead analogGateIn(gateInPin, true);
ResponsiveAnalogRead analogCvIn(cvInPin, true);

int gateInValue = 0;
int cvInValue = 0;
int gateInPrevValue;
int cvInPrevValue;

const long ledInterval = 50;  // millsecconds.
unsigned long prevMillisGate = 0;
unsigned long prevMillisCv = 0;
unsigned long prevMillisMidiA = 0;
unsigned long prevMillisMidiB = 0;

void handleNoteOn(byte channel, byte pitch, byte velocity) {
  midiA.sendNoteOn(pitch, velocity, channel);
}

void handleNoteOff(byte channel, byte pitch, byte velocity) {
  midiA.sendNoteOff(pitch, velocity, channel);
}

void handleControlChange(byte channel, byte number, byte value) {
  midiA.sendControlChange(number, value, channel);
}

void handlePitchBend(byte channel,int value){
  midiA.sendPitchBend(value, channel);
}

void setup() {
  midiA.begin(MIDI_CHANNEL_OMNI);
  midiB.begin(MIDI_CHANNEL_OMNI);

  midiA.turnThruOff();
  midiB.turnThruOff();

  midiA.setHandleNoteOn(handleNoteOn);
  midiB.setHandleNoteOn(handleNoteOn);
  midiA.setHandleNoteOff(handleNoteOff);
  midiB.setHandleNoteOff(handleNoteOff);

  midiA.setHandleControlChange(handleControlChange);
  midiB.setHandleControlChange(handleControlChange);

  midiA.setHandlePitchBend(handlePitchBend);
  midiB.setHandlePitchBend(handlePitchBend);
}

void loop() {
  analogGateIn.update();
  analogCvIn.update();

  // Read gate input.
  if (analogGateIn.hasChanged()) {
    gateInValue = (analogGateIn.getValue() < 512) ? 0 : 127;
    if (gateInValue != gateInPrevValue) {
      midiA.sendControlChange(79, gateInValue, 1);
      gateInPrevValue = gateInValue;
    }
  }

  // Read CV input.
  if (analogCvIn.hasChanged()) {
    cvInValue = map(analogCvIn.getValue(), 255, 768, 0, 127);
    if (cvInValue != cvInPrevValue) {
      midiA.sendControlChange(47, cvInValue, 1);
      cvInPrevValue = cvInValue;
    }
  }

  // Read MIDI.
  midiA.read();
  midiB.read();
}