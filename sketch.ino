#include <Servo.h>

// Declaration of pins for the LDRs
const int LDR1_PIN = A0;
const int LDR2_PIN = A1;

// Declaration of pins for the solar panel servomotors
const int SERVO1_PIN = 9;
const int SERVO2_PIN = 10;
const int SERVO3_PIN = 11;
const int SERVO4_PIN = 12;

// Declaration of pins for the ultrasonic sensor
const int TRIG_PIN = 7;   // Trigger pin
const int ECHO_PIN = 6;   // Echo pin

// Declaration of the servomotor for valve control
const int VALVE_SERVO_PIN = 5;

// Variables for distance management
long signalDuration;
float measuredDistance;


const float TANK_HEIGHT = 30.0; // Total height in cm
const float MIN_LEVEL_THRESHOLD = TANK_HEIGHT / 3.0; // Minimum level = 1/3 of the total height


Servo panelServo1, panelServo2, panelServo3, panelServo4;
Servo valveServo;

void setup() {
  
  panelServo1.attach(SERVO1_PIN);
  panelServo2.attach(SERVO2_PIN);
  panelServo3.attach(SERVO3_PIN);
  panelServo4.attach(SERVO4_PIN);
  valveServo.attach(VALVE_SERVO_PIN);

  // Initializing pins for the ultrasonic sensor
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Initial position of the valve (closed)
  valveServo.write(0);

  // Starting serial communication
  Serial.begin(9600);
}

void loop() {
  // Solar tracking
  int ldr1Value = analogRead(LDR1_PIN);
  int ldr2Value = analogRead(LDR2_PIN);

  Serial.print("LDR1: ");
  Serial.print(ldr1Value);
  Serial.print("\tLDR2: ");
  Serial.println(ldr2Value);

  int calculatedAngle = map(abs(ldr1Value - ldr2Value), 0, 1023, 0, 180);

  if (ldr1Value > ldr2Value) {
    panelServo1.write(calculatedAngle);
    panelServo4.write(180 - calculatedAngle);
  } else if (ldr2Value > ldr1Value) {
    panelServo2.write(calculatedAngle);
    panelServo3.write(180 - calculatedAngle);
  }

  // Water level control
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  signalDuration = pulseIn(ECHO_PIN, HIGH);
  measuredDistance = (signalDuration * 0.0343) / 2;

  Serial.print("Measured distance: ");
  Serial.print(measuredDistance);
  Serial.println(" cm");

  if (measuredDistance < MIN_LEVEL_THRESHOLD) {
    Serial.println("Insufficient level, opening the valve.");
    valveServo.write(90); // Open the valve
  } else {
    Serial.println("Sufficient level, closing the valve.");
    valveServo.write(0); // Close the valve
  }

  // Delay between cycles
  delay(1000);
}

