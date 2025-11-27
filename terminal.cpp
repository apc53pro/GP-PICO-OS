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
#include "terminal.h"
#include "LEXER.h"

struct terminal_aspects terminal_aspects;

int lastInputLine = 3;

const int inputHistoryAmount = 5;
String inputHistoryStr[inputHistoryAmount];
int inputHistoryPtr = 0;

int leftBorder = 2;
int rightBorder = 39;

void scroll_terminal_up(int times)
{
  for(int k = 0; k < times; k++)
  {
    for(int i = 1; i < 60; i++)
    {
      for(int j = 0; j < 40; j++)
      {
        gridBuffer_char[j][i-1] = gridBuffer_char[j][i];
        gridBuffer_charColor[j][i-1] = gridBuffer_charColor[j][i];
        gridBuffer_backColor[j][i-1] = gridBuffer_backColor[j][i];
      }
    }

    for(int i = 0; i < 40; i++)
    {
      gridBuffer_char[i][58] = ' ';
    }
  }

}

int textLinePtr = 0;

void draw_line_pointer(int x, int y, bool show_line_pointer)
{
  if(terminal_aspects.LP.drawn == false && show_line_pointer == true)
  {
    drawString_grid(gridBuffer_char, x, y, terminal_aspects.LP.line_pointer_str, terminal_aspects.LP.color, terminal_aspects.LP.backColor, terminal_aspects.LP.useBackColor);
    terminal_aspects.LP.drawn = true;
  }
}

void echo_message(String message)
{
  gridBuffer_char[terminal_aspects.cur.locationX][terminal_aspects.cur.locationY] = ' ';
  drawString_grid(gridBuffer_char, 0, terminal_aspects.cur.locationY, message, terminal_aspects.inputColor, terminal_aspects.inputBackColor, terminal_aspects.useInputBackColor);
  
  if(terminal_aspects.cur.locationY < 58)
  {
    terminal_aspects.cur.locationY++;
    lastInputLine = terminal_aspects.cur.locationY;

    if(terminal_aspects.cur.locationY > 30 + char_grid_buffer_V_draw_offset - 2 && char_grid_buffer_V_draw_offset < 30)
    {
      char_grid_buffer_V_draw_offset = terminal_aspects.cur.locationY - 30 + 2;
    }
  }else
  {
    scroll_terminal_up(1);

    if(terminal_aspects.cur.locationY > 30 + char_grid_buffer_V_draw_offset - 2 && char_grid_buffer_V_draw_offset < 30)
    {
      char_grid_buffer_V_draw_offset = terminal_aspects.cur.locationY - 30 + 2;
    }
  }
  
  terminal_aspects.LP.drawn = false;
}

char textChar; // holds the last char at cursor coordinates

void userInput()
{
  int key_event_ptr = 0;

  for(int i = 0; i < 20; i++)
  {
    if(KEY_EVENT[i].processed == false && KEY_EVENT[i].printable == true && KEY_EVENT[i].released == false)
    {
        char c = KEY_CHAR;
        drawString_grid(gridBuffer_char, terminal_aspects.cur.locationX, terminal_aspects.cur.locationY, String(c), terminal_aspects.inputColor, terminal_aspects.inputBackColor, terminal_aspects.useInputBackColor);
        if(textLinePtr >= line.length())
        {
          line.concat(" ");
        }
        line.setCharAt(textLinePtr, c);

        textLinePtr++;

        terminal_aspects.cur.locationX++;

        if(terminal_aspects.cur.locationX > rightBorder)
        {
          terminal_aspects.cur.locationX = leftBorder;
          terminal_aspects.cur.locationY++;
        }

        if(terminal_aspects.cur.locationY > 58)
        {
          terminal_aspects.cur.locationY--;
          terminal_aspects.cur.locationX = leftBorder;

          scroll_terminal_up(1);
          if(terminal_aspects.cur.locationY > 30 + char_grid_buffer_V_draw_offset - 2 && char_grid_buffer_V_draw_offset < 30)
          {
            char_grid_buffer_V_draw_offset = terminal_aspects.cur.locationY - 30 + 2;
          }
        }

        KEY_EVENT[i].processed = true;
    }else if(KEY_EVENT[i].processed == false && KEY_EVENT[i].released == false && KEY_EVENT[i].key == 20013)
    {
        if(terminal_aspects.cur.locationX > leftBorder || terminal_aspects.cur.locationY > lastInputLine)
        {
          textLinePtr--;
          line.setCharAt(textLinePtr, ' ');
          

          terminal_aspects.cur.locationX--;
          gridBuffer_char[terminal_aspects.cur.locationX+1][terminal_aspects.cur.locationY] = textChar;
          if(terminal_aspects.cur.locationX < leftBorder)
          {
            terminal_aspects.cur.locationX = rightBorder;
            terminal_aspects.cur.locationY--;
          }

          if(terminal_aspects.cur.locationY < 0)
          {
            terminal_aspects.cur.locationY = 0;
            terminal_aspects.cur.locationX = leftBorder;
          }

          gridBuffer_char[terminal_aspects.cur.locationX][terminal_aspects.cur.locationY] = ' ';
        }

        KEY_EVENT[i].processed = true;
    }else if(KEY_EVENT[i].processed == false && KEY_EVENT[i].released == false && KEY_EVENT[i].key == 13)
    {
        
        gridBuffer_char[terminal_aspects.cur.locationX][terminal_aspects.cur.locationY] = textChar;
        if(terminal_aspects.cur.locationY < 58)
        {
          terminal_aspects.cur.locationY++;

          if(terminal_aspects.cur.locationY > 30 + char_grid_buffer_V_draw_offset - 2 && char_grid_buffer_V_draw_offset < 30)
          {
            char_grid_buffer_V_draw_offset = terminal_aspects.cur.locationY - 30 + 2;
          }
        }else
        {
          gridBuffer_char[terminal_aspects.cur.locationX][terminal_aspects.cur.locationY] = textChar;
          scroll_terminal_up(1);

          if(terminal_aspects.cur.locationY > 30 + char_grid_buffer_V_draw_offset - 2 && char_grid_buffer_V_draw_offset < 30)
          {
            char_grid_buffer_V_draw_offset = terminal_aspects.cur.locationY - 30 + 2;
          }
        }

        Serial.print("\nLINE: ");
        line.trim();
        Serial.print(line);
        lastInputLine = terminal_aspects.cur.locationY;
        terminal_aspects.cur.locationX = leftBorder;
        LEXER(line);

        /*for(int j = inputHistoryAmount; j > 0; j--;)
        {
          inputHistoryStr[j] = inputHistoryStr[j-1];
        }*/

        line = "";
        textLinePtr = 0;
        terminal_aspects.LP.drawn = false;

        KEY_EVENT[i].processed = true;
    }else if(KEY_EVENT[i].processed == false && KEY_EVENT[i].released == true)
    {
        KEY_EVENT[i].processed = true;
    }

  }

  
}

int prevCurX = 0;
int prevCurY = 0;

void blinkCursor(int x, int y, bool active)
{
  if(active == true)
  {
    if(millis() - terminal_aspects.cur.ms > terminal_aspects.cur.blink_time_ms)
    {
      terminal_aspects.cur.showing = !terminal_aspects.cur.showing;
      terminal_aspects.cur.ms = millis();
    }

    if(prevCurX != terminal_aspects.cur.locationX || prevCurY != terminal_aspects.cur.locationY)
    {
      //gridBuffer_char[prevCurX][prevCurY] = textChar;
      textChar = gridBuffer_char[terminal_aspects.cur.locationX][terminal_aspects.cur.locationY];

      prevCurX = terminal_aspects.cur.locationX;
      prevCurY = terminal_aspects.cur.locationY;
    }

    if(terminal_aspects.cur.showing == true)
    {
      
      gridBuffer_char[x][y] = terminal_aspects.cur.symbol;
      gridBuffer_charColor[x][y] = terminal_aspects.cur.color;
      
      if(terminal_aspects.cur.useBackColor == true)
      {
        gridBuffer_backColor[x][y] = terminal_aspects.cur.backColor;
      }else
      {
        gridBuffer_backColor[x][y] = 300;
      }

    }else
    {
      if(textChar != ' ')
      {
        gridBuffer_char[x][y] = textChar;
        gridBuffer_charColor[x][y] = terminal_aspects.cur.color;
      }else
      {
        gridBuffer_char[x][y] = ' ';
        gridBuffer_charColor[x][y] = terminal_aspects.cur.color;
      }
      

      if(terminal_aspects.cur.useBackColor == true)
      {
        gridBuffer_backColor[x][y] = terminal_aspects.cur.backColor;
      }else
      {
        gridBuffer_backColor[x][y] = 300;
      }
    }
  }
}

void controls()
{
  for(int i = 0; i < 20; i++)
  {
    if(KEY_EVENT[i].key == 20012 && KEY_EVENT[i].released == false && KEY_EVENT[i].processed == false)
    {
      shift = true;
      KEY_EVENT[i].processed = true;
    }else if(KEY_EVENT[i].key == 20012 && KEY_EVENT[i].released == true && KEY_EVENT[i].processed == false)
    {
      shift = false;
      KEY_EVENT[i].processed = true;
    }
    
    if(KEY_EVENT[i].key == 20000 && KEY_EVENT[i].released == false && KEY_EVENT[i].processed == false)
    {
      caps = !caps;
      KEY_EVENT[i].processed = true;
    }
    
    if(KEY_EVENT[i].key == 20011 && KEY_EVENT[i].released == false && KEY_EVENT[i].processed == false)
    {
      numLock = !numLock;
      KEY_EVENT[i].processed = true;
    }

    if(KEY_EVENT[i].key == 20001 && KEY_EVENT[i].released == false && KEY_EVENT[i].processed == false)
    {
      if(char_grid_buffer_V_draw_offset > 0)
      {
        char_grid_buffer_V_draw_offset--;
      }
      KEY_EVENT[i].processed = true;
    }

    if(KEY_EVENT[i].key == 20002 && KEY_EVENT[i].released == false && KEY_EVENT[i].processed == false)
    {
      if(char_grid_buffer_V_draw_offset < 30)
      {
        char_grid_buffer_V_draw_offset++;
      }
      KEY_EVENT[i].processed = true;
    }

    if(KEY_EVENT[i].key == 20003 && KEY_EVENT[i].released == false && KEY_EVENT[i].processed == false)
    {
      if(terminal_aspects.cur.locationX > leftBorder || terminal_aspects.cur.locationY > lastInputLine)
      {
       
        gridBuffer_char[terminal_aspects.cur.locationX][terminal_aspects.cur.locationY] = textChar;
        terminal_aspects.cur.locationX--;

        if(terminal_aspects.cur.locationY > lastInputLine && terminal_aspects.cur.locationX < leftBorder)
        {
          terminal_aspects.cur.locationY--;
          terminal_aspects.cur.locationX = rightBorder;
        }

        textLinePtr--;
      }

      KEY_EVENT[i].processed = true;
    }

    if(KEY_EVENT[i].key == 20004 && KEY_EVENT[i].released == false && KEY_EVENT[i].processed == false)
    {
      if(textLinePtr >= line.length())
      {
        line.concat(" ");
      }

      gridBuffer_char[terminal_aspects.cur.locationX][terminal_aspects.cur.locationY] = textChar;
      terminal_aspects.cur.locationX++;
      textLinePtr++;

      if(terminal_aspects.cur.locationX > rightBorder)
      {
        terminal_aspects.cur.locationY++;
        terminal_aspects.cur.locationX = leftBorder;
      }
      

      KEY_EVENT[i].processed = true;
    }
  }
}

void terminal(bool show, bool show_line_pointer, bool show_cursor)
{
  
  if(show == true)
  {

    draw_line_pointer(0, terminal_aspects.cur.locationY, show_line_pointer);

    controls();

    blinkCursor(terminal_aspects.cur.locationX, terminal_aspects.cur.locationY, show_cursor);    
  
    userInput();

    drawCharGridBuffer();

  }
}


