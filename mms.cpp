#include "mms.h"
#include "globals.h"

#include "pico/stdlib.h"
#include "pico/platform.h"
#include "Arduino.h"

uint8_t vga_data_array[TXCOUNT];
uint8_t * address_pointer = &vga_data_array[0] ;



