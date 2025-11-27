#include "draw_funcs.h"
#include "globals.h"
#include "pico/stdlib.h"
#include "pico/platform.h"
#include "Arduino.h"
#include "PS2_KB.h"
#include "pico/stdlib.h"
#include "pico/platform.h"
#include "Arduino.h"
#include "func_exports.h"

#ifndef TEXT_EDITOR_H
#define TEXT_EDITOR_H

void draw_line_pointer2(int x, int y, bool show_line_pointer);

void echo_message2(String message);

void userInput2();

void blinkCursor2();

void controls2();

void text_editor2(bool show, bool show_line_pointer, bool show_cursor);

#endif