#include <AccelStepper.h>
#include <MultiStepper.h>
#include <Servo.h>
#define MotorInterfaceType 4
const int JOINT_LENGTH = 100;
const int MAX_SPEED = 1000;
const int STEPS = 2048;
const int shoulderToElbow = 100;
const int elbowToPen = 120;

typedef struct {
  double r;
  double theta;
} PPos;

typedef struct {
  int x;
  int y;

  void scaleMap() {
    x = map(x, 0, 150, 150, 30);
    y = map(y, 0, 150, 150, 30);
  }

  PPos toPolar() {
    return {
      .r = sqrt(sq((double)x) + sq((double)y)),
      .theta = atan((double)x / (double)y)
    };
  }
} CPos;

AccelStepper shoulder(AccelStepper::FULL4WIRE, 3, 5, 4, 6);
AccelStepper elbow(AccelStepper::FULL4WIRE, 7, 9, 8, 10);

MultiStepper Arm;

int radToSteps(double radians) {
  return (radians / (2 * PI)) * STEPS;
}

void RunSteppers() {

  while (Arm.run()) {
    delay(10);
    // Serial.println("running Steppers");
  }
}

void goTo(CPos pos) {
  pos.scaleMap();
  PPos ppos = pos.toPolar();

  double elbowAngle = acos((sq(shoulderToElbow) + sq(elbowToPen) - sq(ppos.r)) / (sq(shoulderToElbow) + sq(elbowToPen)));
  double internalShoulderAngle = acos(sq(ppos.r) / ((elbowToPen + shoulderToElbow) * ppos.r));

  double shoulderAngle = ppos.theta - internalShoulderAngle + PI / 2;
  long steps[2] = { radToSteps(shoulderAngle), radToSteps(elbowAngle) };
  Arm.moveTo(steps);
  RunSteppers();
}

void drawLine(CPos previous, CPos pos) {
  int rise = pos.y - previous.y;
  int run = pos.x - previous.x;
  float gradient = rise / run;
  goTo(previous);
  if (rise > run) {
    for (int i = 0; i < rise; i++) {
      CPos point = {
        .x = (int)(i / gradient) + previous.x,
        .y = i + previous.y
      };
      goTo(point);
    }
  } else if (run > rise) {
    for (int i = 0; i < run; i++) {
      CPos point = {
        .x = i + previous.x,
        .y = (int)(i * gradient) + previous.y
      };

      goTo(point);
    }
  }
}

void setup() {

  Serial.begin(9600);
  shoulder.setMaxSpeed(80);
  elbow.setMaxSpeed(80);
  Arm.addStepper(shoulder);
  Arm.addStepper(elbow);
  // long steps [2] = {200,400};
  // Arm.moveTo(steps);
  // Serial.println("shoulder speed: " + String(shoulder.speed()));
  // Serial.println("elbow speed: " + String(elbow.speed()));
  // RunSteppers();
  // Serial.println("switching directions");
  // steps[0] = -0;
  // steps[1] = -0;
  // Arm.moveTo(steps);
  // Serial.println("shoulder speed: " + String(shoulder.speed()));
  // Serial.println("elbow speed: " + String(elbow.speed()));
  // RunSteppers();

  //Serial.println("beginning square");
  // drawLine({.x = 0, .y= 10},{.x =100, .y = 110});
  goTo({ .x = 0, .y = 0 });
  goTo({ .x = 20, .y = 20 });
}

void loop() {
  // put your main code here, to run repeatedly:
}
