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
  midiB.sendNoteOn(pitch, velocity, channel);
}

void handleNoteOff(byte channel, byte pitch, byte velocity) {
  midiB.sendNoteOff(pitch, velocity, channel);
}

void handleControlChange(byte channel, byte number, byte value) {
  midiB.sendControlChange(number, value, channel);
}

void handlePitchBend(byte channel,int value){
  Serial.println(value);
  midiB.sendPitchBend(value, channel);
}

void setup() {
  Serial.begin(31250);

  softSerial.begin(31250);
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

  midiB.setHandlePitchBend(handlePitchBend);

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
      midiB.sendControlChange(79, gateInValue, 1);
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
      digitalWrite(6, HIGH);
      midiB.sendControlChange(47, cvInValue, 1);
      prevMillisCv = currentMillis;
      cvInPrevValue = cvInValue;
    }
  }

  // Rad MIDI.
  if (midiA.read()) {
    if (midiA.isChannelMessage(midiA.getType())) {
      digitalWrite(7, HIGH);
      prevMillisMidiA = currentMillis;
    }
  };

  if (midiB.read()) {
    if (midiB.isChannelMessage(midiB.getType())) {
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