#include "mbed.h"

#include <math.h>

#include "countdown.h"
#include "softActuator.h"

#define TEST_ACTUATION      1
//#define MANUAL_ACTUATION    1
//#define INVERSE_KINEMATICS  1

extern SoftActuator actuator[NUM_ACTUATORS];

int main(void)
{   
    // Initialize the soft actuators
    init_actuators();
    
    // Initialize master, in and out mosfets
    init_common_mosfets();

#ifdef TEST_ACTUATION    
    // Actuate the actuators to their maximum limit, separated by 2 seconds gap and then deflate them
    //pc.printf("Test Actuation\n");
    test_actuate();
    
#elif MANUAL_ACTUATION
    // Actuate using pot readings
    manual_actuate();
    
#else
    // Actuate using pressure sensor readings
    // pc.printf("Reverse kinematics Actuation\n");
    inverse_kinematics_actuate();
#endif 
  
    return 0;
}