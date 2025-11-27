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
#include "text_editor.h"
#include "LEXER.h"
#include "file_mgr.h"

struct terminal_aspects terminal_aspects2;

int char_grid_buffer_text_editor_V_draw_offset;

int lastInputLine2 = 3;

const int inputHistoryAmount2 = 5;
String inputHistoryStr2[inputHistoryAmount2];
int inputHistoryPtr2 = 0;

int leftBorder2 = 1;
int text_editor_coulumn_amount = 40;

int textLinePtr2 = 0;

String fileName = "                             ";
int fileNamePtr = 0;

void draw_line_pointer2(int x, int y, bool show_line_pointer)
{
  if(terminal_aspects2.LP.drawn == false && show_line_pointer == true)
  {
    drawString_grid(gridBuffer_char_text_editor, x, y, terminal_aspects2.LP.line_pointer_str, terminal_aspects2.LP.color, terminal_aspects2.LP.backColor, terminal_aspects2.LP.useBackColor);
    terminal_aspects2.LP.drawn = true;
  }
}

void scroll_terminal_up2(int times)
{
  for(int k = 0; k < times; k++)
  {
    for(int i = 1; i < 60; i++)
    {
      for(int j = 0; j < text_editor_coulumn_amount; j++)
      {
        gridBuffer_char_text_editor[j][i-1] = gridBuffer_char_text_editor[j][i];
      }
    }

    for(int i = 0; i < text_editor_coulumn_amount; i++)
    {
     gridBuffer_char_text_editor[i][58] = ' ';
    }
  }

}

void echo_message2(String message)
{
  gridBuffer_char_text_editor[terminal_aspects2.cur.locationX][terminal_aspects2.cur.locationY] = ' ';
  drawString_grid(gridBuffer_char_text_editor, 0, terminal_aspects2.cur.locationY, message, terminal_aspects2.inputColor, terminal_aspects2.inputBackColor, terminal_aspects2.useInputBackColor);
  
  if(terminal_aspects2.cur.locationY < 58)
  {
    terminal_aspects2.cur.locationY++;

    if(terminal_aspects2.cur.locationY > 30 + char_grid_buffer_text_editor_V_draw_offset - 2 && char_grid_buffer_text_editor_V_draw_offset < 30)
    {
      char_grid_buffer_text_editor_V_draw_offset = terminal_aspects2.cur.locationY - 30 + 2;
    }
  }else
  {
    scroll_terminal_up2(1);

    if(terminal_aspects2.cur.locationY > 30 + char_grid_buffer_text_editor_V_draw_offset - 2 && char_grid_buffer_text_editor_V_draw_offset < 30)
    {
      char_grid_buffer_text_editor_V_draw_offset = terminal_aspects2.cur.locationY - 30 + 2;
    }
  }
  
  terminal_aspects2.LP.drawn = false;
}

char textChar2; // holds the last char at cursor coordinates

void userInput2()
{
  int key_event_ptr = 0;

  for(int i = 0; i < 20; i++)
  {
    if(KEY_EVENT[i].processed == false && KEY_EVENT[i].printable == true && KEY_EVENT[i].released == false)
    {
      
      char c = KEY_CHAR;
      drawString_grid(gridBuffer_char_text_editor, terminal_aspects2.cur.locationX, terminal_aspects2.cur.locationY, String(c), terminal_aspects2.inputColor, terminal_aspects2.inputBackColor, terminal_aspects2.useInputBackColor);
      if(textLinePtr2 >= line.length())
      {
        line.concat(" ");
      }
      line.setCharAt(textLinePtr2, c);

      textLinePtr2++;

      terminal_aspects2.cur.locationX++;

      if(terminal_aspects2.cur.locationX >= text_editor_coulumn_amount)
      {
        terminal_aspects2.cur.locationX = leftBorder2;
        terminal_aspects2.cur.locationY++;
      }
      

      KEY_EVENT[i].processed = true;
    }else if(KEY_EVENT[i].processed == false && KEY_EVENT[i].released == false && KEY_EVENT[i].key == 20013)
    {
      
      if(terminal_aspects2.cur.locationX > 1 || terminal_aspects2.cur.locationY > 0)
      {
        line.setCharAt(textLinePtr2, ' ');
        textLinePtr2--;

        terminal_aspects2.cur.locationX--;
        gridBuffer_char_text_editor[terminal_aspects2.cur.locationX+1][terminal_aspects2.cur.locationY] = textChar2;
        if(terminal_aspects2.cur.locationX < leftBorder2)
        {
          terminal_aspects2.cur.locationX = text_editor_coulumn_amount - 1;
          terminal_aspects2.cur.locationY--;
        }

        if(terminal_aspects2.cur.locationY < 0)
        {
          terminal_aspects2.cur.locationY = 0;
          terminal_aspects2.cur.locationX = leftBorder2;
        }

        gridBuffer_char_text_editor[terminal_aspects2.cur.locationX][terminal_aspects2.cur.locationY] = ' ';
      }
      
      KEY_EVENT[i].processed = true;
    }else if(KEY_EVENT[i].processed == false && KEY_EVENT[i].released == false && KEY_EVENT[i].key == 13)
    {
    
      gridBuffer_char_text_editor[terminal_aspects2.cur.locationX][terminal_aspects2.cur.locationY] = textChar2;
      if(terminal_aspects2.cur.locationY < 59)
      {
        terminal_aspects2.cur.locationY++;

        if(terminal_aspects2.cur.locationY > 28 + char_grid_buffer_text_editor_V_draw_offset - 2 && char_grid_buffer_text_editor_V_draw_offset < 28)
        {
          char_grid_buffer_text_editor_V_draw_offset = terminal_aspects2.cur.locationY - 28 + 2;
        }
      }


      lastInputLine2 = terminal_aspects2.cur.locationY;
      terminal_aspects2.cur.locationX = leftBorder2;

      textLinePtr2 = 0;
      terminal_aspects2.LP.drawn = false;
      KEY_EVENT[i].processed = true;
      

    }

  }
}

int prevCurX2 = 0;
int prevCurY2 = 0;

void blinkCursor2(int x, int y, bool active)
{
  if(active == true)
  {
    if(millis() - terminal_aspects2.cur.ms > terminal_aspects2.cur.blink_time_ms)
    {
      terminal_aspects2.cur.showing = !terminal_aspects2.cur.showing;
      terminal_aspects2.cur.ms = millis();
    }

    if(prevCurX2 != terminal_aspects2.cur.locationX || prevCurY2 != terminal_aspects2.cur.locationY)
    {
      //gridBuffer_char_text_editor[prevCurX2][prevCurY] = textChar2;
      textChar2 = gridBuffer_char_text_editor[terminal_aspects2.cur.locationX][terminal_aspects2.cur.locationY];

      prevCurX2 = terminal_aspects2.cur.locationX;
      prevCurY2 = terminal_aspects2.cur.locationY;
    }

    if(terminal_aspects2.cur.showing == true)
    {
      
      gridBuffer_char_text_editor[x][y] = terminal_aspects2.cur.symbol;
      

    }else
    {
      if(textChar2 != ' ')
      {
        gridBuffer_char_text_editor[x][y] = textChar2;
      }else
      {
        gridBuffer_char_text_editor[x][y] = ' ';
      }
      
    }
  }
}

void controls2()
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
      gridBuffer_char_text_editor[terminal_aspects2.cur.locationX][terminal_aspects2.cur.locationY] = textChar2;
      
      if(terminal_aspects2.cur.locationY > 0)
      {
        terminal_aspects2.cur.locationY--;

        if(terminal_aspects2.cur.locationY < char_grid_buffer_text_editor_V_draw_offset)
        {
          char_grid_buffer_text_editor_V_draw_offset = terminal_aspects2.cur.locationY;
        }
      }
      
      KEY_EVENT[i].processed = true;
    }

    if(KEY_EVENT[i].key == 20002 && KEY_EVENT[i].released == false && KEY_EVENT[i].processed == false)
    {
      gridBuffer_char_text_editor[terminal_aspects2.cur.locationX][terminal_aspects2.cur.locationY] = textChar2;

      if(terminal_aspects2.cur.locationY < 58)
      {
        terminal_aspects2.cur.locationY++;

        if(terminal_aspects2.cur.locationY > 28 + char_grid_buffer_text_editor_V_draw_offset - 2 && char_grid_buffer_text_editor_V_draw_offset < 32)
        {
          char_grid_buffer_text_editor_V_draw_offset = terminal_aspects2.cur.locationY - 28 + 2;
        }
      }else if(terminal_aspects2.cur.locationY == 58)
      {
        terminal_aspects2.cur.locationY++;
      }

      KEY_EVENT[i].processed = true;
    }

    if(KEY_EVENT[i].key == 20003 && KEY_EVENT[i].released == false && KEY_EVENT[i].processed == false)
    {
      if(terminal_aspects2.cur.locationX > 1 || terminal_aspects2.cur.locationY > 0)
      {
        gridBuffer_char_text_editor[terminal_aspects2.cur.locationX][terminal_aspects2.cur.locationY] = textChar2;
        terminal_aspects2.cur.locationX--;

        if(terminal_aspects2.cur.locationX < leftBorder2)
        {
          terminal_aspects2.cur.locationY--;
          terminal_aspects2.cur.locationX = text_editor_coulumn_amount - 1;
        }
      }

      KEY_EVENT[i].processed = true;
    }

    if(KEY_EVENT[i].key == 20004 && KEY_EVENT[i].released == false && KEY_EVENT[i].processed == false)
    {
      

      if(textLinePtr2 >= line.length())
      {
        line.concat(" ");
      }

      gridBuffer_char_text_editor[terminal_aspects2.cur.locationX][terminal_aspects2.cur.locationY] = textChar2;
      terminal_aspects2.cur.locationX++;
      textLinePtr2++;

      if(terminal_aspects2.cur.locationX >= text_editor_coulumn_amount)
      {
        terminal_aspects2.cur.locationY++;
        terminal_aspects2.cur.locationX = leftBorder2;
      }
      
      KEY_EVENT[i].processed = true;
    }

    if(KEY_EVENT[i].key == 20015 && KEY_EVENT[i].released == false && KEY_EVENT[i].processed == false)
    {
      //String file = fileName;

      //file.trim();

      String file = file_mgr_open();
      File sdFile = SD.open(file);

      if(sdFile == true && file.length() > 0)
      {
        
        int counter = 0;
        int x;
        while(sdFile.available())
        {
          x = (counter%text_editor_coulumn_amount) + 1;
      
          gridBuffer_char_text_editor[x][counter/text_editor_coulumn_amount] = sdFile.read();
          counter++;
        }

        while(counter < text_editor_coulumn_amount*60)
        {
          gridBuffer_char_text_editor[counter%text_editor_coulumn_amount][counter/text_editor_coulumn_amount] = ' ';
          counter++;
        }

        textChar2 = gridBuffer_char_text_editor[terminal_aspects2.cur.locationX][terminal_aspects2.cur.locationY];
      }else 
      {
        //echo_message("=>NO FILE NAMED \"" + tokens[2] + "\" FOUND IN SD.");
        //echo_message("");
      }
      sdFile.close();
      

      KEY_EVENT[i].processed = true;
    }

    if(KEY_EVENT[i].key == 20016 && KEY_EVENT[i].released == false && KEY_EVENT[i].processed == false)//is fucked, crashes system
    {
      gridBuffer_char_text_editor[terminal_aspects2.cur.locationX][terminal_aspects2.cur.locationY] = textChar2;
          
      String file = file_mgr_save();

      if(file.length() > 0)
      {
        file.trim();
        File sdFile = SD.open(file);
        int counter = 0;

        if(sdFile == true)
        {
          SD.remove(file);
          
        } 
        
        sdFile = SD.open(file, FILE_WRITE);
        for(int i = 0; i < 60; i++)
        {
          for(int k = 1; k < text_editor_coulumn_amount; k++)
          {
            sdFile.print(gridBuffer_char_text_editor[k][i]);
          }
        }

        sdFile.close();
      }

      KEY_EVENT[i].processed = true;
    }
  }
}

void text_editor2(bool show, bool show_line_pointer, bool show_cursor)
{
  
  if(show == true)
  {

    //draw_line_pointer2(0, terminal_aspects2.cur.locationY, show_line_pointer);

    controls2();

    blinkCursor2(terminal_aspects2.cur.locationX, terminal_aspects2.cur.locationY, show_cursor);    
  
    userInput2();

    drawString_free(0, 0, 16, String(" FILE NAME:"), 255, 0x73, true);
    
    
    drawString_free(0, 16, 16, String(" F1:OPEN "), 255, 0x73, true);
    

    
    drawString_free(128, 16, 16, String("   F2:SAVE                      "), 255, 0x73, true);
    //green: 0b00011100
    
    
    drawString_free(176, 0, 16, String(fileName), 255, 0x73, true);
    
    drawCharGridBuffer_text_editor(0, 32, text_editor_coulumn_amount);

  }
}


