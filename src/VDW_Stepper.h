#ifndef VDW_STEPPER_H
#define VDW_STEPPER_H

// Hardware/Platform Abstractions
#define PARTICLE
#if defined(PARTICLE)
  #include "Particle.h"
  #include "SparkIntervalTimer.h"
  #define CPU_Ticks() (System.ticks())
  #define CPU_TICKS_PER_MICROSECOND() (System.ticksPerMicrosecond())
  #define Constrain(value, min, max) (constrain(value, min, max))
#endif

#define MIN_TIME_BETWEEN_RUN_ISR 2
#define ULTIMATE_MIN_SPEED 31 // milli-steps/sec
#define ULTIMATE_MAX_SPEED 100000000 // milli-steps/sec

inline uint32_t milliStepsToUsecInterval(int32_t milliSteps){
  return abs(1000000000/milliSteps);
}

class VDW_Stepper;
typedef VDW_Stepper* StepperPtr;

enum{
  Stepper_ConstantSpeed,
  Stepper_Accelerations,
};


// This is your main class that users will import into their application
class VDW_Stepper
{
public:

  // CONSTRUCTOR
  // Adds the stepper to the list
  VDW_Stepper();

  // Init
  // Provides the stepper with functions to call when a clockwise or counterClockwise step is called
  // \param[void func(void)] clockwise - the clockwise step function
  // \param[void func(void)] counterClockwise - the counterClockwise step function
  // \param[void func(void)] enable - the enable function for the stepper [optional]
  // \parat[void func(void)] disable - the disable function for the stepper [optional]
  void init(void (*clockwise)(), void(*counterClockwise)(), void(*enable)()=nullptr, void(*disable)()=nullptr);

  // Run Speed
  // Move the motor indefinitely with the last set or provided settings
  // Using any of the optional parameters does NOT overide the speed, acceleration or mode settings
  // \param[i32] speed - the speed of the motor will turn (mSteps/sec). Negative == CCW, Positive == CW [optional]
  // \param[u32] acceleration - the acceleration for the motor. (mSteps/sec^2) [optional]
  // \param[bool] mode - the mode of the stepper 0 == Constant Speed, 1 == Accelerations
  void run(int32_t speed=0, bool mode=0, uint32_t acceleration=0);

  // Move Absolute
  // Move the motor to a new target position.
  // Using any of the optional parameters does NOT overide the speed, acceleration or mode settings.
  // \param[i32] position - the new target position (steps). Negative == CCW, Positive == CW
  // \param[i32] speed - the speed of the motor will turn (mSteps/sec). Negative == CCW, Positive == CW [optional]
  // \param[u32] acceleration - the acceleration for the motor. (mSteps/sec^2) [optional]
  // \param[bool] mode - the mode of the stepper 0 == Constant Speed, 1 == Accelerations
  void moveAbsolute(int32_t position, [uint32_t speed], [bool constantSpeed], [uint32_t accel]);

  // Move Relative
  // Move the motor to a distance relative to the current position.
  // Updates the target position and keeps current position accurate.
  // Using any of the optional parameters does NOT overide the speed, acceleration or mode settings.
  // \param[i32] position - the new target position (steps). Negative == CCW, Positive == CW
  // \param[i32] speed - the speed of the motor will turn (mSteps/sec). Negative == CCW, Positive == CW [optional]
  // \param[u32] acceleration - the acceleration for the motor. (mSteps/sec^2) [optional]
  // \param[bool] mode - the mode of the stepper 0 == Constant Speed, 1 == Accelerations
  void moveRelative(int32_t distance, [uint32_t speed], [bool constantSpeed], [uint32_t accel]);

  // Stop
  // Stops the motor by setting a new target postion. 
  // If constant speed (runSpeed), the motor is stopped immediately.
  // If accel/decel are use (run) the deceleration is used to safely stop the motor.
  void stop();

  // Pause
  // Stops the motor but maintains the target position. Motor can be started again with resume()
  // If constant speed (runSpeed), the motor is stopped immediately.
  // if accel/decel (run), the deceleration is used to safely stop the motor
  void pause();

  // Emergency Stop
  // Stops the motor immediately regardless of constant speed (runSpeed) or accel/decel (run)
  // Clears position and target position
  // if a value for disableTimeout greater than 0 is passed, the disable stepper function will be called after that amount of time (milli-seconds)
  void eStop();

  // Disable
  // Disables the stepper motor by calling the disable function passed in init
  // if motor is currently running. stop() is called prior to disable
  void disable();

  // Enable
  // Enables the stepper motor by calling the enable function provided in init.
  // Not necessary to call before move functions. Move functions will call automatically.
  // Only needed if the stepper motors are disabled outside of the library.
  void enable();

  // printSteppers
  // Prints a list of all the stepper pointers
  static void printSteppers();

private:
  // STEPPER MOTOR FUNCTIONS
  void (*_clockwise)();
  void (*_counterClockwise)();
  void (*_enableStepper)();
  void (*_disableStepper)();

  // STEP DATA
  volatile bool _direction = false; // current direction the motor is spinning, 1 == CW
  volatile int32_t _speed = 0; // the current speed (milli-steps/sec). Negative == CCW, Positive == CW
  volatile int32_t _safeSpeed = 0; // the maximum safe speed a motor should every be run. 0 == No Max
  volatile int32_t _stepInterval = 0; // the time between the steps (value < 1 means no steps)
  bool _constantSpeed = false; // True if the stepper is running at constant speed (no accel/decel)
  int32_t _position = 0; // The current position of the motor in steps. Negative == CCW, Positive == CW
  int32_t _target = 0; // The position the motor is moving to in steps. Negative == CCW, Positive = CW

  // STEP TIMING
  volatile static int lastDuration; // amount of time between Run_ISR() calls
  volatile int _stepTime = 0; // amount of time until the next step (value < 1 means no step due)

  // STEPPER LIST MEMBERS
  StepperPtr next = nullptr; // pointer to next stepper
  static StepperPtr head; // the first stepper in the list

  ///RUN ISR MEMBERS
  static IntervalTimer Step_Timer;
  static void Run_ISR();
  static bool ISR_Enabled;

  // Compute New Speed
  // Calculates the next _stepInterval. Implements accel/decel and position tracking if not
  // in Constant Speed Mode
  // \return[int32_t] the next step interval (u-sec)
  int32_t computeNewSpeed();
};

#endif