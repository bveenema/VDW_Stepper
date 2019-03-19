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

void VDW_Stepper::init(void (*clockwise)(), void(*counterClockwise)(), void(*enable)(), void(*disable)()){
	_clockwise = clockwise;
	_counterClockwise = counterClockwise;
	_enableStepper = enable;
  _disableStepper = disable;
}

int32_t VDW_Stepper::computeNewSpeed(){
  if(_mode == ConstantSpeed || _tempMode == ConstantSpeed){
    return _stepInterval;
  }else{
    return 0;
  }
}

void VDW_Stepper::clearTemps(){
  _tempMode = NoChange;
  _tempSpeed = 0;
  _tempAcceleration = 0;
}

void VDW_Stepper::run(Mode mode, int32_t speed, uint32_t acceleration){
  // Return if nothing is changing
  // if _stepTime > 0 and no settings are passed
  // if _stepTime > 0 and settings are same as current
  if(_stepTime > 0){
    if(mode==NoChange || mode==_mode
      && speed==0 || speed==_speed
      && acceleration==0 || acceleration==_acceleration){
        return;
      }
  }

  // Constrain speed if Safe Speed is set
  if(_safeSpeed > 0) speed = Constrain(speed, -_safeSpeed, _safeSpeed);
  
  // Assign any temporary settings
  _tempMode = mode; // will assign no change if nothing is passed
  _tempSpeed = speed; // will assign 0 if nothing is passed
  _tempAcceleration = acceleration; // will assign 0 if nothing is passed

  // Set the motor to run indefinitely
  _hasTarget = false;

  // CONSTANT SPEED MODE
  if(_tempMode == ConstantSpeed 
    || (_mode == ConstantSpeed && _tempMode != Accelerations)){
    
    // Get the speed
    int32_t newSpeed = (_tempSpeed) ? (_tempSpeed) : (_speed);

    // Set the direction
    _direction = (newSpeed > 0) ? 1 : 0;

    // Calculate the ISR interval
    if(newSpeed == 0)  _stepInterval = 0;
    else _stepInterval = milliStepsToUsecInterval((_tempSpeed) ? (_tempSpeed) : (_speed));
    _stepTime = _stepInterval;

    // Enable the stepper
    if(_enableStepper) _enableStepper();
  }

  // Restart the ISR if required
  if(VDW_Stepper::ISR_Enabled == false) 
    VDW_Stepper::Step_Timer.begin(VDW_Stepper::Run_ISR, 65535, hmSec); // duration does not matter, Run_ISR executes immediately which will change duration
}

void VDW_Stepper::stop(){
  if(_mode == ConstantSpeed || _tempMode == ConstantSpeed){ 
    _stepInterval = 0; // Stop immediately
    _target = 0; // Set target position to 0
    clearTemps(); // reset any temporary settings
  }else{
    return;
  }
}

void VDW_Stepper::pause(){
  if(_mode == ConstantSpeed || _tempMode == ConstantSpeed){ // Stop immediately if Constant Speed
    _stepInterval = 0;
  }else{
    return;
  }
}