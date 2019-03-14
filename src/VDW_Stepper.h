#ifndef VDW_STEPPER_H
#define VDW_STEPPER_H

#include "SparkIntervalTimer.h"

class VDW_Stepper;
typedef VDW_Stepper* StepperPtr;


// This is your main class that users will import into their application
class VDW_Stepper
{
public:

  VDW_Stepper(){
    if(Head == nullptr){
      Head = this;
    } else {
      StepperPtr cStepper = Head;
      while(cStepper->next != nullptr){
        cStepper = cStepper->next;
      }
      cStepper->next = this;
    }
  }

  static void printSteppers(){
    Serial.printlnf("---- Steppers ----");
    StepperPtr cStepper = VDW_Stepper::Head;
    int index = 1;
    while(cStepper != nullptr){
      Serial.printlnf("Stepper %d: %d",index++,cStepper);
      cStepper = cStepper->next;
    }
    Serial.printlnf("------------------");
  }

private:
  StepperPtr next = nullptr;

  // Shared Members
  static StepperPtr Head;
  // static IntervalTimer Step_Timer;
  // static void timer_ISR();

};

#endif