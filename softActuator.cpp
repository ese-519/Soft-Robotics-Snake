
#include "softActuator.h"

#include "countdown.h"

Serial pc(USBTX, USBRX);

// Actuators
SoftActuator actuator[NUM_ACTUATORS];

//DigitalOut mosfet_pin[6] = {};

// Pressure transducer
AnalogIn pressureTransducer(p20);

int previousActuatedIndex = -1;

void init_actuators()
{
     // Initialize the baud rate of uart for console
    pc.baud(115200);
    
    pc.printf("Initializing actuators\n");
    
//     Initialize actuator 1
    actuator[0].max_inflate_time_ms = 3000;
    actuator[0].mosfet_gate_pin = new DigitalOut(p8);
    actuator[0].pot_pin = new AnalogIn(p15);
    actuator[0].actuation_time_ms = actuator[0].max_inflate_time_ms / 9;
   
    // Initialize actuator 2
    actuator[1].max_inflate_time_ms = 3000;
    actuator[1].mosfet_gate_pin = new DigitalOut(p9);
    actuator[1].pot_pin = new AnalogIn(p16);
    actuator[1].actuation_time_ms = actuator[1].max_inflate_time_ms / 9;
    
    // Initialize actuator 3
    actuator[2].max_inflate_time_ms = 3000;
    actuator[2].mosfet_gate_pin = new DigitalOut(p10);
    actuator[2].pot_pin = new AnalogIn(p17);
    actuator[2].actuation_time_ms = actuator[2].max_inflate_time_ms / 9;
    
    // Initialize actuator 4
    actuator[3].max_inflate_time_ms = 3000;
    actuator[3].mosfet_gate_pin = new DigitalOut(p11);
    actuator[3].pot_pin = new AnalogIn(p18);
    actuator[3].actuation_time_ms = actuator[3].max_inflate_time_ms / 9;
   /* 
    // Initialize actuator 5
    actuator[4].max_inflate_time_ms = 3000;
    actuator[4].mosfet_gate_pin = new DigitalOut(p12);
    actuator[4].pot_pin = new AnalogIn(p19);
    actuator[4].actuation_time_ms = actuator[4].max_inflate_time_ms / 9;
    
    // Initialize actuator 6
    actuator[5].max_inflate_time_ms = 3000;
    actuator[5].mosfet_gate_pin = new DigitalOut(p13);
    actuator[5].pot_pin = new AnalogIn(p20);
    actuator[5].actuation_time_ms = actuator[5].max_inflate_time_ms / 9;
*/
    // Initialize common parameters of all the actuators
    for(int i = 0; i < NUM_ACTUATORS; i++)
    {
        actuator[i].time_actuated_ms = 0;
        actuator[i].mosfet_state = 1;
        actuator[i].pot_value = 0.0;
        *(actuator[i].mosfet_gate_pin) = actuator[i].mosfet_state;
        actuator[i].max_pressure_inflate_time_ms = 1000;
        actuator[i].pressure_actuation_time_ms = actuator[i].max_pressure_inflate_time_ms / 3;
        actuator[i].actuation_level = HIGH;
    }
    pc.printf("Actuators initialized\n");
}

void actuate(int actuator_index)
{
    actuator[actuator_index].mosfet_state = 0;
    *(actuator[actuator_index].mosfet_gate_pin) = 0;
 
 #ifdef TEST_ACTUATION  
    // Put off the mosfet of previously actuated actuator
    if (previousActuatedIndex != -1)
    {
        printf("switched off %d\n", previousActuatedIndex + 1);
        actuator[previousActuatedIndex].mosfet_state = 1;
        *(actuator[previousActuatedIndex].mosfet_gate_pin) = 1;
    }
#endif
}

void deactuate(int actuator_index)
{
    actuator[actuator_index].mosfet_state = 1;
    *(actuator[actuator_index].mosfet_gate_pin) = 1;
 }


void test_actuate()
{
    pc.printf("Test actuate\n");
    Countdown timer;
    while(1)
    {
        // Loop for actuation
        for(int i = 0; i < NUM_ACTUATORS; i++)
        {
            pc.printf("Actuating %d\n", i + 1);
            actuate(i);
            previousActuatedIndex = i;
            
            setupCommonMosfets(0);      // Actuate state as a parameter
            
            // Timer for 3 seconds
            timer.countdown_ms(3000);
            while(!timer.expired())
            ;
        } 
        
        // Loop for de-actuation
        for(int i = 0; i < NUM_ACTUATORS; i++)
        {
            pc.printf("De-actuating %d\n", i + 1);
            actuate(i);
            previousActuatedIndex = i;
            
            setupCommonMosfets(1);      // Deactuate state as a parameter
            
            // Timer for 3 seconds
            timer.countdown_ms(3000);
            while(!timer.expired())
            ;
        }
    } 
}


void manual_actuate()
{
     pc.printf("Manual Actuation\n");
    int timer_value_ms;
    float pot_value;
    Countdown timer;
    int i;
    bool actuation_type = 1;    // pump-in(1) or pump-out(0)
       
    while(1)
    {
        for(i = 0; i < NUM_ACTUATORS; i++)
        {
            // Read pot value
            pot_value = get_pot_value(*(actuator[i].pot_pin));
            pot_value = (int)(pot_value * 10 + 0.5);     // Only care for 1 digit after decimal
            pot_value = (float)pot_value / 10;
            //pc.printf("pot value: %.1f\n", pot_value);
            wait(0.01);
            
            if (pot_value == actuator[i].pot_value)     // 
                continue;
                
            pc.printf("pot value changed: current: %.1f old: %.1f\n", pot_value, actuator[i].pot_value);
            
            if (pot_value < actuator[i].pot_value)
                actuation_type = 0;
            else
                actuation_type = 1;
            actuator[i].pot_value = pot_value;
          
            // Change the MOSFET state
            // Get the time for which MOSFET state needs to be changed
            timer_value_ms = actuator[i].actuation_time_ms;
            
            actuate(i);
            
            if (actuation_type == 1)
                setupCommonMosfets(0);   
            else
                setupCommonMosfets(1);
            
            // Start the timer
            pc.printf("Actuating for time: %d ms\n", timer_value_ms);
            timer.countdown_ms(timer_value_ms);
            
            while(!timer.expired());
             
            // Switch back the mosfet state to off 
            deactuate(i);
            
            if (actuation_type == 1)
                setupCommonMosfets(1);   
            else
                setupCommonMosfets(0);
            
            if (actuation_type == 1 && actuator[i].time_actuated_ms < (actuator[i].max_inflate_time_ms - actuator[i].actuation_time_ms))
                actuator[i].time_actuated_ms += timer_value_ms;
            if (actuation_type == 0 && actuator[i].time_actuated_ms >= actuator[i].actuation_time_ms)
                actuator[i].time_actuated_ms -= timer_value_ms;
                
            pc.printf("Time actuated : %d ms\n", actuator[i].time_actuated_ms);
            
        }
        i = 0;
    }
}


void inverse_kinematics_actuate()
{
    pc.printf("Inverse kinematics\n");
    
    int timer_value_ms;
    float pressure_value;
    Countdown timer;
    int i;
    bool actuation_type = 1;    // pump-in(1) or pump-out(0)
       
    //while(1)
    {
        for(i = 0; i < NUM_ACTUATORS; i++)
        {
            // Read pot value
            pressure_value = pressureTransducer.read();
            pressure_value = (int)(pressure_value * 10 + 0.5);     // Only care for 1 digit after decimal
            pressure_value = (float)pressure_value / 10;
            //pc.printf("pot value: %.1f\n", pot_value);
            wait(0.01);
            
            if (pressure_value == actuator[i].pressure_value)     // 
                continue;
                
            pc.printf("pressure value changed: current: %.1f old: %.1f\n", pressure_value, actuator[i].pressure_value);
            
            if (pressure_value < actuator[i].pressure_value)
                actuation_type = 0;
            else
                actuation_type = 1;
            actuator[i].pressure_value = pressure_value;
          
            // Change the MOSFET state
            // Get the time for which MOSFET state needs to be changed
            timer_value_ms = actuator[i].actuation_time_ms;
            
            actuate(i);
            
            if (actuation_type == 1)
                setupCommonMosfets(0);   
            else
                setupCommonMosfets(1);
            
            // Start the timer
            pc.printf("Actuating for time: %d ms\n", timer_value_ms);
            timer.countdown_ms(timer_value_ms);
            
            while(!timer.expired());
             
            // Switch back the mosfet state to off 
            deactuate(i);
            
            if (actuation_type == 1)
                setupCommonMosfets(1);   
            else
                setupCommonMosfets(0);
            
            if (actuation_type == 1 && actuator[i].time_actuated_ms < (actuator[i].max_inflate_time_ms - actuator[i].actuation_time_ms))
                actuator[i].time_actuated_ms += timer_value_ms;
            if (actuation_type == 0 && actuator[i].time_actuated_ms >= actuator[i].actuation_time_ms)
                actuator[i].time_actuated_ms -= timer_value_ms;
                
            pc.printf("Time actuated : %d ms\n", actuator[i].time_actuated_ms);
           
        }
        
        i = 0;
        
    }
}