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
  // Runs the stepper at a constant speed (no accel/decel)
  // \param[in32_t] speed - the milli-steps/sec of the motor. Negative == CCW, Positive == CW
  //
  void runSpeed(int32_t speed);

  // printSteppers
  // Prints a list of all the stepper pointers
  static void printSteppers();

private:
  // STEP FUNCTIONS
  void (*_clockwise)();
  void (*_counterClockwise)();

  // ENABLE FUNCTION
  void (*_enableStepper)();

  // STEP DATA
  bool _direction = false; // current direction the motor is spinning, 1 == CW
  int32_t _speed = 0; // the current speed (milli-steps/sec). Negative == CCW, Positive == CW
  int32_t _safeSpeed = 0; // the maximum safe speed a motor should every be run. 0 == No Max
  int32_t _stepInterval = 0; // the time between the steps (value < 1 means no steps)
  bool _constantSpeed = false; // True if the stepper is running at constant speed (no accel/decel)

  // STEP TIMING
  static int lastDuration; // amount of time between Run_ISR() calls
  int _stepTime = 0; // amount of time until the next step (value < 1 means no step due)

  // STEPPER LIST MEMBERS
  StepperPtr next = nullptr; // pointer to next stepper
  static StepperPtr head; // the first stepper in the list

  ///RUN ISR MEMBERS
  static IntervalTimer Step_Timer;
  static void Run_ISR();
  static bool ISR_Enabled;

  // Compute New Speed
  // Calculates the next _stepInterval. Implements accel/decel and position tracking if not
  // in Constant Spee Mode
  // \return[int32_t] the next step interval (u-sec)
  int32_t computeNewSpeed();
};

#endif