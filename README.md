# VDW_Stepper

A full-featured stepper motor library, implementing accelerations, and position tracking.  Multiple motors can be run simultaneously utilizing a single interrupt (no polling).

## Usage

Connect a stepper driver, add the VDW_Stepper library to your project and follow this simple example:

```
#include "VDW_Stepper.h"

void clockwiseStep(){
  // code to make your stepper motor move one step clockwise
}
void counterClockwiseStep(){
  // code to make your stepper motor move one step counter-clockwise
}
void enableStepper(){
  // code to enable your stepper motor
}

VDW_Stepper myStepper;

void setup() {
  myStepper.init(clockWiseStep, counterClockwiseStep, enableStepper);
  myStepper.run(200*1000); // run stepper motor at 200 steps/sec. VDW_Stepper uses milli-steps/Sec for speed
}

void loop() {
}
```

See the [examples](examples) folder for more details.

##### Note: VDW_Stepper uses milli-steps/Sec(mSteps/sec) for speed and mSteps/sec^2 for acceleration. Position is in steps. [See note below](Why the weird units)

## Documentation

### API

###### Setup
`VDW_Stepper()` - Create the stepper object
`void init(void (*clockwise)(), void(*counterClockwise)(), [void(*enable)()], ]void(*disable)()])` - Provides the motor with functions to call when steps or enables/disables are needed

###### Movers
`void run([uint32_t speed], [bool constantSpeed], [uint32_t accel])` - Move the motor indefinitely at the last set or specified speed. Using any of the optional parameters does NOT overide the speed, acceleration or mode settings
`void moveAbsolute(int32_t position, [uint32_t speed], [bool constantSpeed], [uint32_t accel])` - Move the motor to a new target position. Using any of the optional parameters does NOT overide the speed, acceleration or mode settings.
`void moveRelative(int32_t distance, [uint32_t speed], [bool constantSpeed], [uint32_t accel])` - Move the motor to a distance relative to the current position. Updates the target position and keeps current position accurate. Using any of the optional parameters does NOT overide the speed, acceleration or mode settings.
`void stop()` - Stops the motor by setting a new target position
`void pause()` - Stops the motor but maintains the target postion. Motor can be started again with resume()
`void eStop()` - Stops the motor immediately regardless of mode
`void disable()` - Disables the stepper motor by calling the disable function provided in init
`void enable()` - Enables the stepper motor by calling the enable function provided in init. Not necessary to call before move functions. Move functions will call automatically. Only needed if the stepper motors are disabled outside of the library.

###### Setters
`void setMaxSpeed(float speed)` - Set the maximum permitted speed. Does NOT set the current/target speed. Used for setting safety limits. Speeds are further limited by Ultimate Max and Min Speeds, which are processor limits
`void setAcceleration(float acceleration)` - sets the acceleration rate
`void setSpeed(float speed)` - Set the target speed
`void setMode(bool constantSpeed)` - Set the mode, constant speed or Accel/Decel
`void setCurrentPosition(long position)` - Sets the current position of the motor

###### Getters
`uint32_t getMaxSpeed()` - Returns the max speed
`uint32_t getTargetSpeed()` - the most recently set speed
`uint32_t getCurrentSpeed()` - the current speed of the motor
`long distanceToGo() `- the distance from the current to target position
`int32_t targetPosition()` - returns the target position
`int32_t currentPosition()` - returns the current position
`bool isRunning()` - Checks to see if the motor is currently running to a target


### PWM Warning
`VDW_Stepper` uses a hardware timer. Different timers can be allocated and [SparkIntervalTimer](https://github.com/pkourany/SparkIntervalTimer), the library used for allocating timers, is smart enough to  use timers that have not been otherwise allocated. Care should be taken to ensure a hardware timer is available and PWM function is not needed. See table below for timer information of Particle Core and Photon
CORE:

```
PIN		TMR2	TMR3	TMR4
----------------------------
D0						 x
D1						 x
A0		 x
A1		 x
A4				 x
A5				 x
A6				 x
A7 				 x

PHOTON:
PIN		TMR3	TMR4	TMR5	TMR6	TMR7
--------------------------------------------
D0			  	 x
D1			  	 x
D2		 x
D3		 x
A4		 x
A5		 x
WKP					     x
```

### Interrupt Sharing

`VDW_Stepper` shares a single interrupt for multiple motors.  To minimize the compute cycles of the interrupt, a wrapper class is built around `SparkIntervalTimer` that contains helper data preventing unnecessary function calls

```cpp
struct TimerData{
	bool nextToRun[255];
	uint16_t lastDuration;
	uint8_t numSteppers;
};
```
An example of how 2 motors might share the interrupt timer:

![](ReadmeAssets/InterruptSharing.jpg)

## Why the weird units


## Why a new Stepper Library

### The problem

Other stepper libraries require you to poll a `run()` or `step()` function to move the motor.  At low motor speeds or in programs where other processes are limited, this works okay, but you end up calling the `run()` or `step()` function much more frequently than required.

At higher motor speeds, it becomes necessary to call the `run()` or `step()` function from a timer-based interrupt, however, in order to accommodate a range of motor speeds, a very short timer duration is required, often 20 us or less. This again leads to calling the `run()` or `step()` function far more than necessary. For example, for a **4000 step/sec** motor (quick but not fast), a step only needs to be issued every **250 us**. If we pick a **20 us** update interval, the stepper will be called **10 us** late every other step, a **4% error!** or as if it was running at **3846 steps/sec**. Because most motors vary in speed, it is difficult if not impossible to pick the ideal update interval.

### The Solution

What if the stepper library new how when the next step was required and called the `run()` function on it's own? Stepper libraries that have speed control already calculate when the next step is due. All it needs to do is set a timer-interrupt to fire right when it's needed.  That means for any given `step/sec` target speed, the controller can send it's pulse on-time to within a micro-second! With the previous example of a **4000 step/sec** motor, the error is reduced to **.4%** in the worst case scenario AND the update function is only called when needed.  In the previous example, with a **20 us** update rate the `run()` function was called **13 times** for one step!

Enter VDW_Stepper.  `VDW_Stepper` leverages the powerful `SparkIntervalTimer` library to manage timer-interrupt calls and sets the interval according to every steps needs.  Multiple stepper motors can even be run at the same time, using the same interrupt!

## Contributing

Here's how you can make changes to this library and eventually contribute those changes back.

To get started, [clone the library from GitHub to your local machine](https://help.github.com/articles/cloning-a-repository/).

Change the name of the library in `library.properties` to something different. You can add your name at then end.

Modify the sources in <src> and <examples> with the new behavior.

To compile an example, use `particle compile examples/usage` command in [Particle CLI](https://docs.particle.io/guide/tools-and-features/cli#update-your-device-remotely) or use our [Desktop IDE](https://docs.particle.io/guide/tools-and-features/dev/#compiling-code).

After your changes are done you can upload them with `particle library upload` or `Upload` command in the IDE. This will create a private (only visible by you) library that you can use in other projects. Do `particle library add VDW_Stepper_myname` to add the library to a project on your machine or add the VDW_Stepper_myname library to a project on the Web IDE or Desktop IDE.

At this point, you can create a [GitHub pull request](https://help.github.com/articles/about-pull-requests/) with your changes to the original library. 

If you wish to make your library public, use `particle library publish` or `Publish` command.

## LICENSE
Copyright 2018 Ben Veenema

Licensed under the MIT license
