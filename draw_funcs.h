#include "globals.h"
#include "pico/stdlib.h"
#include "pico/platform.h"
#include "Arduino.h"
#include "PS2_KB.h"
#include "pico/stdlib.h"
#include "pico/platform.h"
#include "Arduino.h"
#include "func_exports.h"

#ifndef DRAW_FUNCS_H
#define DRAW_FUNCS_H

void drawString_grid(char arr[40][60], uint8_t column, uint8_t line, String str, uint8_t color, uint8_t backColor, bool useBackColor);

void drawCharGridBuffer();

void drawCharGridBuffer_text_editor(uint16_t originX, uint16_t originY, int column_amount);

void drawLetter(int16_t baseX, int16_t baseY, char letter, uint8_t color, uint8_t backColor, bool useBackColor);

void drawString_free(uint16_t baseX, uint16_t baseY, int letterSpacing, String str, uint8_t color, uint8_t backColor, bool useBackColor);

void drawPixel(int16_t pixelX, int16_t pixelY, uint8_t pixelColor);

void draw_pixel_half_res_buffer(int x, int y, uint8_t color);

void drawThiccPixel(int x, int y, int thiccness);

void draw_line_half_res_buffer(int x0, int y0, int x1, int y1, uint8_t color, int thiccness);

inline void draw_half_res_buffer()
{
  /*for(int i = 0; i < 240; i++)
  {
    for(int j = 0; j < 320; j++)
    {
      vga_data_array[(640 * (i*2)) + (j*2)] = half_res_buffer[(320*i) + j];
      vga_data_array[(640 * (i*2)) + (j*2) + 1] = half_res_buffer[(320*i) + j];
    
      vga_data_array[(640 * ((i*2) + 1)) + (j*2)] = half_res_buffer[(320*i) + j];
      vga_data_array[(640 * ((i*2) + 1)) + (j*2) + 1] = half_res_buffer[(320*i) + j];
      
    } 
  }*/

  for (uint16_t y = 0; y < 240; y++) 
  {
    for (uint16_t x = 0; x < 320; x++) 
    {
      uint32_t pixel_index = (y * 320) + x;
      uint32_t slot_index = pixel_index >> 2;
      uint32_t pixelInSlot = pixel_index & 3;

      static const uint8_t shift_lookup[4] = {0, 8, 16, 24};
      uint8_t shift = shift_lookup[pixelInSlot];

      uint8_t color = (memory[vbuff_start_address + slot_index].value.i >> shift) & 0xFF;

      int tx = x * 2;
      int ty = y * 2;

      vga_data_array[(ty + 0) * 640 + (tx + 0)] = color;
      vga_data_array[(ty + 0) * 640 + (tx + 1)] = color;
      vga_data_array[(ty + 1) * 640 + (tx + 0)] = color;
      vga_data_array[(ty + 1) * 640 + (tx + 1)] = color;
    }
  }   
}

#endif