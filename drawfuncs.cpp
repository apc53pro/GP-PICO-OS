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

char gridBuffer_char_text_editor[40][60];
bool graphics_mode;

void drawString_grid(char arr[40][60], uint8_t column, uint8_t line, String str, uint8_t color, uint8_t backColor, bool useBackColor)
{
  if(column < 40 && line < 60)
  {
    int counter = 0;

    while(counter < str.length()) 
    {
      if(16*column > 639)
      {
        if(line < 59)
        {
          column = 0;
          line++;
        }else
        {
          column = 0;
          line = 0;
        }
      }

      arr[column][line] = str.charAt(counter);
      gridBuffer_charColor[column][line] = color;

      if(useBackColor == false)
      {
        gridBuffer_backColor[column][line] = 300;
      }else
      {
        gridBuffer_backColor[column][line] = backColor;
      }

      counter++;
      column++;
    }
  }
}

void drawCharGridBuffer()
{
  for(int i = 0; i < 30; i++)
  {
    for(int j = 0; j < 40; j++)
    {
      if(gridBuffer_backColor[j][i] == 300)
      {
        drawLetter(16*j, i*16, gridBuffer_char[j][i + char_grid_buffer_V_draw_offset], gridBuffer_charColor[j][i + char_grid_buffer_V_draw_offset], gridBuffer_backColor[j][i + char_grid_buffer_V_draw_offset], false);
      }else
      {
        drawLetter(16*j, i*16, gridBuffer_char[j][i + char_grid_buffer_V_draw_offset], gridBuffer_charColor[j][i + char_grid_buffer_V_draw_offset], gridBuffer_backColor[j][i + char_grid_buffer_V_draw_offset], true);
      }
    }
  }
}

void drawCharGridBuffer_text_editor(uint16_t originX, uint16_t originY, int column_amount)
{
  int lineCounter = 1; //ASCII FOR 1
  for(int i = 0; i < 30; i++)
  {
    for(int j = 0; j < column_amount; j++)
    {
      
      if((16*j) + originX == 0)
      {
        drawString_free((16*j) + originX, (i*16) + originY, 8, String(char_grid_buffer_text_editor_V_draw_offset + lineCounter++), 255, 0x73, true);
      }else
      {
        drawLetter((16*j) + originX, (i*16) + originY, gridBuffer_char_text_editor[j][i + char_grid_buffer_text_editor_V_draw_offset], 255, 0x4f, true);
      }
      
    }
  }
}

void drawLetter(int16_t baseX, int16_t baseY, char letter, uint8_t color, uint8_t backColor, bool useBackColor)
{
  
  for(int i = 0; i < 8; i++)
  {
    uint16_t byte1 = (memory[font_start_address + (letter*8) + i].value.i >> 16) | 0b0000000000000000;
    uint16_t byte2 = memory[font_start_address + (letter*8) + i].value.i | 0b0000000000000000;

    for(int j = 0; j < 16; j++)
    {
      if(0b1000000000000000 & byte1 << j)
      {
        if(color != 0 || graphics_mode == false)
        {
          drawPixel(baseX + j, (baseY + (i*2)), color);
        }
        
      }else if(useBackColor == true)
      {
        if(backColor != 0 || graphics_mode == false)
        {
          drawPixel(baseX + j, (baseY + (i*2)), backColor);
        }
      }

      if(0b1000000000000000 & byte2 << j)
      {
        if(color != 0 || graphics_mode == false)
        {
          drawPixel(baseX + j, (baseY + (i*2)+1), color);
        }
        
      }else if(useBackColor == true)
      {
        if(backColor != 0 || graphics_mode == false)
        {
          drawPixel(baseX + j, (baseY + (i*2)+1), backColor);
        }
      }
    }
  }
  
}

void drawString_free(uint16_t baseX, uint16_t baseY, int letterSpacing, String str, uint8_t color, uint8_t backColor, bool useBackColor)
{
  for(int i = 0; i < str.length(); i++)
  {
    drawLetter(baseX+(letterSpacing*i), baseY, str.charAt(i), color, backColor, useBackColor);
  }
}

void drawPixel(int16_t pixelX, int16_t pixelY, uint8_t pixelColor)
{

  uint32_t pixelAddress = (pixelY * 640) + pixelX;

  if(pixelAddress >= 0 && pixelAddress < 307200)
  {
    vga_data_array[pixelAddress] = pixelColor;
  }
}

void draw_pixel_half_res_buffer(int x, int y, uint8_t color)
{
  if (x >= 0 && x < 320 && y >= 0 && y < 240) 
  {
    //half_res_buffer[(320*y) + x] = color;
  }
}

void drawThiccPixel(int x, int y, int thiccness, int color) 
{
  int r = thiccness / 2; // radius
  for (int dy = -r; dy <= r; dy++) 
  {
    for (int dx = -r; dx <= r; dx++) 
    {
      
      draw_pixel_half_res_buffer(x + dx, y + dy, color);
      
    }
  }
}

void draw_line_half_res_buffer(int x0, int y0, int x1, int y1, uint8_t color, int thiccness) 
{
  int dx = abs(x1 - x0);
  int sx = (x0 < x1) ? 1 : -1;
  int dy = -abs(y1 - y0);
  int sy = (y0 < y1) ? 1 : -1;
  int err = dx + dy; // error term

  while (true) 
  {
    // mark the pixel if inside bounds
    if (x0 >= 0 && x0 < 320 && y0 >= 0 && y0 < 240) 
    {
      drawThiccPixel(x0, y0, thiccness, color);
    }

    if (x0 == x1 && y0 == y1) break;

    int e2 = 2 * err;
    if (e2 >= dy) { err += dy; x0 += sx; }
    if (e2 <= dx) { err += dx; y0 += sy; }
  }
}

