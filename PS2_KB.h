#include "pico/stdlib.h"
#include "pico/platform.h"
#include "Arduino.h"
#include "globals.h"
#include "pico/stdlib.h"
#include "pico/platform.h"
#include "Arduino.h"

#ifndef PS2_KB_H
#define PS2_KB_H

void kb_ISR();

struct key_event returnKBEvent(bool caps, bool shift, bool extChar, bool release, uint8_t key_code);

#endif