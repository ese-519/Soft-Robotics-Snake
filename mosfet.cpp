
#include "mosfet.h"

// Master and pump_in solenoid & pump_out solenoid mosfets
DigitalOut mosfet_master(p5);             // Drives all the solenoids connected to soft actuators
DigitalOut mosfet_in(p6), mosfet_out(p7);     // Connected to pump in and pump out

void set_mosfet_state(DigitalOut pin, bool state) 
{
    pin = state;
}

void init_common_mosfets()
{
    mosfet_out = 1;     // Connected to ambient
    mosfet_in = 1;      // Connected to ambient
    mosfet_master = 0;  // Connected to mosfet_out
}

void setupCommonMosfets(bool actuationState)
{
    if (actuationState == 1)    // De-actuate
    {
        mosfet_out = 0;
        mosfet_in = 1;
        mosfet_master = 1;
    }
    else                        // Actuate
    {
        mosfet_out = 1;
        mosfet_in = 0;
        mosfet_master = 0;   
    }
}