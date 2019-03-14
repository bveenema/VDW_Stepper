/*
 * Project VDW_Stepper
 * Description:
 * Author:
 * Date:
 */

#include "VDW_Stepper.h"

SYSTEM_MODE(AUTOMATIC);

VDW_Stepper Stepper1;
VDW_Stepper Stepper2;

// setup() runs once, when the device is first turned on.
void setup() {
  // Put initialization like pinMode and begin functions here.

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