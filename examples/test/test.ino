/*
 * Project VDW_Stepper
 * Description:
 * Author:
 * Date:
 */

#include "VDW_Stepper.h"

VDW_Stepper Stepper1;
VDW_Stepper Stepper2;
VDW_Stepper Stepper3;
VDW_Stepper Stepper4;
VDW_Stepper Stepper5;


void clockwiseStep(){
  int one = 2;
}
void counterClockwiseStep(){
  int two = 1;
}
void enableStepper(){
  int three = 4;
}

// setup() runs once, when the device is first turned on.
void setup() {
  // Put initialization like pinMode and begin functions here.
  Serial.begin(230400);

  delay(1000);

  Stepper1.init(clockwiseStep, counterClockwiseStep, enableStepper);
  Stepper2.init(clockwiseStep, counterClockwiseStep, enableStepper);
  Stepper3.init(clockwiseStep, counterClockwiseStep, enableStepper);
  Stepper4.init(clockwiseStep, counterClockwiseStep, enableStepper);
  Stepper5.init(clockwiseStep, counterClockwiseStep, enableStepper);

  Stepper1.runSpeed(200);
  Stepper2.runSpeed(300);
}

// loop() runs over and over again, as quickly as it can execute.
void loop() {
  // The core of your code will likely live here.
  static uint32_t lastPrint = 0;
  if(millis() - lastPrint > 1000){
    VDW_Stepper::printSteppers();
    lastPrint = millis();
  }
}