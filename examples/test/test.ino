/*
 * Project VDW_Stepper
 * Description:
 * Author:
 * Date:
 */

#include "VDW_Stepper.h"

VDW_Stepper Stepper0;
VDW_Stepper Stepper1;
VDW_Stepper Stepper2;
VDW_Stepper Stepper3;
VDW_Stepper Stepper4;

SYSTEM_MODE(SEMI_AUTOMATIC);

uint32_t cwCounter[5];
uint32_t ccwCounter[5];
uint32_t enableCounter[5];
uint32_t disableCounter[5];

// setup() runs once, when the device is first turned on.
void setup() {
  // Put initialization like pinMode and begin functions here.
  Serial.begin(230400);

  delay(5000);

  Stepper0.init([](){ cwCounter[0] += 1;}, [](){ ccwCounter[0] += 1;}, [](){ enableCounter[0] += 1;});
  Stepper1.init([](){ cwCounter[1] += 1;}, [](){ ccwCounter[1] += 1;}, [](){ enableCounter[1] += 1;});
  Stepper2.init([](){ cwCounter[2] += 1;}, [](){ ccwCounter[2] += 1;}, [](){ enableCounter[2] += 1;});
  Stepper3.init([](){ cwCounter[3] += 1;}, [](){ ccwCounter[3] += 1;}, [](){ enableCounter[3] += 1;});
  Stepper4.init([](){ cwCounter[4] += 1;}, [](){ ccwCounter[4] += 1;}, [](){ enableCounter[4] += 1;});

}

// loop() runs over and over again, as quickly as it can execute.
void loop() {

  // ---------------------------------------------
  // VDW_Stepper Tests
  // ---------------------------------------------
  static uint32_t testNumber = 1;

  switch(testNumber){
    case 1: {
      static bool runTest = false;
      static bool printTestTitle = false;
      static bool setupTest = false;
      static uint32_t testTimer = millis();
      if(!printTestTitle){
        Serial.printlnf("\n****************\nTest %d: Two motors should turn CW every second\n****************\n",testNumber);
        printTestTitle = true;
      }
      if(!setupTest) {
        // Clear the testing state
        ResetTestingState();

        // Set Stepper0 to run every second
        Stepper0.run(ConstantSpeed, 1000);

        // Set Stepper1 to run every second
        Stepper1.setSpeed(1000);
        Stepper1.setMode(ConstantSpeed);
        Stepper1.run();
        
        setupTest = true;
      }
      if(!runTest) {
        if( cwCounter[0] >= 4  && enableCounter[0] > 0){
          Serial.printlnf("Success, %d", cwCounter[0]);
          runTest = true;
          testNumber++;
        } else if(millis() - testTimer > 5000){
          Serial.printlnf("Test Fail, %d", cwCounter[0]);
          runTest = true;
          testNumber++;
        }
        delay(100);
      }
      break;
    }
    default:{
      static bool AllTestsComplete = false;
      if(!AllTestsComplete){
        Serial.println("\nAll Test Complete");
        AllTestsComplete = true;
        System.dfu();
      }
      break;
    }
      
  }
}

void ResetTestingState(){
  for(uint8_t i=0; i<5; i++){
    cwCounter[i] = 0;
    ccwCounter[i] = 0;
    enableCounter[i] = 0;
    disableCounter[i] = 0;
  }
}