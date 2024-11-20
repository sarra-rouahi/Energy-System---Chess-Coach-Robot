#include <Servo.h>

// Déclaration des broches pour les LDRs
const int LDR1_PIN = A0;
const int LDR2_PIN = A1;

// Déclaration des broches pour les servomoteurs du panneau solaire
const int SERVO1_PIN = 9;
const int SERVO2_PIN = 10;
const int SERVO3_PIN = 11;
const int SERVO4_PIN = 12;

// Déclaration des broches pour le capteur ultrasonique
const int trigPin = 7;   // Broche Trigger
const int echoPin = 6;   // Broche Echo

// Déclaration du servomoteur pour le contrôle de la vanne
const int servoVannePin = 5;

// Variables pour la gestion de la distance
long duration;
float distance;

// Hauteur du bain et seuil pour le niveau minimal
const float hauteurBain = 30.0; // Hauteur totale en cm
const float seuilNiveau = hauteurBain / 3.0; // Niveau minimal = 1/3 de la hauteur totale

// Objets Servo
Servo panelServo1, panelServo2, panelServo3, panelServo4;
Servo servoVanne;

void setup() {
  // Attachement des servomoteurs
  panelServo1.attach(SERVO1_PIN);
  panelServo2.attach(SERVO2_PIN);
  panelServo3.attach(SERVO3_PIN);
  panelServo4.attach(SERVO4_PIN);
  servoVanne.attach(servoVannePin);

  // Initialisation des broches du capteur ultrasonique
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Position initiale de la vanne (fermée)
  servoVanne.write(0);

  // Démarrage de la communication série
  Serial.begin(9600);
}

void loop() {
  // Suivi solaire
  int ldr1 = analogRead(LDR1_PIN);
  int ldr2 = analogRead(LDR2_PIN);

  Serial.print("LDR1: ");
  Serial.print(ldr1);
  Serial.print("\tLDR2: ");
  Serial.println(ldr2);

  int angle = map(abs(ldr1 - ldr2), 0, 1023, 0, 180);

  if (ldr1 > ldr2) {
    panelServo1.write(angle);
    panelServo4.write(180 - angle);
  } else if (ldr2 > ldr1) {
    panelServo2.write(angle);
    panelServo3.write(180 - angle);
  }

  // Contrôle du niveau d'eau
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = (duration * 0.0343) / 2;

  Serial.print("Distance mesurée : ");
  Serial.print(distance);
  Serial.println(" cm");

  if (distance < seuilNiveau) {
    Serial.println("Niveau insuffisant, ouverture de la vanne.");
    servoVanne.write(90); // Ouvre la vanne
  } else {
    Serial.println("Niveau suffisant, fermeture de la vanne.");
    servoVanne.write(0); // Ferme la vanne
  }

  // Délai entre les cycles
  delay(1000);
}
