#include "VDW_Stepper.h"

VDW_Stepper::VDW_Stepper(){
	if(VDW_Stepper::head == nullptr){
		VDW_Stepper::head = this;
	} else {
		StepperPtr cStepper = VDW_Stepper::head;
		while(cStepper->next != nullptr){
			cStepper = cStepper->next;
		}
		cStepper->next = this;
	}
}

void VDW_Stepper::init(void (*clockwise)(), void(*counterClockwise)(), void(*enable)(), void(*disable)){
	_clockwise = clockwise;
	_counterClockwise = counterClockwise;
	_enableStepper = enable;
  _disableStepper = disable;
}

int32_t VDW_Stepper::computeNewSpeed(){
    if(_constantSpeed){
      return _stepInterval;
    }else{
      return 0;
    }
  }

	void VDW_Stepper::runSpeed(int32_t speed){
    // Check for change
    if(_constantSpeed && speed == _speed) return;

    // Constrain speed if Safe Speed is set
    if(_safeSpeed > 0) speed = Constrain(speed, -_safeSpeed, _safeSpeed);

    // Set the direction
    _direction = (speed > 0) ? 1 : 0;

    // Calculate the ISR interval
    if(speed == 0){
      _stepInterval = 0;
    }
    _stepInterval = milliStepsToUsecInterval(speed);
		_stepTime = _stepInterval;

    // Enable the stepper
    if(_enableStepper) _enableStepper();

    // Set Stepper to Constant Speed Mode
    _constantSpeed = true;

    // Restart the ISR if required
    if(VDW_Stepper::ISR_Enabled == false) VDW_Stepper::Run_ISR();
  }