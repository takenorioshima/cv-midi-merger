#include <MIDI.h>
#include <ResponsiveAnalogRead.h>
#include <SoftwareSerial.h>

SoftwareSerial softSerial(2, 3);  // RX, TX
MIDI_CREATE_INSTANCE(HardwareSerial, Serial, midiA);
MIDI_CREATE_INSTANCE(SoftwareSerial, softSerial, midiB);

const int gateInPin = A0;
const int cvInPin = A1;

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

  pinMode(5, OUTPUT);  // LED_GATE - PD5
  pinMode(6, OUTPUT);  // LED_CV - PD6
  pinMode(7, OUTPUT);  // LED_MIDI_A - PD7
  pinMode(8, OUTPUT);  // LED_MIDI_B - PB0
}

void loop() {
  unsigned long currentMillis = millis();

  analogGateIn.update();
  analogCvIn.update();

  // Read gate input.
  if (analogGateIn.hasChanged()) {
    gateInValue = (analogGateIn.getValue() < 512) ? 0 : 127;
    if (gateInValue != gateInPrevValue) {
      midiA.sendControlChange(79, gateInValue, 1);
      if (gateInValue == 127) {
        digitalWrite(5, HIGH);
        prevMillisGate = currentMillis;
      }
      gateInPrevValue = gateInValue;
    }
  }

  // Read CV input.
  if (analogCvIn.hasChanged()) {
    cvInValue = map(analogCvIn.getValue(), 255, 768, 0, 127);
    if (cvInValue != cvInPrevValue) {
      midiA.sendControlChange(47, cvInValue, 1);
      digitalWrite(6, HIGH);
      prevMillisCv = currentMillis;
      cvInPrevValue = cvInValue;
    }
  }

  // Rad MIDI.
  if (midiA.read()) {
    midi::MidiType type = midiA.getType();
    if (midiA.isChannelMessage(type)) {
      digitalWrite(7, HIGH);
      prevMillisMidiA = currentMillis;
    }
  };

  if (midiB.read()) {
    midi::MidiType type = midiB.getType();
    if (midiB.isChannelMessage(type)) {
      digitalWrite(8, HIGH);
      prevMillisMidiB = currentMillis;
    }
  };

  // Status LEDs.
  if (currentMillis - prevMillisGate >= ledInterval) {
    digitalWrite(5, LOW);
  }
  if (currentMillis - prevMillisCv >= ledInterval) {
    digitalWrite(6, LOW);
  }
  if (currentMillis - prevMillisMidiA >= ledInterval) {
    digitalWrite(7, LOW);
  }
  if (currentMillis - prevMillisMidiB >= ledInterval) {
    digitalWrite(8, LOW);
  }
}