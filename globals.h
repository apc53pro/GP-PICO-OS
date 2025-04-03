#include "pico/stdlib.h"
#include "pico/platform.h"
#include "Arduino.h"

#ifndef GLOBALS_H
#define GLOBALS_H

#define TXCOUNT 307200 // Total pixels

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
#define TOTAL_INTERNAL_RAM 100000  //DEFAULT IS 400000
#define DOUBLE_SIZE 16
#define INT64_SIZE 16
#define INT32_SIZE 12
#define INT16_SIZE 10
#define INT8_SIZE 9
#define BOOL_SIZE 9
#define CHAR_SIZE 9

extern uint8_t INTERNAL_RAM[TOTAL_INTERNAL_RAM];  // LARGEST VARIABLE (DOUBLE, INT64, UINT64) USES 8 BYTES OF RAM

extern uint32_t DOUBLE_START_ADDRESS;
extern uint32_t DOUBLE_END_ADDRESS;

extern uint32_t INT64_START_ADDRESS;
extern uint32_t INT64_END_ADDRESS;

extern uint32_t UINT64_START_ADDRESS;
extern uint32_t UINT64_END_ADDRESS;

extern uint32_t INT32_START_ADDRESS;
extern uint32_t INT32_END_ADDRESS;

extern uint32_t UINT32_START_ADDRESS;
extern uint32_t UINT32_END_ADDRESS;

extern uint32_t INT16_START_ADDRESS;
extern uint32_t INT16_END_ADDRESS;

extern uint32_t UINT16_START_ADDRESS;
extern uint32_t UINT16_END_ADDRESS;

extern uint32_t INT8_START_ADDRESS;
extern uint32_t INT8_END_ADDRESS;

extern uint32_t UINT8_START_ADDRESS;
extern uint32_t UINT8_END_ADDRESS;

extern uint32_t BOOL_START_ADDRESS;
extern uint32_t BOOL_END_ADDRESS;

extern uint32_t CHAR_START_ADDRESS;
extern uint32_t CHAR_END_ADDRESS;

extern uint32_t ARRAY_DATA_START_ADDRESS;
extern uint32_t ARRAY_DATA_END_ADDRESS;

extern uint16_t font[128][16];

#endif