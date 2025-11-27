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
#include "bytecode_interpreter.h"
#include "terminal.h"
#include <functional>

struct key_event KB_RX;

typedef struct
{
  union
  {
    int32_t i;
    float f;
    bool b;
  }value;

}reg;

reg opreg[opreg_size];
var memory[memory_size];

const int program_mem_size = 15000;
uint8_t program[program_mem_size];

uint8_t memVarType[memory_size];
uint8_t regVarType[opreg_size];

bool runFromRam = false;
bool run_program = true;
int JMP = 0;
int prog_size = 0;

void load_bytecode(String bin_file)
{
  prog_size = 0;

  for(int i = 0; i < program_mem_size; i++)
  {
    program[i] = 0;
  }

  File sdFile = SD.open(bin_file);

  Serial.print("\nLoaded code\n");

  echo_message("=>LOADING BIN PROGRAM \"" + bin_file + "\"");

  if(runFromRam == true)
  {
    int counter = 0;
    while(sdFile.available())
    {
      program[counter] = sdFile.read();
      counter++;
    }
    sdFile.seek(0);
  }

  prog_size = 0;
  while(sdFile.available())
  {
    sdFile.read();
    prog_size++;
  }
  sdFile.seek(0);

  Serial.print("\nPROG_SIZE = ");
  Serial.print(prog_size);

  echo_message("=>PROG USES " + String(prog_size) + "/" + String(program_mem_size) + " B OF INSTR MEM.");
  echo_message("");

}

static int prog_address_counter = 0;

__attribute__((section(".itcm"))) void run_bytecode()
{
  prog_address_counter = 0;
  run_program = true;
  KB_RX.processed = true;
  
  for(int i = 0; i < opreg_size; i++)
  {
    opreg[i].value.i = 0;
  }

  for(int i = 0; i < memory_size; i++)//CLEAR MEMORY?
  {
    //memory[i].value.i = 0;
  }

  int ms = millis();
  
  int OP;

  static void *dispatch_table[] = 
  {
    &&OP_STORE,
    &&OP_LOAD,
    &&OP_MOV,
    &&OP_MOVI,
    &&OP_MOVF,
    &&OP_ADD,
    &&OP_SUB,
    &&OP_MUL,
    &&OP_DIV,
    &&OP_CMP,
    &&OP_JMP,
    &&OP_JMPZ,
    &&OP_JMPNZ,
    &&OP_DRWBUFF,
    &&OP_DRWPXL,
    &&OP_KBRX,
    &&OP_HALT,
    &&OP_DRWLN
  };

  if(runFromRam == true)
  {

    uint8_t op = program[prog_address_counter++];

    goto *dispatch_table[op];

    while(true)
    {

      OP_STORE://STORE INSTRUCTION
      {

        //READ NEXT TWO BYTES = DESTINATION MEMORY ADDRESS
        uint16_t dest_mem_addr = (program[prog_address_counter] << 8) | program[prog_address_counter + 1];
        prog_address_counter += 2;
        //READ NEXT TWO BYTES = DESTINATION MEMORY ADDRESS

        //READ NEXT TWO BYTES = REGISTER ADDRESS
        uint16_t reg_addr = (program[prog_address_counter] << 8) | program[prog_address_counter + 1];
        prog_address_counter += 2;
        //READ NEXT TWO BYTES = REGISTER ADDRESS

        if(dest_mem_addr >= memory_size || reg_addr >= opreg_size)
        {
          return;
        }

        memVarType[dest_mem_addr] = regVarType[reg_addr];

        if(regVarType[reg_addr] == 0)
        {
          memory[dest_mem_addr].value.i = opreg[reg_addr].value.i;
        }else if(regVarType[reg_addr] == 1)
        {
          memory[dest_mem_addr].value.f = opreg[reg_addr].value.f;
        }else if(regVarType[reg_addr] == 2)
        {
          memory[dest_mem_addr].value.b = opreg[reg_addr].value.b;
        }
          
        Serial.print("\nSTORE");

        Serial.print(" ");
        Serial.print(dest_mem_addr);

        Serial.print(" ");
        Serial.print(reg_addr);
        Serial.print(";\n");

        op = program[prog_address_counter++];
        goto *dispatch_table[op];

      }

      OP_LOAD://LOAD INSTRUCTION
      {

        //READ NEXT TWO BYTES = destination_register_address
        uint16_t dest_reg_addr = (program[prog_address_counter] << 8) | program[prog_address_counter + 1];
        prog_address_counter += 2;
        //READ NEXT TWO BYTES = destination_register_address

        //READ NEXT TWO BYTES = memory_address
        uint16_t mem_addr = (program[prog_address_counter] << 8) | program[prog_address_counter + 1];
        prog_address_counter += 2;
        //READ NEXT TWO BYTES = memory_address

        if(dest_reg_addr >= opreg_size || mem_addr >= memory_size)
        {
          return;
        }
          
        regVarType[dest_reg_addr] = memVarType[mem_addr];

        if(memVarType[mem_addr] == 0)
        {
          opreg[dest_reg_addr].value.i = memory[mem_addr].value.i;
        }else if(memVarType[mem_addr] == 1)
        {
          opreg[dest_reg_addr].value.f = memory[mem_addr].value.f;
        }else if(memVarType[mem_addr] == 2)
        {
          opreg[dest_reg_addr].value.b = memory[mem_addr].value.b;
        }
          
        Serial.print("\nLOAD");

        Serial.print(" ");
        Serial.print(dest_reg_addr);

        Serial.print(" ");
        Serial.print(mem_addr);
        Serial.print(";\n");

        op = program[prog_address_counter++];
        goto *dispatch_table[op];

      }

      OP_MOV://MOV INSTRUCTION
      {
        //READ NEXT TWO BYTES = COPY REGISTER
        uint16_t copy_reg_addr = (program[prog_address_counter] << 8) | program[prog_address_counter + 1];
        prog_address_counter += 2;
        //READ NEXT TWO BYTES = COPY REGISTER

        //READ NEXT TWO BYTES = PASTE REGISTER
        uint16_t paste_reg_addr = (program[prog_address_counter] << 8) | program[prog_address_counter + 1];
        prog_address_counter += 2;
        //READ NEXT TWO BYTES = PASTE REGISTER

        if(copy_reg_addr >= opreg_size || paste_reg_addr >= opreg_size)
        {
          return;
        }

        regVarType[paste_reg_addr] = regVarType[copy_reg_addr];

        if(regVarType[copy_reg_addr] == 0)
        {
          opreg[paste_reg_addr].value.i = opreg[copy_reg_addr].value.i;
        }else if(regVarType[copy_reg_addr] == 1)
        {
          opreg[paste_reg_addr].value.f = opreg[copy_reg_addr].value.f;
        }else if(regVarType[copy_reg_addr] == 2)
        {
          opreg[paste_reg_addr].value.b = opreg[copy_reg_addr].value.b;
        }

        op = program[prog_address_counter++];
        goto *dispatch_table[op];
        
      }
      
      OP_MOVI://MOVI INSTRUCTION
      {
        //READ NEXT TWO BYTES = PASTE REGISTER
        uint16_t paste_reg_addr = (program[prog_address_counter] << 8) | program[prog_address_counter + 1];
        prog_address_counter += 2;
        //READ NEXT TWO BYTES = PASTE REGISTER

        //READ NEXT 4 BYTES = CONST INT VALUE
        int32_t constint = ((int32_t)program[prog_address_counter] << 24) | ((int32_t)program[prog_address_counter+1] << 16) | ((int32_t)program[prog_address_counter+2] << 8) | ((int32_t)program[prog_address_counter+3]);
        prog_address_counter += 4;
        //READ NEXT 4 BYTES = CONST INT VALUE

        if(paste_reg_addr >= opreg_size)
        {
          return;
        }

        regVarType[paste_reg_addr] = 0;
        opreg[paste_reg_addr].value.i = constint;
        
        /*Serial.print("\nMOVI");
        Serial.print(" ");
        Serial.print(paste_register_address);
        Serial.print(" ");
        Serial.print(constint);
        Serial.print(";\n");*/

        op = program[prog_address_counter++];
        goto *dispatch_table[op];

      }
      
      OP_MOVF://MOVF INSTRUCTION
      {
        //READ NEXT TWO BYTES = PASTE REGISTER
        uint16_t paste_reg_addr = (program[prog_address_counter] << 8) | program[prog_address_counter + 1];
        prog_address_counter += 2;
        //READ NEXT TWO BYTES = PASTE REGISTER

        //READ NEXT 4 BYTES = FLOAT VALUE
        float constfloat = 0;
        uint8_t floatBytes[4];

        floatBytes[0] = program[prog_address_counter];
        prog_address_counter++;

        floatBytes[1] = program[prog_address_counter];
        prog_address_counter++;

        floatBytes[2] = program[prog_address_counter];
        prog_address_counter++;

        floatBytes[3] = program[prog_address_counter];
        prog_address_counter++;

        memcpy(&constfloat, floatBytes, sizeof(constfloat));
        //READ NEXT 4 BYTES = FLOAT VALUE

        if(paste_reg_addr >= opreg_size)
        {
          return;
        }

        regVarType[paste_reg_addr] = 1;
        opreg[paste_reg_addr].value.f = constfloat;

        op = program[prog_address_counter++];
        goto *dispatch_table[op];

      }
      
      OP_ADD://ADD INSTRUCTION
      {
        //READ NEXT TWO BYTES = OPREG1 ADDRESS
        uint16_t opreg1_address = (program[prog_address_counter] << 8) | program[prog_address_counter + 1];
        prog_address_counter += 2;
        //READ NEXT TWO BYTES = OPREG1 ADDRESS

        //READ NEXT TWO BYTES = OPREG2 ADDRESS
        uint16_t opreg2_address = (program[prog_address_counter] << 8) | program[prog_address_counter + 1];
        prog_address_counter += 2;
        //READ NEXT TWO BYTES = OPREG2 ADDRESS

        //READ NEXT TWO BYTES = STORE ADDRESS
        uint16_t store_address = (program[prog_address_counter] << 8) | program[prog_address_counter + 1];
        prog_address_counter += 2;
        //READ NEXT TWO BYTES = STORE ADDRESS

        if (opreg1_address >= opreg_size || opreg2_address >= opreg_size || store_address >= opreg_size)
        {
          return; // or handle error
        }
        
        uint8_t result_type = (regVarType[opreg1_address] == 1 || regVarType[opreg1_address] == 1) ? 1 : 0;
        regVarType[store_address] = result_type;

        if(result_type == 1)
        {
          float n1 = (regVarType[opreg1_address] == 1) ? opreg[opreg1_address].value.f : (float)opreg[opreg1_address].value.i;
          float n2 = (regVarType[opreg2_address] == 1) ? opreg[opreg2_address].value.f : (float)opreg[opreg2_address].value.i;

          opreg[store_address].value.f = n1 + n2;
        }else if(result_type == 0)
        {
          opreg[store_address].value.i = opreg[opreg1_address].value.i + opreg[opreg2_address].value.i;
        }
        
        /*Serial.print("\nADD ");
        Serial.print(opreg1_address);
        Serial.print(" ");
        Serial.print(opreg2_address);
        Serial.print(" ");
        Serial.print(store_address);
        Serial.print(";\n");*/

        op = program[prog_address_counter++];
        goto *dispatch_table[op];

      }
      
      OP_SUB://SUB INSTRUCTION
      {
        //READ NEXT TWO BYTES = OPREG1 ADDRESS
        uint16_t opreg1_address = (program[prog_address_counter] << 8) | program[prog_address_counter + 1];
        prog_address_counter += 2;
        //READ NEXT TWO BYTES = OPREG1 ADDRESS

        //READ NEXT TWO BYTES = OPREG2 ADDRESS
        uint16_t opreg2_address = (program[prog_address_counter] << 8) | program[prog_address_counter + 1];
        prog_address_counter += 2;
        //READ NEXT TWO BYTES = OPREG2 ADDRESS

        //READ NEXT TWO BYTES = STORE ADDRESS
        uint16_t store_address = (program[prog_address_counter] << 8) | program[prog_address_counter + 1];
        prog_address_counter += 2;
        //READ NEXT TWO BYTES = STORE ADDRESS

        if (opreg1_address >= opreg_size || opreg2_address >= opreg_size || store_address >= opreg_size)
        {
          return; // or handle error
        }
        
        uint8_t result_type = (regVarType[opreg1_address] == 1 || regVarType[opreg1_address] == 1) ? 1 : 0;
        regVarType[store_address] = result_type;

        if(result_type == 1)
        {
          float n1 = (regVarType[opreg1_address] == 1) ? opreg[opreg1_address].value.f : (float)opreg[opreg1_address].value.i;
          float n2 = (regVarType[opreg2_address] == 1) ? opreg[opreg2_address].value.f : (float)opreg[opreg2_address].value.i;

          opreg[store_address].value.f = n1 - n2;
        }else if(result_type == 0)
        {
          opreg[store_address].value.i = opreg[opreg1_address].value.i - opreg[opreg2_address].value.i;
        }

        //Serial.print("\nSUB;\n");

        op = program[prog_address_counter++];
        goto *dispatch_table[op];

      }
      
      OP_MUL://MUL INSTRUCTION
      {
        //READ NEXT TWO BYTES = OPREG1 ADDRESS
        uint16_t opreg1_address = (program[prog_address_counter] << 8) | program[prog_address_counter + 1];
        prog_address_counter += 2;
        //READ NEXT TWO BYTES = OPREG1 ADDRESS

        //READ NEXT TWO BYTES = OPREG2 ADDRESS
        uint16_t opreg2_address = (program[prog_address_counter] << 8) | program[prog_address_counter + 1];
        prog_address_counter += 2;
        //READ NEXT TWO BYTES = OPREG2 ADDRESS

        //READ NEXT TWO BYTES = STORE ADDRESS
        uint16_t store_address = (program[prog_address_counter] << 8) | program[prog_address_counter + 1];
        prog_address_counter += 2;
        //READ NEXT TWO BYTES = STORE ADDRESS

        if (opreg1_address >= opreg_size || opreg2_address >= opreg_size || store_address >= opreg_size)
        {
          return; // or handle error
        }
        
        uint8_t result_type = (regVarType[opreg1_address] == 1 || regVarType[opreg1_address] == 1) ? 1 : 0;
        regVarType[store_address] = result_type;

        if(result_type == 1)
        {
          float n1 = (regVarType[opreg1_address] == 1) ? opreg[opreg1_address].value.f : (float)opreg[opreg1_address].value.i;
          float n2 = (regVarType[opreg2_address] == 1) ? opreg[opreg2_address].value.f : (float)opreg[opreg2_address].value.i;

          opreg[store_address].value.f = n1 * n2;
        }else if(result_type == 0)
        {
          opreg[store_address].value.i = opreg[opreg1_address].value.i * opreg[opreg2_address].value.i;
        }

        //Serial.print("\nMUL;\n");

        op = program[prog_address_counter++];
        goto *dispatch_table[op];

      }
      
      OP_DIV://DIV INSTRUCTION
      {
        //READ NEXT TWO BYTES = OPREG1 ADDRESS
        uint16_t opreg1_address = (program[prog_address_counter] << 8) | program[prog_address_counter + 1];
        prog_address_counter += 2;
        //READ NEXT TWO BYTES = OPREG1 ADDRESS

        //READ NEXT TWO BYTES = OPREG2 ADDRESS
        uint16_t opreg2_address = (program[prog_address_counter] << 8) | program[prog_address_counter + 1];
        prog_address_counter += 2;
        //READ NEXT TWO BYTES = OPREG2 ADDRESS

        //READ NEXT TWO BYTES = STORE ADDRESS
        uint16_t store_address = (program[prog_address_counter] << 8) | program[prog_address_counter + 1];
        prog_address_counter += 2;
        //READ NEXT TWO BYTES = STORE ADDRESS

        if (opreg1_address >= opreg_size || opreg2_address >= opreg_size || store_address >= opreg_size)
        {
          return; // or handle error
        }
        
        uint8_t result_type = (regVarType[opreg1_address] == 1 || regVarType[opreg1_address] == 1) ? 1 : 0;
        regVarType[store_address] = result_type;

        if(result_type == 1)
        {
          float n1 = (regVarType[opreg1_address] == 1) ? opreg[opreg1_address].value.f : (float)opreg[opreg1_address].value.i;
          float n2 = (regVarType[opreg2_address] == 1) ? opreg[opreg2_address].value.f : (float)opreg[opreg2_address].value.i;

          opreg[store_address].value.f = n1 / n2;
        }else if(result_type == 0)
        {
          opreg[store_address].value.i = opreg[opreg1_address].value.i / opreg[opreg2_address].value.i;
        }

        //Serial.print("\nDIV;\n");

        op = program[prog_address_counter++];
        goto *dispatch_table[op];
      }
      
      OP_CMP://CMP INSTRUCTION
      {
        uint8_t cmp_op;

        cmp_op = program[prog_address_counter];
        prog_address_counter++;

        //READ NEXT TWO BYTES = COMPARE NUMBER ADDRESS 1
        uint16_t cmp_num_addr1 = (program[prog_address_counter] << 8) | program[prog_address_counter + 1];
        prog_address_counter += 2;
        //READ NEXT TWO BYTES = COMPARE NUMBER ADDRESS 1

        //READ NEXT TWO BYTES = COMPARE NUMBER ADDRESS 2
        uint16_t cmp_num_addr2 = (program[prog_address_counter] << 8) | program[prog_address_counter + 1];
        prog_address_counter += 2;
        //READ NEXT TWO BYTES = COMPARE NUMBER ADDRESS 2

        //READ NEXT TWO BYTES = COMPARE RESULT STORE ADDRESS
        uint16_t cmp_num_store_addr = (program[prog_address_counter] << 8) | program[prog_address_counter + 1];
        prog_address_counter += 2;
        //READ NEXT TWO BYTES = COMPARE RESULT STORE ADDRESS

        if(cmp_num_addr1 >= opreg_size || cmp_num_addr2 >= opreg_size || cmp_num_store_addr >= opreg_size)
        {
          return;
        }

        bool cmp_res = false;

        if (regVarType[cmp_num_addr1] == 0 && regVarType[cmp_num_addr2] == 0)
        {
          int32_t v1 = opreg[cmp_num_addr1].value.i;
          int32_t v2 = opreg[cmp_num_addr2].value.i;

          switch (cmp_op)
          {
            case 0: cmp_res = (v1 <  v2); break; // LT
            case 1: cmp_res = (v1 >  v2); break; // GT
            case 2: cmp_res = (v1 == v2); break; // EQ
            case 3: cmp_res = (v1 != v2); break; // NE
            case 4: cmp_res = (v1 >= v2); break; // GE
            case 5: cmp_res = (v1 <= v2); break; // LE
            default: return; //error
          }
        }else
        {
          // promote to float if any operand is float
          float v1 = (regVarType[cmp_num_addr1] == 1) ? opreg[cmp_num_addr1].value.f : (float)opreg[cmp_num_addr1].value.i;
          float v2 = (regVarType[cmp_num_addr2] == 1) ? opreg[cmp_num_addr2].value.f : (float)opreg[cmp_num_addr2].value.i;

          switch (cmp_op)
          {
            case 0: cmp_res = (v1 <  v2); break;
            case 1: cmp_res = (v1 >  v2); break;
            case 2: cmp_res = (v1 == v2); break;
            case 3: cmp_res = (v1 != v2); break;
            case 4: cmp_res = (v1 >= v2); break;
            case 5: cmp_res = (v1 <= v2); break;
            default: return; //error
          }
        }

        regVarType[cmp_num_store_addr] = 2;
        opreg[cmp_num_store_addr].value.b = cmp_res;

        /*Serial.print("\nCMP_LT");
        Serial.print(" ");
        Serial.print(cmp_op);

        Serial.print(" ");
        Serial.print(cmp_num_addr1);

        Serial.print(" ");
        Serial.print(cmp_num_addr2);

        Serial.print(" ");
        Serial.print(cmp_num_store_addr);
        Serial.print(";\n");*/

        op = program[prog_address_counter++];
        goto *dispatch_table[op];

      }
      
      OP_JMP://UNCONDITIONAL JMP INSTRUCTION
      {
        //JMP = JMP + 1;
        //READ NEXT 4 BYTES = PROGRAM JUMP ADDRESS
        uint32_t prog_jmp_addr = ((uint32_t)program[prog_address_counter] << 24) | ((uint32_t)program[prog_address_counter+1] << 16) | ((uint32_t)program[prog_address_counter+2] << 8) | ((uint32_t)program[prog_address_counter+3]);
        prog_address_counter += 4;
        //READ NEXT 4 BYTES = PROGRAM JUMP ADDRESS

        prog_address_counter = prog_jmp_addr;

        /*Serial.print("\nJMP ");
        Serial.print(prog_jmp_addr);
        Serial.print(";\n");*/

        op = program[prog_address_counter++];
        goto *dispatch_table[op];

      }
      
      OP_JMPZ://JMPZ
      {
        //JMP = JMP + 1;
        //READ NEXT TWO BYTES = CONDITION LOOKUP ADDRESS
        uint16_t cond_lookup_addr = (program[prog_address_counter] << 8) | program[prog_address_counter + 1];
        prog_address_counter += 2;
        //READ NEXT TWO BYTES = CONDITION LOOKUP ADDRESS

        //READ NEXT 4 BYTES = PROGRAM JUMP ADDRESS
        uint32_t prog_jmp_addr = ((uint32_t)program[prog_address_counter] << 24) | ((uint32_t)program[prog_address_counter+1] << 16) | ((uint32_t)program[prog_address_counter+2] << 8) | ((uint32_t)program[prog_address_counter+3]);
        prog_address_counter += 4;
        //READ NEXT 4 BYTES = PROGRAM JUMP ADDRESS

        if(cond_lookup_addr >= opreg_size)
        {
          return;
        }

        if(opreg[cond_lookup_addr].value.b == false)
        {

          prog_address_counter = prog_jmp_addr;

          /*Serial.print("\nJMPZ ");
          Serial.print(cond_lookup_addr);
          Serial.print(" ");
          Serial.print(prog_jmp_addr);
          Serial.print(";\n");*/
        }

        op = program[prog_address_counter++];
        goto *dispatch_table[op];
        
      }
      
      OP_JMPNZ://JMPNZ
      {
        //JMP = JMP + 1;
        //READ NEXT TWO BYTES = CONDITION LOOKUP ADDRESS
        uint16_t cond_lookup_addr = (program[prog_address_counter] << 8) | program[prog_address_counter + 1];
        prog_address_counter += 2;
        //READ NEXT TWO BYTES = CONDITION LOOKUP ADDRESS

        //READ NEXT 4 BYTES = PROGRAM JUMP ADDRESS
        uint32_t prog_jmp_addr = ((uint32_t)program[prog_address_counter] << 24) | ((uint32_t)program[prog_address_counter+1] << 16) | ((uint32_t)program[prog_address_counter+2] << 8) | ((uint32_t)program[prog_address_counter+3]);
        prog_address_counter += 4;
        //READ NEXT 4 BYTES = PROGRAM JUMP ADDRESS

        if(cond_lookup_addr >= opreg_size)
        {
          return;
        }

        if(opreg[cond_lookup_addr].value.b == true)
        {
          prog_address_counter = prog_jmp_addr;

          /*Serial.print("\nJMPNZ ");
          Serial.print(cond_lookup_addr);
          Serial.print(" ");
          Serial.print(prog_jmp_addr);
          Serial.print(";\n");*/
        }

        op = program[prog_address_counter++];
        goto *dispatch_table[op];
        
      }
      
      OP_DRWBUFF://DRAWS THE GRAPHICS BUFFER
      {
        draw_half_res_buffer();

        op = program[prog_address_counter++];
        goto *dispatch_table[op];
      }
      
      OP_DRWPXL://COLORS A PIXEL WITH THE VALUE AT THE REGISTER X ADDRESS
      {
        //READ NEXT 2 BYTES = PIXEL ADDRESS
        uint16_t pxl_addr_reg = (program[prog_address_counter] << 8) | program[prog_address_counter + 1];
        prog_address_counter += 2;
        //READ NEXT 2 BYTES = PIXEL ADDRESS

        //READ NEXT 2 BYTES = REGISTER ADDRESS
        uint16_t pxl_color_reg = (program[prog_address_counter] << 8) | program[prog_address_counter + 1];
        prog_address_counter += 2;
        //READ TWO BYTE = REGISTER ADDRESS

        uint32_t colorSlotIndex = opreg[pxl_addr_reg].value.i >> 2;
        uint32_t pixelInSlot = opreg[pxl_addr_reg].value.i & 3; //MODULO OF 4, YES IT IS WEIRD BUT IT IS MODULO OF 4

        static const uint8_t shift_lookup[4] = {0, 8, 16, 24};
        uint8_t shift = shift_lookup[pixelInSlot];


        uint32_t color = opreg[pxl_color_reg].value.i;
        uint32_t mask = 0xFF << shift;
        color = (color << shift) | (memory[vbuff_start_address + colorSlotIndex].value.i & ~mask);
        
        memory[vbuff_start_address + colorSlotIndex].value.i = color;

        op = program[prog_address_counter++];
        goto *dispatch_table[op];
      }

      OP_DRWLN: //DRAWS A LINE BETWEEN POINT 1 AND 2 WITH COLOR Z AND THICKNESS Y ***GOTTA FIX. NOT WORKING.***
      {
        //READ NEXT 2 BYTES = POINT 1 IN BUFFER REG ADDRESS
        uint32_t p1_reg_addr = (program[prog_address_counter] << 16) | (program[prog_address_counter + 1] << 8) | program[prog_address_counter + 2];
        prog_address_counter += 3;
        //READ NEXT 2 BYTES = POINT 1 IN BUFFER REG ADDRESS

        //READ NEXT 2 BYTES = POINT 2 IN BUFFER REG ADDRESS
        uint32_t p2_reg_addr = (program[prog_address_counter] << 16) | (program[prog_address_counter + 1] << 8) | program[prog_address_counter + 2];
        prog_address_counter += 3;
        //READ NEXT 2 BYTES = POINT 2 IN BUFFER REG ADDRESS

        //READ NEXT 1 BYTES = LINE COLOR
        uint8_t ln_color = program[prog_address_counter];
        prog_address_counter += 1;
        //READ NEXT 1 BYTES = LINE COLOR

        //READ NEXT 1 BYTES = THICKNESS
        uint8_t ln_thickness = program[prog_address_counter];
        prog_address_counter += 1;
        //READ NEXT 1 BYTES = THICKNESS


        draw_line_half_res_buffer(p1_reg_addr%320, p1_reg_addr/320, p2_reg_addr%320, p2_reg_addr/320, ln_color, ln_thickness) ;

        op = program[prog_address_counter++];
        goto *dispatch_table[op];
      }
      
      OP_KBRX://GETS INFO FROM KB_RX
      {
        //READ NEXT 2 BYTES = ASCII DATA SAVE ADDRESS
        uint16_t ascii_addr_reg = (program[prog_address_counter] << 8) | program[prog_address_counter + 1];
        prog_address_counter += 2;
        //READ NEXT 2 BYTES = ASCII DATA SAVE ADDRESS

        //READ NEXT 2 BYTES = NEW KEY TO BE PROCESSED BOOL
        uint16_t unprocessed_reg = (program[prog_address_counter] << 8) | program[prog_address_counter + 1];
        prog_address_counter += 2;
        //READ NEXT 2 BYTES = NEW KEY TO BE PROCESSED BOOL

        if(KB_RX.processed == false)
        {
          
          opreg[ascii_addr_reg].value.i = KB_RX.key;

          regVarType[unprocessed_reg] = 2;
          opreg[unprocessed_reg].value.b = true;
          KB_RX.processed = true;
        }

        /*Serial.print("\nKBRX");
        Serial.print(" ");
        Serial.print(ascii_addr_reg);
        Serial.print(" ");
        Serial.print(unprocessed_reg);
        Serial.print(";\n");*/

        op = program[prog_address_counter++];
        goto *dispatch_table[op];
      }

      OP_HALT:
      {
        break;
      }
           
    }
  }else
  {
    
  }

  Serial.print("\nTOOK MS: ");
  Serial.print(millis() - ms);

  echo_message("=>EXECUTION TOOK " + String(millis() - ms) + " MS.");
  echo_message("");

  Serial.print("\nTRUTH TIME: ");

  int final_address = 21;
  if(memVarType[final_address] == 0)
  {
    Serial.print(memory[final_address].value.i);
  }else if(memVarType[final_address] == 1)
  {
    Serial.print(memory[final_address].value.f);
  }else if(memVarType[final_address] == 2)
  {
    Serial.print(memory[final_address].value.b);
  }

  Serial.print("\nTRUTH TIME 2 : ");
  int opreg_address = 0;
  if(regVarType[opreg_address] == 0)
  {
    Serial.print(opreg[opreg_address].value.i);
  }else if(regVarType[opreg_address] == 1)
  {
    Serial.print(opreg[opreg_address].value.f);
  }else if(regVarType[opreg_address] == 2)
  {
    Serial.print(opreg[opreg_address].value.b);
  }

  //Serial.print("\nJMPs : ");
  //Serial.print(JMP);

}

