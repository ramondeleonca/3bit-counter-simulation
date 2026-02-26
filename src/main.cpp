#include <Arduino.h>

// JK flip-flop state variables
// Flip flop 3 IO
uint8_t J3 = 0;
uint8_t K3 = 0;
uint8_t Q3 = 0;

// Flip flop 2 IO
uint8_t J2 = 0;
uint8_t K2 = 0;
uint8_t Q2 = 0;

// Flip flop 1 IO
uint8_t J1 = 0;
uint8_t K1 = 0;
uint8_t Q1 = 0;

// Flip flop 0 IO
uint8_t J0 = 0;
uint8_t K0 = 0;
uint8_t Q0 = 0;

// Pinouts
// Control pins
const uint8_t buttonPin = 10;
const uint8_t buzzerPin = 11;

// Display pins
const uint8_t bit0Pin = 3;
const uint8_t bit1Pin = 5;
const uint8_t bit2Pin = 6;
const uint8_t bit3Pin = 9;

bool jkUpdate(uint8_t currentQ, uint8_t J, uint8_t K) {
  if (J == 0 && K == 0) return currentQ;       // No change
  if (J == 0 && K == 1) return 0;              // Reset
  if (J == 1 && K == 0) return 1;              // Set
  if (J == 1 && K == 1) return !currentQ;      // Toggle
  return currentQ;
}

void clk() {
  uint8_t J1 = 1,      K1 = 1;
  uint8_t J2 = Q1,     K2 = Q1;
  uint8_t J3 = Q1 && Q2, K3 = Q1 && Q2;

  Q3 = jkUpdate(Q3, J3, K3);
  Q2 = jkUpdate(Q2, J2, K2);
  Q1 = jkUpdate(Q1, J1, K1);
}

void setup() {
  Serial.begin(9600);

  // Pin modes
  pinMode(buttonPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(bit0Pin, OUTPUT);
  pinMode(bit1Pin, OUTPUT);
  pinMode(bit2Pin, OUTPUT);
  pinMode(bit3Pin, OUTPUT);
}

uint8_t lastButtonState = 0;
uint16_t breathePeriodMs = 750;
uint8_t baseBreatheDutyCycle = 180;
uint16_t buzzerFreq = 1000;
uint16_t buzzerDurationMs = 100;
uint32_t buzzerStartTime = 0;
uint32_t clkTime = 0;
void loop() {
  uint8_t currentButtonState = digitalRead(buttonPin);

  // Clock counter
  if (currentButtonState == HIGH && lastButtonState == LOW) {
    clk();
    tone(buzzerPin, buzzerFreq);
    buzzerStartTime = millis();
    clkTime = millis();
  }

  // Update display
  uint16_t dutyCycle = baseBreatheDutyCycle + ((255 - baseBreatheDutyCycle) * sin(2 * PI * (millis() - clkTime) / breathePeriodMs));
  analogWrite(bit0Pin, Q0 * dutyCycle);
  analogWrite(bit1Pin, Q1 * dutyCycle);
  analogWrite(bit2Pin, Q2 * dutyCycle);
  analogWrite(bit3Pin, Q3 * dutyCycle);

  // Stop buzzer after duration
  if (buzzerStartTime > 0 && millis() - buzzerStartTime >= buzzerDurationMs) {
    noTone(buzzerPin);
    buzzerStartTime = 0;
  }

  lastButtonState = currentButtonState;
  delay(10);
}
