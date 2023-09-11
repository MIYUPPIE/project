#include <Servo.h>

// Motor driver pins
const int motor1Pin1 = 8;
const int motor1Pin2 = 9;
const int motor2Pin1 = 10;
const int motor2Pin2 = 11;

// IR sensor pins
const int leftIRPin = 2;
const int rightIRPin = 3;

// Ultrasonic sensor pins
const int trigPin = 6;
const int echoPin = 5;

// IR sensor threshold values for line detection
const int irThreshold = 500;

// Ultrasonic sensor threshold distance (in cm)
const int obstacleDistanceThreshold = 20;

// Servo motor pin
const int servoPin = 4;

// Servo motor angle for center position
const int servoCenterAngle = 90;

// Servo motor object
Servo servoMotor;

void setup() {
  // Configure pin modes for motor driver
  pinMode(motor1Pin1, OUTPUT);
  pinMode(motor1Pin2, OUTPUT);
  pinMode(motor2Pin1, OUTPUT);
  pinMode(motor2Pin2, OUTPUT);

  // Configure pin modes for IR sensors
  pinMode(leftIRPin, INPUT);
  pinMode(rightIRPin, INPUT);

  // Configure pin modes for ultrasonic sensor
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Attach the servo motor to the corresponding pin
  servoMotor.attach(servoPin);

  // Initialize the servo motor position to the center angle
  servoMotor.write(servoCenterAngle);
}

void loop() {
  // Check if a human is detected
  if (checkHuman()) {
    // Follow the human using IR sensors
    followHuman();
  } else {
    // No human detected, stop the robot
    motorControl(LOW, LOW, LOW, LOW);
  }
}

// Helper function to control the motor driver
void motorControl(int in1, int in2, int in3, int in4) {
  digitalWrite(motor1Pin1, in1);
  digitalWrite(motor1Pin2, in2);
  digitalWrite(motor2Pin1, in3);
  digitalWrite(motor2Pin2, in4);
}

// Helper function to check if a human is detected using IR sensors
bool checkHuman() {
  int leftIRValue = analogRead(leftIRPin);
  int rightIRValue = analogRead(rightIRPin);

  // Check if both IR sensors detect a human
  return (leftIRValue < irThreshold && rightIRValue < irThreshold);
}

// Helper function to measure the distance using the ultrasonic sensor
int getUltrasonicDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  int distance = duration * 0.034 / 2;
  return distance;
}

// Helper function to follow the human
void followHuman() {
  int distance = getUltrasonicDistance();

  // Check if an obstacle is detected
  if (distance <= obstacleDistanceThreshold) {
    // Stop the robot
    motorControl(LOW, LOW, LOW, LOW);
  } else {
    // Move towards the human
    motorControl(HIGH, LOW, HIGH, LOW); 
  }
}
