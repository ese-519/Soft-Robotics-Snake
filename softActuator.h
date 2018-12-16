
#ifndef SOFT_SEGMENT_H
#define SOFT_SEGMENT_H

#include "mbed.h"

#include "mosfet.h"
#include "potentiometer.h"

#define NUM_ACTUATORS   4
 
typedef enum
{
    LOW,
    HIGH,
    MEDIUM
}Level;

typedef struct 
{
    int max_inflate_time_ms;
    int time_actuated_ms;
    int actuation_time_ms;
    int max_pressure_inflate_time_ms;
    int pressure_actuation_time_ms;
    DigitalOut* mosfet_gate_pin;
    bool mosfet_state;
    AnalogIn* pot_pin;
    float pot_value;
    float pressure_value;
    Level actuation_level;
    
}SoftActuator;

/* Initialize the actuators */
void init_actuators();

/* Actuate the actuators to their maximum limit, separated by 2 seconds gap and then deflate them */
void test_actuate();

/* Actuate using potentiometer readings */
void manual_actuate();

// Actuate using pressure sensor readings
void inverse_kinematics_actuate();
    
#endif 