#include "globals.h"
#include "LEXER.h"

String instr_tokens[100];
static uint8_t tokenTypes[100];
static uint16_t tokenTreeSize = 0;

static uint8_t codeTokenizer(String code)
{
  for(int i = 0; i < tokenTreeSize; i++)
  {
    instr_tokens[i] = "";
  }

  code.trim();
  tokenTreeSize = 0;
  
  int codePtr = 0;
  uint8_t tokenPtr = 0;

  bool openQuotation = false;

  bool alphaIncluded = false;

  while(codePtr < code.length())
  {
    if(code.charAt(codePtr) == ' ' && openQuotation == false || code.charAt(codePtr) == '\n' && openQuotation == false || code.charAt(codePtr) == '\t' && openQuotation == false)
    {
      codePtr++;
    }else if(code.charAt(codePtr) == '\"')
    {
      openQuotation = !openQuotation;

      tokenTreeSize++;
      instr_tokens[tokenPtr] = String(code.charAt(codePtr));
      tokenPtr++;
      codePtr++;
    }else if(openQuotation == true)
    {
      while(code.charAt(codePtr) != '\"' && codePtr < code.length())
      {
        instr_tokens[tokenPtr] += String(code.charAt(codePtr));
        codePtr++;
      }

      tokenTreeSize++;
      tokenPtr++;
    }else if(isValidSymbol(code.charAt(codePtr)))
    {

      tokenTreeSize++;
      instr_tokens[tokenPtr] = String(code.charAt(codePtr));
      tokenPtr++;
      codePtr++;
      
    }else if(isAlpha(code.charAt(codePtr)) || isDigit(code.charAt(codePtr)) || code.charAt(codePtr) == '.' || code.charAt(codePtr) == '_')
    {
      while(isAlpha(code.charAt(codePtr)) || isDigit(code.charAt(codePtr)) || code.charAt(codePtr) == '.' || code.charAt(codePtr) == '_')
      {
        instr_tokens[tokenPtr] += String(code.charAt(codePtr));
        codePtr++;
      }
      
      tokenTreeSize++;
      tokenPtr++;
    }else
    {
      return -1;
    }

  }

  if(openQuotation == true)
  {
    Serial.print("\nCode is fucked because of missing double quotes.");
  }

  return 0;
}

struct label
{
  String name;
  uint32_t addr;
};

static label labels[100];

static bool validNum(String num)
{
  for(int i = 0; i < num.length(); i++)
  {
    if(num.charAt(i) != 0 && num.charAt(i) != 1 && num.charAt(i) != 2 && num.charAt(i) != 3 && num.charAt(i) != 4 && num.charAt(i) != 5 && num.charAt(i) != 6 && num.charAt(i) != 7 && num.charAt(i) != 8 && num.charAt(i) != 9)
    {
      return false;
    }
  }
  return true;
}

void compile_alpha_instructions(String text_file)//compiles bytecode instructions written in text in a .txt file into .bin binary bytecode file executable by the interpreter
{
  File txtFile = SD.open(text_file);

  String bin_file = text_file;
  bin_file.setCharAt(bin_file.length() - 3, 'b');
  bin_file.setCharAt(bin_file.length() - 2, 'i');
  bin_file.setCharAt(bin_file.length() - 1, 'n');

  File binFile = SD.open(bin_file);

  if(binFile == true)
  {
    SD.remove(bin_file);
    Serial.print("\nFile already exists, overwriting...\n");
  }

  binFile = SD.open(bin_file, FILE_WRITE);

  if(txtFile == true)
  {
    Serial.print("\nLoaded text file\n");
  }else
  {
    Serial.print("\nFile does not exist.\n");
  }

  int prog_counter = 0;
  int labelCounter = 0;

  while(txtFile.available())
  {
    String instr;
    char c = ' ';
    while(c != ';' && txtFile.available())
    {
      c = txtFile.read();
      instr.concat(c);
    }

    codeTokenizer(instr);

    if(instr_tokens[0] == "label" && instr_tokens[1] == ":" && tokenTreeSize == 4)
    {
      labels[labelCounter].addr = prog_counter;
      labels[labelCounter].name = instr_tokens[2];

      labelCounter++;
    }else if(instr_tokens[0] == "STORE" && tokenTreeSize == 4)
    {
      prog_counter = prog_counter + 5;
      binFile.write((uint8_t)0);

      uint16_t mem_dest = instr_tokens[1].toInt();
      uint16_t reg_addr = instr_tokens[2].toInt();

      for (int i = sizeof(mem_dest) - 1; i >= 0; i--) 
      {
        binFile.write(((uint8_t*)&mem_dest)[i]);
      }
      
      for (int i = sizeof(reg_addr) - 1; i >= 0; i--) 
      {
        binFile.write(((uint8_t*)&reg_addr)[i]);
      }
    }else if(instr_tokens[0] == "LOAD" && tokenTreeSize == 4)
    {
      prog_counter = prog_counter + 5;
      binFile.write((uint8_t)1);

      uint16_t reg_dest = instr_tokens[1].toInt();
      uint16_t mem_addr = instr_tokens[2].toInt();

      for (int i = sizeof(reg_dest) - 1; i >= 0; i--) 
      {
        binFile.write(((uint8_t*)&reg_dest)[i]);
      }
      
      for (int i = sizeof(mem_addr) - 1; i >= 0; i--) 
      {
        binFile.write(((uint8_t*)&mem_addr)[i]);
      }
    }else if(instr_tokens[0] == "MOV" && tokenTreeSize == 4)
    {
      prog_counter = prog_counter + 5;
      binFile.write((uint8_t)2);

      uint16_t dest_reg = instr_tokens[1].toInt();
      uint16_t copy_reg = instr_tokens[2].toInt();

      for (int i = sizeof(dest_reg) - 1; i >= 0; i--) 
      {
        binFile.write(((uint8_t*)&dest_reg)[i]);
      }
      
      for (int i = sizeof(copy_reg) - 1; i >= 0; i--) 
      {
        binFile.write(((uint8_t*)&copy_reg)[i]);
      }
    }else if(instr_tokens[0] == "MOVI" && tokenTreeSize == 4)
    {
      prog_counter = prog_counter + 7;
      binFile.write((uint8_t)3);

      uint16_t dest_reg = instr_tokens[1].toInt();
      int32_t const_int = instr_tokens[2].toInt();

      for (int i = sizeof(dest_reg) - 1; i >= 0; i--) 
      {
        binFile.write(((uint8_t*)&dest_reg)[i]);
      }
      
      for (int i = sizeof(const_int) - 1; i >= 0; i--) 
      {
        binFile.write(((uint8_t*)&const_int)[i]);
      }
    }else if(instr_tokens[0] == "MOVF" && tokenTreeSize == 4)
    {
      prog_counter = prog_counter + 7;
      binFile.write((uint8_t)4);

      uint16_t dest_reg = instr_tokens[1].toInt();
      float const_float = instr_tokens[2].toInt();

      for (int i = sizeof(dest_reg) - 1; i >= 0; i--) 
      {
        binFile.write(((uint8_t*)&dest_reg)[i]);
      }
      
      for (int i = sizeof(const_float) - 1; i >= 0; i--) 
      {
        binFile.write(((uint8_t*)&const_float)[i]);
      }
    }else if(instr_tokens[0] == "ADD" && tokenTreeSize == 5)
    {
      prog_counter = prog_counter + 7;
      binFile.write((uint8_t)5);

      uint16_t regx_addr = instr_tokens[1].toInt();
      uint16_t regy_addr = instr_tokens[2].toInt();

      uint16_t reg_store_addr = instr_tokens[3].toInt();

      for (int i = sizeof(regx_addr) - 1; i >= 0; i--) 
      {
        binFile.write(((uint8_t*)&regx_addr)[i]);
      }
      
      for (int i = sizeof(regy_addr) - 1; i >= 0; i--) 
      {
        binFile.write(((uint8_t*)&regy_addr)[i]);
      }

      for (int i = sizeof(reg_store_addr) - 1; i >= 0; i--) 
      {
        binFile.write(((uint8_t*)&reg_store_addr)[i]);
      }
    }else if(instr_tokens[0] == "SUB" && tokenTreeSize == 5)
    {
      prog_counter = prog_counter + 7;
      binFile.write((uint8_t)6);

      uint16_t regx_addr = instr_tokens[1].toInt();
      uint16_t regy_addr = instr_tokens[2].toInt();

      uint16_t reg_store_addr = instr_tokens[3].toInt();

      for (int i = sizeof(regx_addr) - 1; i >= 0; i--) 
      {
        binFile.write(((uint8_t*)&regx_addr)[i]);
      }
      
      for (int i = sizeof(regy_addr) - 1; i >= 0; i--) 
      {
        binFile.write(((uint8_t*)&regy_addr)[i]);
      }

      for (int i = sizeof(reg_store_addr) - 1; i >= 0; i--) 
      {
        binFile.write(((uint8_t*)&reg_store_addr)[i]);
      }
    }else if(instr_tokens[0] == "MUL" && tokenTreeSize == 5)
    {
      prog_counter = prog_counter + 7;
      binFile.write((uint8_t)7);

      uint16_t regx_addr = instr_tokens[1].toInt();
      uint16_t regy_addr = instr_tokens[2].toInt();

      uint16_t reg_store_addr = instr_tokens[3].toInt();

      for (int i = sizeof(regx_addr) - 1; i >= 0; i--) 
      {
        binFile.write(((uint8_t*)&regx_addr)[i]);
      }
      
      for (int i = sizeof(regy_addr) - 1; i >= 0; i--) 
      {
        binFile.write(((uint8_t*)&regy_addr)[i]);
      }

      for (int i = sizeof(reg_store_addr) - 1; i >= 0; i--) 
      {
        binFile.write(((uint8_t*)&reg_store_addr)[i]);
      }
    }else if(instr_tokens[0] == "DIV" && tokenTreeSize == 5)
    {
      prog_counter = prog_counter + 7;
      binFile.write((uint8_t)8);

      uint16_t regx_addr = instr_tokens[1].toInt();
      uint16_t regy_addr = instr_tokens[2].toInt();

      uint16_t reg_store_addr = instr_tokens[3].toInt();

      for (int i = sizeof(regx_addr) - 1; i >= 0; i--) 
      {
        binFile.write(((uint8_t*)&regx_addr)[i]);
      }
      
      for (int i = sizeof(regy_addr) - 1; i >= 0; i--) 
      {
        binFile.write(((uint8_t*)&regy_addr)[i]);
      }

      for (int i = sizeof(reg_store_addr) - 1; i >= 0; i--) 
      {
        binFile.write(((uint8_t*)&reg_store_addr)[i]);
      }
    }else if(instr_tokens[0] == "CMP" && tokenTreeSize == 6)
    {
      prog_counter = prog_counter + 8;
      binFile.write((uint8_t)9);

      uint8_t mode = instr_tokens[1].toInt();

      uint16_t regx_addr = instr_tokens[2].toInt();
      uint16_t regy_addr = instr_tokens[3].toInt();

      uint16_t reg_store_addr = instr_tokens[4].toInt();

      for (int i = sizeof(mode) - 1; i >= 0; i--) 
      {
        binFile.write(((uint8_t*)&mode)[i]);
      }
      
      for (int i = sizeof(regx_addr) - 1; i >= 0; i--) 
      {
        binFile.write(((uint8_t*)&regx_addr)[i]);
      }
      
      for (int i = sizeof(regy_addr) - 1; i >= 0; i--) 
      {
        binFile.write(((uint8_t*)&regy_addr)[i]);
      }

      for (int i = sizeof(reg_store_addr) - 1; i >= 0; i--) 
      {
        binFile.write(((uint8_t*)&reg_store_addr)[i]);
      }
    }else if(instr_tokens[0] == "JMP" && tokenTreeSize == 3)
    {
      prog_counter = prog_counter + 5;
      binFile.write((uint8_t)10);

      uint32_t jmp_addr;
      if(validNum(instr_tokens[1]) == true)
      {
        jmp_addr = instr_tokens[1].toInt();
      }else
      {
        for(int i = 0; i < 100; i++)
        {
          if(labels[i].name == instr_tokens[1])
          {
            jmp_addr = labels[i].addr;
          }
        }
        
      }

      for (int i = sizeof(jmp_addr) - 1; i >= 0; i--) 
      {
        binFile.write(((uint8_t*)&jmp_addr)[i]);
      }
      
    }else if(instr_tokens[0] == "JMPZ" && tokenTreeSize == 4)
    {
      prog_counter = prog_counter + 7;
      binFile.write((uint8_t)11);

      uint16_t dest_reg = instr_tokens[1].toInt();
      uint32_t jmp_addr;

      if(validNum(instr_tokens[2]) == true)
      {
        jmp_addr = instr_tokens[2].toInt();
      }else
      {
        for(int i = 0; i < 100; i++)
        {
          if(labels[i].name == instr_tokens[2])
          {
            jmp_addr = labels[i].addr;
          }
        }
      }

      for (int i = sizeof(dest_reg) - 1; i >= 0; i--) 
      {
        binFile.write(((uint8_t*)&dest_reg)[i]);
      }

      for (int i = sizeof(jmp_addr) - 1; i >= 0; i--) 
      {
        binFile.write(((uint8_t*)&jmp_addr)[i]);
      }
    }else if(instr_tokens[0] == "JMPNZ" && tokenTreeSize == 4)
    {
      prog_counter = prog_counter + 7;
      binFile.write((uint8_t)12);

      uint16_t dest_reg = instr_tokens[1].toInt();
      uint32_t jmp_addr;

      if(validNum(instr_tokens[2]) == true)
      {
        jmp_addr = instr_tokens[2].toInt();
      }else
      {
        for(int i = 0; i < 100; i++)
        {
          if(labels[i].name == instr_tokens[2])
          {
            jmp_addr = labels[i].addr;
          }
        }
        
      }

      for (int i = sizeof(dest_reg) - 1; i >= 0; i--) 
      {
        binFile.write(((uint8_t*)&dest_reg)[i]);
      }

      for (int i = sizeof(jmp_addr) - 1; i >= 0; i--) 
      {
        binFile.write(((uint8_t*)&jmp_addr)[i]);
      }
    }else if(instr_tokens[0] == "DRWBUFF" && tokenTreeSize == 2)
    {
      prog_counter = prog_counter + 1;
      binFile.write((uint8_t)13);
    }else if(instr_tokens[0] == "DRWPXL" && tokenTreeSize == 4)
    {
      prog_counter = prog_counter + 5;
      binFile.write((uint8_t)14);

      uint16_t pxl_addr = instr_tokens[1].toInt();
      uint16_t pxl_color = instr_tokens[2].toInt();

      for (int i = sizeof(pxl_addr) - 1; i >= 0; i--) 
      {
        binFile.write(((uint8_t*)&pxl_addr)[i]);
      }
      
      for (int i = sizeof(pxl_color) - 1; i >= 0; i--) 
      {
        binFile.write(((uint8_t*)&pxl_color)[i]);
      }
    }else if(instr_tokens[0] == "KBRX" && tokenTreeSize == 4)
    {
      prog_counter = prog_counter + 5;
      binFile.write((uint8_t)15);

      uint16_t key_ascii = instr_tokens[1].toInt();
      uint16_t new_flag = instr_tokens[2].toInt();

      for (int i = sizeof(key_ascii) - 1; i >= 0; i--) 
      {
        binFile.write(((uint8_t*)&key_ascii)[i]);
      }
      
      for (int i = sizeof(new_flag) - 1; i >= 0; i--) 
      {
        binFile.write(((uint8_t*)&new_flag)[i]);
      }
    }else if(instr_tokens[0] == "HALT" && tokenTreeSize == 2)
    {
      prog_counter = prog_counter + 1;
      binFile.write((uint8_t)16);
      break;
    }else if(instr_tokens[0] == "DRWLN" && tokenTreeSize == 8)//GOTTA FIX. NOT WORKING.
    {
      prog_counter = prog_counter + 7;
      binFile.write((uint8_t)17);

      uint16_t p1x = instr_tokens[1].toInt();
      uint16_t p1y = instr_tokens[2].toInt();
      uint32_t X = (p1y*320) + p1x;

      uint16_t p2x = instr_tokens[3].toInt();
      uint16_t p2y = instr_tokens[4].toInt();
      uint32_t Y = (p2y*320) + p2x;

      uint8_t color = instr_tokens[5].toInt();

      uint8_t thickness = instr_tokens[6].toInt();

      for (int i = sizeof(X) - 2; i >= 0; i--) 
      {
        binFile.write(((uint8_t*)&X)[i]);
      }

      for (int i = sizeof(Y) - 2; i >= 0; i--) 
      {
        binFile.write(((uint8_t*)&Y)[i]);
      }

      for (int i = sizeof(color) - 1; i >= 0; i--) 
      {
        binFile.write(((uint8_t*)&color)[i]);
      }

      for (int i = sizeof(thickness) - 1; i >= 0; i--) 
      {
        binFile.write(((uint8_t*)&thickness)[i]);
      }

    }else
    {
      Serial.print("\nERROR NO OPERATION: " + instr_tokens[0]);
      break;
    }

  }

  txtFile.close();
  binFile.close();
}

void compile_high_level_code(String text_file)
{

}