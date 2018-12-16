
#ifndef MOSFET_H
#define MOSFET

#include "mbed.h"

void init_common_mosfets();
void set_mosfet_state(DigitalOut pin, bool state);
void setupCommonMosfets(bool actuationState);       // 0 - actuate, 1 - deactuate
#endif