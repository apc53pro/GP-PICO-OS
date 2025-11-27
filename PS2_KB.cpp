#include "globals.h"
#include "pico/stdlib.h"
#include "pico/platform.h"
#include "Arduino.h"
#include "PS2_KB.h"
#include "pico/stdlib.h"
#include "pico/platform.h"
#include "Arduino.h"
#include "func_exports.h"
#include "draw_funcs.h"
#include "bytecode_interpreter.h"

volatile int readCount;
volatile bool extChar;
volatile bool release;
volatile bool readCountSet;
volatile uint8_t key_code;
volatile int16_t KEY_CHAR = -20000;

struct key_event KEY_EVENT[20];
int key_event_ptr = 0;

bool caps = false;
bool shift = false;
bool numLock = true;

struct key_event returnKBEvent(bool caps, bool shift, bool extChar, bool release, uint8_t key_code)
{
  uint16_t alpha[200];
  memset(alpha, 0, sizeof(alpha));
  KEY_EVENT[key_event_ptr].printable = true; //every key event is printable until proven otherwise

  if(shift == false && caps == false)
  {
    alpha[0x1c] = 'a'; alpha[0x32] = 'b'; alpha[0x21] = 'c'; alpha[0x23] = 'd'; alpha[0x24] = 'e'; alpha[0x2b] = 'f';
    alpha[0x34] = 'g'; alpha[0x33] = 'h'; alpha[0x43] = 'i'; alpha[0x3b] = 'j'; alpha[0x42] = 'k'; alpha[0x4b] = 'l';
    alpha[0x3a] = 'm'; alpha[0x31] = 'n'; alpha[0x44] = 'o'; alpha[0x4d] = 'p'; alpha[0x15] = 'q'; alpha[0x2d] = 'r';
    alpha[0x1b] = 's'; alpha[0x2c] = 't'; alpha[0x3c] = 'u'; alpha[0x2a] = 'v'; alpha[0x1d] = 'w'; alpha[0x22] = 'x';
    alpha[0x35] = 'y'; alpha[0x1a] = 'z'; 
  }else
  {
    alpha[0x1c] = 'A'; alpha[0x32] = 'B'; alpha[0x21] = 'C'; alpha[0x23] = 'D'; alpha[0x24] = 'E'; alpha[0x2b] = 'F';
    alpha[0x34] = 'G'; alpha[0x33] = 'H'; alpha[0x43] = 'I'; alpha[0x3b] = 'J'; alpha[0x42] = 'K'; alpha[0x4b] = 'L';
    alpha[0x3a] = 'M'; alpha[0x31] = 'N'; alpha[0x44] = 'O'; alpha[0x4d] = 'P'; alpha[0x15] = 'Q'; alpha[0x2d] = 'R';
    alpha[0x1b] = 'S'; alpha[0x2c] = 'T'; alpha[0x3c] = 'U'; alpha[0x2a] = 'V'; alpha[0x1d] = 'W'; alpha[0x22] = 'X';
    alpha[0x35] = 'Y'; alpha[0x1a] = 'Z'; 
  }

  if(shift == false)
  {
    alpha[0x45] = '0'; alpha[0x16] = '1'; alpha[0x1e] = '2'; alpha[0x26] = '3'; alpha[0x25] = '4'; alpha[0x2e] = '5';
    alpha[0x36] = '6'; alpha[0x3d] = '7'; alpha[0x3e] = '8'; alpha[0x46] = '9';

    alpha[0x0e] = '\''; alpha[0x4e] = '-'; alpha[0x55] = '='; alpha[0x5d] = ']'; alpha[0x49] = '.'; alpha[0x0d] = '\t';
    alpha[0x4a] = ';'; alpha[0x5b] = '['; alpha[0x54] = '´'; alpha[0x41] = ','; alpha[0x61] = '\\';
    alpha[0x51] = '/';
  }else
  {
    alpha[0x45] = ')'; alpha[0x16] = '!'; alpha[0x1e] = '@'; alpha[0x26] = '#'; alpha[0x25] = '$'; alpha[0x2e] = '%';
    alpha[0x36] = '^'; alpha[0x3d] = '&'; alpha[0x3e] = '*'; alpha[0x46] = '(';

    alpha[0x0e] = '"'; alpha[0x4e] = '_'; alpha[0x55] = '+'; alpha[0x5d] = '}'; alpha[0x49] = '>'; alpha[0x0d] = '\t';
    alpha[0x4a] = ':'; alpha[0x5b] = '{'; alpha[0x54] = '´'; alpha[0x41] = '<'; alpha[0x61] = '|';
    alpha[0x51] = '?';
  }

  alpha[0x29] = ' ';         alpha[0x66] = 20013; 
  
  //CapsLock                 NUMLOCK                 SHIFT                       ESC
  alpha[0x58] = 20000;       alpha[0x77] = 20011;    alpha[0x12] = 20012;        alpha[0x76] = 20014;

  alpha[0x05] = 20015; //F1
  alpha[0x06] = 20016; //F2
  alpha[0x04] = 20017; //F3
  alpha[0x0c] = 20018; //F4
  alpha[0x03] = 20019; //F5
  alpha[0x0b] = 20020; //F6
  alpha[0x83] = 20021; //F7
  alpha[0x0a] = 20022; //F8
  alpha[0x01] = 20023; //F9
  alpha[0x09] = 20024; //F10
  alpha[0x78] = 20025; //F11
  alpha[0x07] = 20026; //F12

  if(numLock == false)
  {
    //U ARROW               D ARROW                 L ARROW                R ARROW           
    alpha[0x75] = 20001;    alpha[0x72] = 20002;    alpha[0x6b] = 20003;   alpha[0x74] = 20004; 

    //INSERT                //END                   //PG DN
    alpha[0x70] = 20005;    alpha[0x69] = 20006;    alpha[0x7a] = 20007;  alpha[0x73] = '5';  
    
    //HOME                       //PG UP                 
    alpha[0x6c] = 20008;    alpha[0x7d] = 20009;     alpha[0x7c] = '*';   alpha[0x7b] = '-';      
    //                                                //ENTER
    alpha[0x79] = '+';    alpha[0x6d] = '.';   alpha[0x5a] = 13;

    if(extChar == true)
    {
      alpha[0x4a] = '/';
    }else if(shift == false)
    {
      alpha[0x4a] = ';';
    }else if(shift == true)
    {
      alpha[0x4a] = ':';
    }

    //DEL
    alpha[0x71] = 20010;
  }else
  {
    alpha[0x70] = '0';    alpha[0x69] = '1';    alpha[0x72] = '2';   alpha[0x7a] = '3';
    alpha[0x6b] = '4';    alpha[0x73] = '5';    alpha[0x74] = '6';   alpha[0x6c] = '7';
    alpha[0x75] = '8';    alpha[0x7d] = '9';    alpha[0x7c] = '*';   alpha[0x7b] = '-';    
    alpha[0x79] = '+';    alpha[0x6d] = '.';   alpha[0x5a] = 13;     alpha[0x71] = ',';

    if(extChar == true)
    {
      alpha[0x4a] = '/';
    }else if(shift == false)
    {
      alpha[0x4a] = ';';
    }else if(shift == true)
    {
      alpha[0x4a] = ':';
    }
  }

  KEY_EVENT[key_event_ptr].key = alpha[key_code];
  KEY_EVENT[key_event_ptr].released = release;

  if(extChar == true)
  {
  
    KEY_EVENT[key_event_ptr].printable = false;
    
  }else
  {
    if(key_code == 0x58 || key_code == 0x12 || key_code == 0x66 || key_code == 0x14 || key_code == 0x1f || 
       key_code == 0x11 || key_code == 0x59 || key_code == 0x14 || key_code == 0x27 || key_code == 0x76 || 
       key_code == 0x05 || key_code == 0x06 || key_code == 0x04 || key_code == 0x0c || key_code == 0x03 ||
       key_code == 0x0b || key_code == 0x83 || key_code == 0x0a || key_code == 0x01 || key_code == 0x09 ||
       key_code == 0x78 || key_code == 0x07 || key_code == 0x7e || key_code == 0x09 || key_code == 0x77 || key_code == 0x5a)
    {
      KEY_EVENT[key_event_ptr].printable = false;
    }else if(numLock == false)
    {
      if(key_code == 0x70 || key_code == 0x69 || key_code == 0x72 || key_code == 0x7a || key_code == 0x6b || 
         key_code == 0x74 || key_code == 0x6c || key_code == 0x75 || key_code == 0x7d || key_code == 0x71)
      {
        KEY_EVENT[key_event_ptr].printable = false;
      }
    }
  }

  return KEY_EVENT[key_event_ptr];
 
}

void kb_ISR()
{
  readCount = 1;
  extChar = false;
  release = false;
  readCountSet = false;
  key_code = 0;

  if(digitalRead(27) == LOW)
  {
    for(int i = 0; i < readCount; i++)
    {
      uint16_t kb_code = 0;

      while(digitalRead(27) == HIGH){};

      kb_code = (digitalRead(28) << 10) | kb_code;

      while(digitalRead(27) == LOW){};
      while(digitalRead(27) == HIGH){};

      kb_code = (digitalRead(28) << 2) | kb_code;

      while(digitalRead(27) == LOW){};
      while(digitalRead(27) == HIGH){};

      kb_code = (digitalRead(28) << 3) | kb_code;

      while(digitalRead(27) == LOW){};
      while(digitalRead(27) == HIGH){};

      kb_code = (digitalRead(28) << 4) | kb_code;

      while(digitalRead(27) == LOW){};
      while(digitalRead(27) == HIGH){};

      kb_code = (digitalRead(28) << 5) | kb_code;

      while(digitalRead(27) == LOW){};
      while(digitalRead(27) == HIGH){};

      kb_code = (digitalRead(28) << 6) | kb_code;

      while(digitalRead(27) == LOW){};
      while(digitalRead(27) == HIGH){};

      kb_code = (digitalRead(28) << 7) | kb_code;

      while(digitalRead(27) == LOW){};
      while(digitalRead(27) == HIGH){};

      kb_code = (digitalRead(28) << 8) | kb_code;

      while(digitalRead(27) == LOW){};
      while(digitalRead(27) == HIGH){};

      kb_code = (digitalRead(28) << 9) | kb_code;

      while(digitalRead(27) == LOW){};
      while(digitalRead(27) == HIGH){};

      kb_code = (digitalRead(28) << 1) | kb_code;

      while(digitalRead(27) == LOW){};
      while(digitalRead(27) == HIGH){};

      kb_code = (digitalRead(28)) | kb_code;

      while(digitalRead(27) == LOW){};
      
      key_code = 0;

      key_code = key_code | (0b00000001 & (kb_code >> 2)); 
      key_code = key_code | (0b00000010 & (kb_code >> 2)); 
      key_code = key_code | (0b00000100 & (kb_code >> 2)); 
      key_code = key_code | (0b00001000 & (kb_code >> 2)); 
      key_code = key_code | (0b00010000 & (kb_code >> 2)); 
      key_code = key_code | (0b00100000 & (kb_code >> 2)); 
      key_code = key_code | (0b01000000 & (kb_code >> 2)); 
      key_code = key_code | (0b10000000 & (kb_code >> 2)); 

      if(key_code == 0xE0 && readCountSet == false)
      {
        readCount = 2;

        extChar = true;
      }else if(key_code == 0xF0 && readCountSet == false)
      {
        if(extChar == true)
        {
          readCount = 3;
        }else
        {
          readCount = 2;
        }
        release = true;
        readCountSet = true;
      }else if(readCountSet == false)
      {
        readCount = 1;
        readCountSet = true;
      }

      if(key_code != 0XF0 && key_code != 0XE0)
      {

        for(int i = 0; i < 20; i++)
        {
          if(KEY_EVENT[i].processed == true)
          {
            key_event_ptr = i;
            break;
          }
        }

        KEY_EVENT[key_event_ptr] = returnKBEvent(caps, shift, extChar, release, key_code); 
        KB_RX = returnKBEvent(caps, shift, extChar, release, key_code); 
        KB_RX.processed = false;

        char c = KEY_EVENT[key_event_ptr].key;

        KEY_CHAR = c;
        KEY_EVENT[key_event_ptr].processed = false;

        for(int i = 0; i < 20; i++)
        {
          if(KEY_EVENT[i].key == 20014 && KEY_EVENT[i].released == false && KEY_EVENT[i].processed == false)
          {
            run_program = false;
          }
        }
    
        release = false;
        extChar = false;
      }
    }
  }
}