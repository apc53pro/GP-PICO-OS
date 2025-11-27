#include <SPI.h>
#include <SD.h>
#include "pico/stdlib.h"
#include "pico/platform.h"
#include "Arduino.h"

#ifndef GLOBALS_H
#define GLOBALS_H

#define TXCOUNT 640*480 // Total pixels in 640p resolution

extern uint8_t vga_data_array[TXCOUNT];
extern uint8_t * address_pointer;

#define HSYNC 07    
#define VSYNC 06
// VGA timing constants
#define H_ACTIVE   655    // (active + frontporch - 1) - one cycle delay for mov
#define V_ACTIVE   479    // (active - 1)
#define RGB_ACTIVE 639    // (horizontal active) - 1
// Screen width/height
#define _width 640
#define _height 480
extern const int program_mem_size;
extern const int memory_size;


#define DOUBLE_SIZE 16
#define INT64_SIZE 16
#define INT32_SIZE 12
#define INT16_SIZE 10
#define INT8_SIZE 9
#define BOOL_SIZE 9
#define CHAR_SIZE 9

#define RED   0b11100000
#define GREEN 0b00011100
#define BLUE  0b00000011
#define BLACK 0b00000000
#define WHITE 0b11111111

extern bool runFromRam; // defines if a .bin program will run directly on the SD card or placed on the RAM then run

extern uint16_t font[128][16];

extern int leftBorder; //terminal left border
extern int rightBorder; //terminal right border

struct key_event
{
  int16_t key;
  bool released;
  bool printable = true;
  bool processed = true;
};

extern int file_mgr_cursorY;

extern struct key_event KEY_EVENT[20];
extern volatile int readCount;
extern volatile bool extChar;
extern volatile bool release;
extern volatile bool readCountSet;
extern volatile uint8_t key_code;
extern volatile int16_t KEY_CHAR;

extern char gridBuffer_char[40][60];

extern int text_editor_coulumn_amount;
extern char gridBuffer_char_text_editor[40][60];
extern uint8_t gridBuffer_charColor[40][60];
extern uint16_t gridBuffer_backColor[40][60];

extern int char_grid_buffer_V_draw_offset;
extern int char_grid_buffer_text_editor_V_draw_offset;

typedef struct
{
  union
  {
    int32_t i;
    float f;
    bool b;
    uint16_t str_ptr;
  }value;

}var;

#define opreg_size 1500
#define memory_size 30000 //Size is in amount of int32, so byte amount is memory_size*4

#define font_start_address 0
#define vbuff_start_address font_start_address + (256*8)

extern var memory[memory_size];

extern uint8_t regVarType[opreg_size];
extern uint8_t memVarType[memory_size];

struct line_pointer
{
  bool drawn = false;
  bool useBackColor = true;
  int color = 0;
  int backColor = 255;

  String line_pointer_str = ">:";
};

struct cursor
{
  int locationX = 2;
  int locationY = 0;

  bool showing = true;
  int blink_time_ms = 500;
  int ms = 0;
  char symbol = '_';
  int color = 0;
  int backColor = 255;
  bool useBackColor = true;
};

struct terminal_aspects
{
  struct cursor cur;
  struct line_pointer LP;

  int inputColor = 0;
  int inputBackColor = 255;
  bool useInputBackColor = true;
};

extern struct terminal_aspects terminal_aspects;
extern struct terminal_aspects terminal_aspects2;

extern bool caps;
extern bool shift;
extern bool numLock;
extern bool run_program;

extern String line;
extern bool graphics_mode;
extern uint8_t UI;
extern uint8_t lastUI;
extern bool terminal_on;
extern bool sdInitialized;

#endif