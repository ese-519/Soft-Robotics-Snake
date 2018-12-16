
#include "potentiometer.h"

float get_pot_value(AnalogIn pot)
{ 
    return pot.read();
}
