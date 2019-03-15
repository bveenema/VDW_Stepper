#include "VDW_Stepper.h"

// Initialize Static Members
StepperPtr VDW_Stepper::head = nullptr;
IntervalTimer VDW_Stepper::Step_Timer;
int VDW_Stepper::lastDuration = 0;
bool VDW_Stepper::ISR_Enabled = false;

void VDW_Stepper::printSteppers(){
	Serial.printlnf("------- Steppers -------");
	StepperPtr cStepper = VDW_Stepper::head;
	int index = 1;
	while(cStepper != nullptr){
		Serial.printlnf("  Stepper %d: %d",index++,cStepper);
		cStepper = cStepper->next;
	}
	Serial.printlnf("------------------------");
}

// RUN ISR
void VDW_Stepper::Run_ISR(){
	// Check if ISR was disabled
	if(VDW_Stepper::ISR_Enabled == false) VDW_Stepper::lastDuration = 0;
	

	// Record time ISR start
	uint32_t timeISRStarted = CPU_Ticks();

	// Get the list
	StepperPtr cStepper = VDW_Stepper::head;

	// Cycle through the list
	int index = 0;
	int nextDuration = 0x7FFFFFFF; // largest signed int
	while(cStepper != nullptr){
		if(cStepper->_stepTime > 0){ // Only operate on "active" steppers
			// Subtract lastDuration from _stepTime
			cStepper->_stepTime -= VDW_Stepper::lastDuration;

			// Call a step if due
			if(cStepper->_stepTime <= MIN_TIME_BETWEEN_RUN_ISR){
				(cStepper->_direction) ? cStepper->_clockwise() : cStepper->_counterClockwise();
				cStepper->_stepTime = cStepper->computeNewSpeed();
				Serial.printlnf("Step Stepper %d, next: %d",index, cStepper->_stepTime);
			}

			// Determine the next time Run_ISR should fire
			if(cStepper->_stepTime < nextDuration) nextDuration = cStepper->_stepTime;
		}
		cStepper = cStepper->next;
		index += 1;
	}

	// Record Time ISR End
	uint32_t timeISREnded = CPU_Ticks();

	// Remove ISR duration from _stepTime and nextDuration
	uint32_t ISR_Duration = ((timeISREnded - timeISRStarted) / CPU_TICKS_PER_MICROSECOND()) + 1; // add 1 microsecond for time to 
	cStepper = VDW_Stepper::head;
	Serial.printlnf("ISR Duration: %d", ISR_Duration);
	while(cStepper != nullptr){
		if(cStepper->_stepTime > 0 ) cStepper->_stepTime -= ISR_Duration;
		cStepper = cStepper->next;
	}
	nextDuration -= ISR_Duration;

	// Setup for next Run_ISR
	VDW_Stepper::lastDuration = nextDuration;
	Serial.printlnf("Next ISR: %d", nextDuration);
	if(nextDuration < MIN_TIME_BETWEEN_RUN_ISR) nextDuration = MIN_TIME_BETWEEN_RUN_ISR;
	if(nextDuration == 0x7FFFFFFF){
		VDW_Stepper::ISR_Enabled = false;
		VDW_Stepper::Step_Timer.end();
	}else if(VDW_Stepper::ISR_Enabled == false){
		Serial.println("Start the ISR");
		VDW_Stepper::ISR_Enabled = true;
		VDW_Stepper::Step_Timer.begin(VDW_Stepper::Run_ISR, 655, uSec);
	}else{
		VDW_Stepper::Step_Timer.resetPeriod_SIT(nextDuration, uSec);
	}
	
}