#include <PID_v1.h>
#include <Encoder.h>

// ===== PIN DEFINITIONS =====
const int RPWM = 3;
const int LPWM = 4;

const int ENC_A = 2;
const int ENC_B = 7;

// ===== ENCODER =====
Encoder motorEnc(ENC_A, ENC_B);

// ===== SYSTEM PARAMETERS =====
const double countsPerRev = 5760.0;   
const double degreesPerCount = 360.0 / countsPerRev;

// ===== PID VARIABLES =====
double setpoint = 90.0;   // target angle (degrees)
double input    = 0.0;    // current angle
double output   = 0.0;    // motor command

const double angleTolerance = .0;  // degrees

// ===== PID TUNING =====
double Kp = 7.0;
double Ki = 2.;
double Kd = 1.75;

PID motorPID(&input, &output, &setpoint, Kp, Ki, Kd, DIRECT);

// ===== LIMITS =====
const int maxPWM = 255;
const int deadband = 15;

// ===== TIMING =====
const unsigned long sampleTimeMs = 20;  // faster for position control
unsigned long lastTime = 0;

void setup() {
  Serial.begin(115200);

  pinMode(RPWM, OUTPUT);
  pinMode(LPWM, OUTPUT);
 
pinMode(ENC_A, INPUT_PULLUP);
pinMode(ENC_B, INPUT_PULLUP);

  analogWriteFrequency(RPWM, 20000);
  analogWriteFrequency(LPWM, 20000);

  // PID setup
  motorPID.SetOutputLimits(-maxPWM, maxPWM);
  motorPID.SetSampleTime(sampleTimeMs);
  motorPID.SetMode(AUTOMATIC);
}

void loop() {
  unsigned long now = millis();

  if (now - lastTime >= sampleTimeMs) {

    // ===== READ POSITION =====
    long counts = motorEnc.read();
    input = counts * degreesPerCount;

    double error = setpoint - input;

    // ===== STOP CONDITION =====
    if (abs(error) < angleTolerance) {

      // BRAKE motor (important!)
      analogWrite(RPWM, 255);
      analogWrite(LPWM, 255);

      // Reset PID (prevents drift)
      motorPID.SetMode(MANUAL);
      output = 0;
      motorPID.SetMode(AUTOMATIC);

    } 
    else {
      // ===== PID =====
      motorPID.Compute();

      int pwm = (int)output;

      if (pwm > maxPWM) pwm = maxPWM;
      if (pwm < -maxPWM) pwm = -maxPWM;

if (pwm > 0) {
  analogWrite(RPWM, 0);
  analogWrite(LPWM, pwm);
} else {
  analogWrite(RPWM, -pwm);
  analogWrite(LPWM, 0);
}
    }

    Serial.print("Angle: ");
    Serial.print(input);
    Serial.print(" | Error: ");
    Serial.print(error);
    Serial.print(" | PWM: ");
    Serial.println(output);

    lastTime = now;
  }
}
