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

const long ledInterval = 50;  // millsecconds.
unsigned long prevMillisGate = 0;
unsigned long prevMillisCv = 0;

void setup() {
  MIDI.begin(MIDI_CHANNEL_OMNI);

  pinMode(5, OUTPUT);  // LED_GATE - PD5
  pinMode(6, OUTPUT);  // LED_CV - PD6
}

void loop() {
  analogGateIn.update();
  analogCvIn.update();

  unsigned long currentMillis = millis();

  // Gate input.
  if (analogGateIn.hasChanged()) {
    gateInValue = (analogGateIn.getValue() < 512) ? 0 : 127;
    if (gateInValue != gateInPrevValue) {
      MIDI.sendControlChange(79, gateInValue, 1);
      if (gateInValue == 127) {
        digitalWrite(5, HIGH);
        prevMillisGate = currentMillis;
      }
      gateInPrevValue = gateInValue;
    }
  }

  // CV input.
  if (analogCvIn.hasChanged()) {
    cvInValue = map(analogCvIn.getValue(), 255, 768, 0, 127);
    if (cvInValue != cvInPrevValue) {
      MIDI.sendControlChange(47, cvInValue, 1);
      digitalWrite(6, HIGH);
      prevMillisCv = currentMillis;
      cvInPrevValue = cvInValue;
    }
  }

  // Turn off LEDs.
  if (currentMillis - prevMillisGate >= ledInterval) {
    digitalWrite(5, LOW);
  }
  if (currentMillis - prevMillisCv >= ledInterval) {
    digitalWrite(6, LOW);
  }
}
