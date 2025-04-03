#include "mms.h"
#include "globals.h"

#include "pico/stdlib.h"
#include "pico/platform.h"
#include "Arduino.h"

uint8_t vga_data_array[TXCOUNT];
uint8_t * address_pointer = &vga_data_array[0] ;

uint8_t INTERNAL_RAM[TOTAL_INTERNAL_RAM];  // LARGEST VARIABLE

uint32_t DOUBLE_START_ADDRESS = 0;
uint32_t DOUBLE_END_ADDRESS = 0;

uint32_t INT64_START_ADDRESS = 1;
uint32_t INT64_END_ADDRESS = 1;

uint32_t UINT64_START_ADDRESS = 2;
uint32_t UINT64_END_ADDRESS = 2;

uint32_t INT32_START_ADDRESS = 3;
uint32_t INT32_END_ADDRESS = 3;

uint32_t UINT32_START_ADDRESS = 4;
uint32_t UINT32_END_ADDRESS = 4;

uint32_t INT16_START_ADDRESS = 5;
uint32_t INT16_END_ADDRESS = 5;

uint32_t UINT16_START_ADDRESS = 6;
uint32_t UINT16_END_ADDRESS = 6;

uint32_t INT8_START_ADDRESS = 7;
uint32_t INT8_END_ADDRESS = 7;

uint32_t UINT8_START_ADDRESS = 8;
uint32_t UINT8_END_ADDRESS = 8;

uint32_t BOOL_START_ADDRESS = 9;
uint32_t BOOL_END_ADDRESS = 9;

uint32_t CHAR_START_ADDRESS = 10;
uint32_t CHAR_END_ADDRESS = 10;

uint32_t ARRAY_DATA_START_ADDRESS = TOTAL_INTERNAL_RAM - 1;
uint32_t ARRAY_DATA_END_ADDRESS = TOTAL_INTERNAL_RAM - 1;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////      V V             MEMORY  MANAGEMENT SYSTEM      V V       ///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//----------------------------GENERAL VARIABLE FUNCTIONS------------------------------------------------------------------------------------------------------------------------

int32_t GET_ARRAY_INFO(String ARRAY_NAME, String ARRAY_VARTYPE, uint8_t INFO_TYPE)// THIS FUNCTION WILL READ ARRAY INFO AND RETURN START ADDRESS, END ADDRESS OR THE BEGINNING ADDRESS OF THE DATA OF THIS ARRAY. WILL RETURN -1 IF NO ARRAY WITH SUCH NAME IS FOUND. 
{
  ARRAY_NAME.trim();

  uint32_t VARTYPE_START_ADDRESS;
  uint32_t VARTYPE_END_ADDRESS;

  if(ARRAY_VARTYPE == "DOUBLE")
  {
    VARTYPE_START_ADDRESS = DOUBLE_START_ADDRESS;
    VARTYPE_END_ADDRESS = DOUBLE_END_ADDRESS;
  }else if(ARRAY_VARTYPE == "INT64")
  {
    VARTYPE_START_ADDRESS = INT64_START_ADDRESS;
    VARTYPE_END_ADDRESS = INT64_END_ADDRESS;
  }else if(ARRAY_VARTYPE == "UINT64")
  {
    VARTYPE_START_ADDRESS = UINT64_START_ADDRESS;
    VARTYPE_END_ADDRESS = UINT64_END_ADDRESS;
  }else if(ARRAY_VARTYPE == "INT32")
  {
    VARTYPE_START_ADDRESS = INT32_START_ADDRESS;
    VARTYPE_END_ADDRESS = INT32_END_ADDRESS;
  }else if(ARRAY_VARTYPE == "UINT32")
  {
    VARTYPE_START_ADDRESS = UINT32_START_ADDRESS;
    VARTYPE_END_ADDRESS = UINT32_END_ADDRESS;
  }else if(ARRAY_VARTYPE == "INT16")
  {
    VARTYPE_START_ADDRESS = INT16_START_ADDRESS;
    VARTYPE_END_ADDRESS = INT16_END_ADDRESS;
  }else if(ARRAY_VARTYPE == "UINT16")
  {
    VARTYPE_START_ADDRESS = UINT16_START_ADDRESS;
    VARTYPE_END_ADDRESS = UINT16_END_ADDRESS;
  }else if(ARRAY_VARTYPE == "INT8")
  {
    VARTYPE_START_ADDRESS = INT8_START_ADDRESS;
    VARTYPE_END_ADDRESS = INT8_END_ADDRESS;
  }else if(ARRAY_VARTYPE == "UINT8")
  {
    VARTYPE_START_ADDRESS = UINT8_START_ADDRESS;
    VARTYPE_END_ADDRESS = UINT8_END_ADDRESS;
  }else if(ARRAY_VARTYPE == "BOOL")
  {
    VARTYPE_START_ADDRESS = BOOL_START_ADDRESS;
    VARTYPE_END_ADDRESS = BOOL_END_ADDRESS;
  }else if(ARRAY_VARTYPE == "CHAR")
  {
    VARTYPE_START_ADDRESS = CHAR_START_ADDRESS;
    VARTYPE_END_ADDRESS = CHAR_END_ADDRESS;
  }

  if(ARRAY_NAME.length() != 8)
  {
    return -1;
  }

  if(INFO_TYPE > 2 || INFO_TYPE < 0)
  {
    return -1;
  }

  for(int i = 0; i < (ARRAY_DATA_END_ADDRESS - ARRAY_DATA_START_ADDRESS) / 16; i++)
  {
    for(int j = 0; j < 8; j++)
    {
      if(INTERNAL_RAM[(ARRAY_DATA_START_ADDRESS + j) + (i * 16)] != ARRAY_NAME.charAt(j))
      {
        break;
      }

      if(j == 7 && INFO_TYPE == 0)
      {
        uint32_t value = 0;
        uint32_t n = 1;
        for (int k = 0; k < 4; k++) 
        {
          uint8_t valueByte = INTERNAL_RAM[ARRAY_DATA_START_ADDRESS + (i * 16) + 11 - k];

          for (int l = 0; l < 8; l++) 
          {
            uint8_t mask = 1 << l;
            if (mask & valueByte) 
            {
              value = value + n;
            }

            n = n * 2;
          }
        }

        if(value >= VARTYPE_START_ADDRESS && value <= VARTYPE_END_ADDRESS || ARRAY_VARTYPE == "NULL")
        {
          return value;
        }
      }

      if(j == 7 && INFO_TYPE == 1)
      {
        uint32_t value = 0;
        uint32_t n = 1;
        for (int k = 0; k < 4; k++) 
        {
          uint8_t valueByte = INTERNAL_RAM[ARRAY_DATA_START_ADDRESS + (i * 16) + 15 - k];

          for (int l = 0; l < 8; l++) 
          {
            uint8_t mask = 1 << l;
            if (mask & valueByte) 
            {
              value = value + n;
            }

            n = n * 2;
          }
        }

        if(value >= VARTYPE_START_ADDRESS && value <= VARTYPE_END_ADDRESS || ARRAY_VARTYPE == "NULL")
        {
          return value;
        }
      }

      if(j == 7 && INFO_TYPE == 2)
      {
        return ARRAY_DATA_START_ADDRESS + (i*16);
      }
    }
  }

  return -1;
}

int32_t CHECK_VARIABLE_EXISTS(String VARNAME, String VARTYPE)  //WILL CHECK IF VARIABLE WITH PROVIDED NAME EXISTS IN THE SPECIFIED VARIABLE TYPE MEMORY SECTION. IF IT DOES, RETURNS VARIABLE ADDRESS. IF NOT, RETURNS -1. IF VARTYPE IS SET TO NULL, WILL RETURN THE VARIABLE--
{                                                              //ADDRESS IF IT  EXISTS IN ANY OF THE VARIABLE TYPE MEMORY SECTIONS.
  VARNAME.trim();
  VARTYPE.trim();

  while(VARNAME.length() < 8)
  {
    VARNAME += "_";
  }

  uint32_t SEARCH_BEGIN_ADDRESS;
  uint32_t SEARCH_END_ADDRESS;

  if (UINT64_END_ADDRESS - (UINT64_START_ADDRESS + 1) >= INT64_SIZE - 1 && VARTYPE == "UINT64" || UINT64_END_ADDRESS - (UINT64_START_ADDRESS + 1) >= INT64_SIZE - 1 && VARTYPE == "NULL") 
  {
    for (int i = 0; i < (UINT64_END_ADDRESS - (UINT64_START_ADDRESS + 1) + 1) / INT64_SIZE; i++) 
    {
      for (int j = 0; j < 8; j++) 
      {
        if (INTERNAL_RAM[(UINT64_START_ADDRESS + 1 + j) + (i * INT64_SIZE)] != VARNAME.charAt(j)) 
        {
          break;
        }

        if (j == 7) 
        {
          uint32_t VAR_ADDRESS = (UINT64_START_ADDRESS + 1) + (i * INT64_SIZE);

          
          return VAR_ADDRESS;
          
        }
      }
    }
  }

  if (INT64_END_ADDRESS - (INT64_START_ADDRESS + 1) >= INT64_SIZE - 1 && VARTYPE == "INT64" || INT64_END_ADDRESS - (INT64_START_ADDRESS + 1) >= INT64_SIZE - 1 && VARTYPE == "NULL") {
    for (int i = 0; i < (INT64_END_ADDRESS - (INT64_START_ADDRESS + 1) + 1) / INT64_SIZE; i++) {
      for (int j = 0; j < 8; j++) {
        if (INTERNAL_RAM[(INT64_START_ADDRESS + 1 + j) + (i * INT64_SIZE)] != VARNAME.charAt(j)) {
          break;
        }

        if (j == 7) {
          uint32_t VAR_ADDRESS = (INT64_START_ADDRESS + 1) + (i * INT64_SIZE);

          return VAR_ADDRESS;
        }
      }
    }
  }

  if (DOUBLE_END_ADDRESS - (DOUBLE_START_ADDRESS + 1) >= DOUBLE_SIZE - 1 && VARTYPE == "DOUBLE" || DOUBLE_END_ADDRESS - (DOUBLE_START_ADDRESS + 1) >= DOUBLE_SIZE - 1 && VARTYPE == "NULL") 
  {
    for (int i = 0; i < (DOUBLE_END_ADDRESS - (DOUBLE_START_ADDRESS + 1) + 1) / DOUBLE_SIZE; i++) {
      for (int j = 0; j < 8; j++) {
        if (INTERNAL_RAM[(DOUBLE_START_ADDRESS + 1 + j) + (i * DOUBLE_SIZE)] != VARNAME.charAt(j)) {
          break;
        }

        if (j == 7) {
          uint32_t VAR_ADDRESS = (DOUBLE_START_ADDRESS + 1) + (i * DOUBLE_SIZE);

          return VAR_ADDRESS;
        }
      }
    }
  }

  if (INT32_END_ADDRESS - (INT32_START_ADDRESS + 1) >= INT32_SIZE - 1 && VARTYPE == "INT32" || INT32_END_ADDRESS - (INT32_START_ADDRESS + 1) >= INT32_SIZE - 1 && VARTYPE == "NULL") 
  {
    for (int i = 0; i < (INT32_END_ADDRESS - (INT32_START_ADDRESS + 1) + 1) / INT32_SIZE; i++) {
      for (int j = 0; j < 8; j++) {
        if (INTERNAL_RAM[(INT32_START_ADDRESS + 1 + j) + (i * INT32_SIZE)] != VARNAME.charAt(j)) {
          break;
        }

        if (j == 7) {
          uint32_t VAR_ADDRESS = (INT32_START_ADDRESS + 1) + (i * INT32_SIZE);

          return VAR_ADDRESS;
        }
      }
    }
  }

  if (UINT32_END_ADDRESS - (UINT32_START_ADDRESS + 1) >= INT32_SIZE - 1 && VARTYPE == "UINT32" || UINT32_END_ADDRESS - (UINT32_START_ADDRESS + 1) >= INT32_SIZE - 1 && VARTYPE == "NULL") 
  {
    for (int i = 0; i < (UINT32_END_ADDRESS - (UINT32_START_ADDRESS + 1) + 1) / INT32_SIZE; i++) {
      for (int j = 0; j < 8; j++) {
        if (INTERNAL_RAM[(UINT32_START_ADDRESS + 1 + j) + (i * INT32_SIZE)] != VARNAME.charAt(j)) {
          break;
        }

        if (j == 7) {
          uint32_t VAR_ADDRESS = (UINT32_START_ADDRESS + 1) + (i * INT32_SIZE);

          return VAR_ADDRESS;
        }
      }
    }
  }

  if (INT16_END_ADDRESS - (INT16_START_ADDRESS + 1) >= INT16_SIZE - 1 && VARTYPE == "INT16" || INT16_END_ADDRESS - (INT16_START_ADDRESS + 1) >= INT16_SIZE - 1 && VARTYPE == "NULL") 
  {
    for (int i = 0; i < (INT16_END_ADDRESS - (INT16_START_ADDRESS + 1) + 1) / INT16_SIZE; i++) {
      for (int j = 0; j < 8; j++) {
        if (INTERNAL_RAM[(INT16_START_ADDRESS + 1 + j) + (i * INT16_SIZE)] != VARNAME.charAt(j)) {
          break;
        }

        if (j == 7) {
          uint32_t VAR_ADDRESS = (INT16_START_ADDRESS + 1) + (i * INT16_SIZE);

          return VAR_ADDRESS;
        }
      }
    }
  }

  if (UINT16_END_ADDRESS - (UINT16_START_ADDRESS + 1) >= INT16_SIZE - 1 && VARTYPE == "UINT16" || UINT16_END_ADDRESS - (UINT16_START_ADDRESS + 1) >= INT16_SIZE - 1 && VARTYPE == "NULL") 
  {
    for (int i = 0; i < (UINT16_END_ADDRESS - (UINT16_START_ADDRESS + 1) + 1) / INT16_SIZE; i++) {
      for (int j = 0; j < 8; j++) {
        if (INTERNAL_RAM[(UINT16_START_ADDRESS + 1 + j) + (i * INT16_SIZE)] != VARNAME.charAt(j)) {
          break;
        }

        if (j == 7) {
          uint32_t VAR_ADDRESS = (UINT16_START_ADDRESS + 1) + (i * INT16_SIZE);

          return VAR_ADDRESS;
        }
      }
    }
  }

  if (INT8_END_ADDRESS - (INT8_START_ADDRESS + 1) >= INT8_SIZE - 1 && VARTYPE == "INT8" || INT8_END_ADDRESS - (INT8_START_ADDRESS + 1) >= INT8_SIZE - 1 && VARTYPE == "NULL") 
  {
    for (int i = 0; i < (INT8_END_ADDRESS - (INT8_START_ADDRESS + 1) + 1) / INT8_SIZE; i++) {
      for (int j = 0; j < 8; j++) {
        if (INTERNAL_RAM[(INT8_START_ADDRESS + 1 + j) + (i * INT8_SIZE)] != VARNAME.charAt(j)) {
          break;
        }

        if (j == 7) {
          uint32_t VAR_ADDRESS = (INT8_START_ADDRESS + 1) + (i * INT8_SIZE);

          return VAR_ADDRESS;
        }
      }
    }
  }

  if (UINT8_END_ADDRESS - (UINT8_START_ADDRESS + 1) >= INT8_SIZE - 1 && VARTYPE == "UINT8" || UINT8_END_ADDRESS - (UINT8_START_ADDRESS + 1) >= INT8_SIZE - 1 && VARTYPE == "NULL") 
  {
    for (int i = 0; i < (UINT8_END_ADDRESS - (UINT8_START_ADDRESS + 1) + 1) / INT8_SIZE; i++) {
      for (int j = 0; j < 8; j++) {
        if (INTERNAL_RAM[(UINT8_START_ADDRESS + 1 + j) + (i * INT8_SIZE)] != VARNAME.charAt(j)) {
          break;
        }

        if (j == 7) {
          uint32_t VAR_ADDRESS = (UINT8_START_ADDRESS + 1) + (i * INT8_SIZE);

          return VAR_ADDRESS;
        }
      }
    }
  }

  if (BOOL_END_ADDRESS - (BOOL_START_ADDRESS + 1) >= BOOL_SIZE - 1 && VARTYPE == "BOOL" || BOOL_END_ADDRESS - (BOOL_START_ADDRESS + 1) >= BOOL_SIZE - 1 && VARTYPE == "NULL") 
  {
    for (int i = 0; i < (BOOL_END_ADDRESS - (BOOL_START_ADDRESS + 1) + 1) / BOOL_SIZE; i++) {
      for (int j = 0; j < 8; j++) {
        if (INTERNAL_RAM[(BOOL_START_ADDRESS + 1 + j) + (i * BOOL_SIZE)] != VARNAME.charAt(j)) {
          break;
        }

        if (j == 7) {
          uint32_t VAR_ADDRESS = (BOOL_START_ADDRESS + 1) + (i * BOOL_SIZE);

          return VAR_ADDRESS;
        }
      }
    }
  }

  if (CHAR_END_ADDRESS - (CHAR_START_ADDRESS + 1) >= CHAR_SIZE - 1 && VARTYPE == "CHAR" || CHAR_END_ADDRESS - (CHAR_START_ADDRESS + 1) >= CHAR_SIZE - 1 && VARTYPE == "NULL") 
  {
    for (int i = 0; i < (CHAR_END_ADDRESS - (CHAR_START_ADDRESS + 1) + 1) / CHAR_SIZE; i++) {
      for (int j = 0; j < 8; j++) {
        if (INTERNAL_RAM[(CHAR_START_ADDRESS + 1 + j) + (i * CHAR_SIZE)] != VARNAME.charAt(j)) {
          break;
        }

        if (j == 7) {
          uint32_t VAR_ADDRESS = (CHAR_START_ADDRESS + 1) + (i * CHAR_SIZE);

          return VAR_ADDRESS;
        }
      }
    }
  }

  int32_t ARRAY_ADDRESS = GET_ARRAY_INFO(VARNAME, VARTYPE, 0);
  if(ARRAY_ADDRESS > -1)
  {
    return ARRAY_ADDRESS;
  }

  return -1;
}

int8_t DELETE_ARRAY(String ARRAY_NAME)
{
  ARRAY_NAME.trim();

  if(GET_ARRAY_INFO(ARRAY_NAME, "NULL", 0) < 0)
  {
    return -1;
  }

  if(ARRAY_NAME.length() != 8)
  {
    return -1;
  }

  uint32_t ARRAY_START = GET_ARRAY_INFO(ARRAY_NAME, "NULL", 0);
  uint32_t ARRAY_END = GET_ARRAY_INFO(ARRAY_NAME, "NULL", 1);
  uint16_t ARRAY_LENGTH = (ARRAY_END - ARRAY_START) + 1;

  for(int i = 0; i < CHAR_END_ADDRESS - ARRAY_END ; i++)
  {
    //Serial.print("\n");
    //Serial.print(i);
    //Serial.print(" of ");
    //Serial.print(CHAR_END_ADDRESS - ARRAY_END);
    INTERNAL_RAM[ARRAY_START + i] = INTERNAL_RAM[ARRAY_END + 1 + i]; 
  }

  //NOW DELETE ARRAY DATA 

  if(ARRAY_START < DOUBLE_END_ADDRESS)
  {

    CHAR_END_ADDRESS = CHAR_END_ADDRESS - (ARRAY_END - ARRAY_START + 1);
    CHAR_START_ADDRESS = CHAR_START_ADDRESS - (ARRAY_END - ARRAY_START + 1);

    BOOL_START_ADDRESS = BOOL_START_ADDRESS - (ARRAY_END - ARRAY_START + 1);
    BOOL_END_ADDRESS = BOOL_END_ADDRESS - (ARRAY_END - ARRAY_START + 1);

    UINT8_START_ADDRESS = UINT8_START_ADDRESS - (ARRAY_END - ARRAY_START + 1);
    UINT8_END_ADDRESS = UINT8_END_ADDRESS - (ARRAY_END - ARRAY_START + 1);

    INT8_START_ADDRESS = INT8_START_ADDRESS - (ARRAY_END - ARRAY_START + 1);
    INT8_END_ADDRESS = INT8_END_ADDRESS - (ARRAY_END - ARRAY_START + 1);

    UINT16_START_ADDRESS = UINT16_START_ADDRESS - (ARRAY_END - ARRAY_START + 1);
    UINT16_END_ADDRESS = UINT16_END_ADDRESS - (ARRAY_END - ARRAY_START + 1);

    INT16_START_ADDRESS = INT16_START_ADDRESS - (ARRAY_END - ARRAY_START + 1);
    INT16_END_ADDRESS = INT16_END_ADDRESS - (ARRAY_END - ARRAY_START + 1);

    UINT32_START_ADDRESS = UINT32_START_ADDRESS - (ARRAY_END - ARRAY_START + 1);
    UINT32_END_ADDRESS = UINT32_END_ADDRESS - (ARRAY_END - ARRAY_START + 1);

    INT32_START_ADDRESS = INT32_START_ADDRESS - (ARRAY_END - ARRAY_START + 1);
    INT32_END_ADDRESS = INT32_END_ADDRESS - (ARRAY_END - ARRAY_START + 1);

    UINT64_START_ADDRESS = UINT64_START_ADDRESS - (ARRAY_END - ARRAY_START + 1);
    UINT64_END_ADDRESS = UINT64_END_ADDRESS - (ARRAY_END - ARRAY_START + 1);

    INT64_START_ADDRESS = INT64_START_ADDRESS - (ARRAY_END - ARRAY_START + 1);
    INT64_END_ADDRESS = INT64_END_ADDRESS - (ARRAY_END - ARRAY_START + 1);

    DOUBLE_END_ADDRESS = DOUBLE_END_ADDRESS - (ARRAY_END - ARRAY_START + 1);
  }else if(ARRAY_START < INT64_END_ADDRESS)
  {

    CHAR_END_ADDRESS = CHAR_END_ADDRESS - (ARRAY_END - ARRAY_START + 1);
    CHAR_START_ADDRESS = CHAR_START_ADDRESS - (ARRAY_END - ARRAY_START + 1);

    BOOL_START_ADDRESS = BOOL_START_ADDRESS - (ARRAY_END - ARRAY_START + 1);
    BOOL_END_ADDRESS = BOOL_END_ADDRESS - (ARRAY_END - ARRAY_START + 1);

    UINT8_START_ADDRESS = UINT8_START_ADDRESS - (ARRAY_END - ARRAY_START + 1);
    UINT8_END_ADDRESS = UINT8_END_ADDRESS - (ARRAY_END - ARRAY_START + 1);

    INT8_START_ADDRESS = INT8_START_ADDRESS - (ARRAY_END - ARRAY_START + 1);
    INT8_END_ADDRESS = INT8_END_ADDRESS - (ARRAY_END - ARRAY_START + 1);

    UINT16_START_ADDRESS = UINT16_START_ADDRESS - (ARRAY_END - ARRAY_START + 1);
    UINT16_END_ADDRESS = UINT16_END_ADDRESS - (ARRAY_END - ARRAY_START + 1);

    INT16_START_ADDRESS = INT16_START_ADDRESS - (ARRAY_END - ARRAY_START + 1);
    INT16_END_ADDRESS = INT16_END_ADDRESS - (ARRAY_END - ARRAY_START + 1);

    UINT32_START_ADDRESS = UINT32_START_ADDRESS - (ARRAY_END - ARRAY_START + 1);
    UINT32_END_ADDRESS = UINT32_END_ADDRESS - (ARRAY_END - ARRAY_START + 1);

    INT32_START_ADDRESS = INT32_START_ADDRESS - (ARRAY_END - ARRAY_START + 1);
    INT32_END_ADDRESS = INT32_END_ADDRESS - (ARRAY_END - ARRAY_START + 1);

    UINT64_START_ADDRESS = UINT64_START_ADDRESS - (ARRAY_END - ARRAY_START + 1);
    UINT64_END_ADDRESS = UINT64_END_ADDRESS - (ARRAY_END - ARRAY_START + 1);

    INT64_END_ADDRESS = INT64_END_ADDRESS - (ARRAY_END - ARRAY_START + 1);

  }else if(ARRAY_START < UINT64_END_ADDRESS)
  {

    CHAR_END_ADDRESS = CHAR_END_ADDRESS - (ARRAY_END - ARRAY_START + 1);
    CHAR_START_ADDRESS = CHAR_START_ADDRESS - (ARRAY_END - ARRAY_START + 1);

    BOOL_START_ADDRESS = BOOL_START_ADDRESS - (ARRAY_END - ARRAY_START + 1);
    BOOL_END_ADDRESS = BOOL_END_ADDRESS - (ARRAY_END - ARRAY_START + 1);

    UINT8_START_ADDRESS = UINT8_START_ADDRESS - (ARRAY_END - ARRAY_START + 1);
    UINT8_END_ADDRESS = UINT8_END_ADDRESS - (ARRAY_END - ARRAY_START + 1);

    INT8_START_ADDRESS = INT8_START_ADDRESS - (ARRAY_END - ARRAY_START + 1);
    INT8_END_ADDRESS = INT8_END_ADDRESS - (ARRAY_END - ARRAY_START + 1);

    UINT16_START_ADDRESS = UINT16_START_ADDRESS - (ARRAY_END - ARRAY_START + 1);
    UINT16_END_ADDRESS = UINT16_END_ADDRESS - (ARRAY_END - ARRAY_START + 1);

    INT16_START_ADDRESS = INT16_START_ADDRESS - (ARRAY_END - ARRAY_START + 1);
    INT16_END_ADDRESS = INT16_END_ADDRESS - (ARRAY_END - ARRAY_START + 1);

    UINT32_START_ADDRESS = UINT32_START_ADDRESS - (ARRAY_END - ARRAY_START + 1);
    UINT32_END_ADDRESS = UINT32_END_ADDRESS - (ARRAY_END - ARRAY_START + 1);

    INT32_START_ADDRESS = INT32_START_ADDRESS - (ARRAY_END - ARRAY_START + 1);
    INT32_END_ADDRESS = INT32_END_ADDRESS - (ARRAY_END - ARRAY_START + 1);

    UINT64_END_ADDRESS = UINT64_END_ADDRESS - (ARRAY_END - ARRAY_START + 1);

  }else if(ARRAY_START < INT32_END_ADDRESS)
  {

    CHAR_END_ADDRESS = CHAR_END_ADDRESS - (ARRAY_END - ARRAY_START + 1);
    CHAR_START_ADDRESS = CHAR_START_ADDRESS - (ARRAY_END - ARRAY_START + 1);

    BOOL_START_ADDRESS = BOOL_START_ADDRESS - (ARRAY_END - ARRAY_START + 1);
    BOOL_END_ADDRESS = BOOL_END_ADDRESS - (ARRAY_END - ARRAY_START + 1);

    UINT8_START_ADDRESS = UINT8_START_ADDRESS - (ARRAY_END - ARRAY_START + 1);
    UINT8_END_ADDRESS = UINT8_END_ADDRESS - (ARRAY_END - ARRAY_START + 1);

    INT8_START_ADDRESS = INT8_START_ADDRESS - (ARRAY_END - ARRAY_START + 1);
    INT8_END_ADDRESS = INT8_END_ADDRESS - (ARRAY_END - ARRAY_START + 1);

    UINT16_START_ADDRESS = UINT16_START_ADDRESS - (ARRAY_END - ARRAY_START + 1);
    UINT16_END_ADDRESS = UINT16_END_ADDRESS - (ARRAY_END - ARRAY_START + 1);

    INT16_START_ADDRESS = INT16_START_ADDRESS - (ARRAY_END - ARRAY_START + 1);
    INT16_END_ADDRESS = INT16_END_ADDRESS - (ARRAY_END - ARRAY_START + 1);

    UINT32_START_ADDRESS = UINT32_START_ADDRESS - (ARRAY_END - ARRAY_START + 1);
    UINT32_END_ADDRESS = UINT32_END_ADDRESS - (ARRAY_END - ARRAY_START + 1);

    INT32_END_ADDRESS = INT32_END_ADDRESS - (ARRAY_END - ARRAY_START + 1);

  }else if(ARRAY_START < UINT32_END_ADDRESS)
  {

    CHAR_END_ADDRESS = CHAR_END_ADDRESS - (ARRAY_END - ARRAY_START + 1);
    CHAR_START_ADDRESS = CHAR_START_ADDRESS - (ARRAY_END - ARRAY_START + 1);

    BOOL_START_ADDRESS = BOOL_START_ADDRESS - (ARRAY_END - ARRAY_START + 1);
    BOOL_END_ADDRESS = BOOL_END_ADDRESS - (ARRAY_END - ARRAY_START + 1);

    UINT8_START_ADDRESS = UINT8_START_ADDRESS - (ARRAY_END - ARRAY_START + 1);
    UINT8_END_ADDRESS = UINT8_END_ADDRESS - (ARRAY_END - ARRAY_START + 1);

    INT8_START_ADDRESS = INT8_START_ADDRESS - (ARRAY_END - ARRAY_START + 1);
    INT8_END_ADDRESS = INT8_END_ADDRESS - (ARRAY_END - ARRAY_START + 1);

    UINT16_START_ADDRESS = UINT16_START_ADDRESS - (ARRAY_END - ARRAY_START + 1);
    UINT16_END_ADDRESS = UINT16_END_ADDRESS - (ARRAY_END - ARRAY_START + 1);

    INT16_START_ADDRESS = INT16_START_ADDRESS - (ARRAY_END - ARRAY_START + 1);
    INT16_END_ADDRESS = INT16_END_ADDRESS - (ARRAY_END - ARRAY_START + 1);

    UINT32_END_ADDRESS = UINT32_END_ADDRESS - (ARRAY_END - ARRAY_START + 1);

  }else if(ARRAY_START < INT16_END_ADDRESS)
  {

    CHAR_END_ADDRESS = CHAR_END_ADDRESS - (ARRAY_END - ARRAY_START + 1);
    CHAR_START_ADDRESS = CHAR_START_ADDRESS - (ARRAY_END - ARRAY_START + 1);

    BOOL_START_ADDRESS = BOOL_START_ADDRESS - (ARRAY_END - ARRAY_START + 1);
    BOOL_END_ADDRESS = BOOL_END_ADDRESS - (ARRAY_END - ARRAY_START + 1);

    UINT8_START_ADDRESS = UINT8_START_ADDRESS - (ARRAY_END - ARRAY_START + 1);
    UINT8_END_ADDRESS = UINT8_END_ADDRESS - (ARRAY_END - ARRAY_START + 1);

    INT8_START_ADDRESS = INT8_START_ADDRESS - (ARRAY_END - ARRAY_START + 1);
    INT8_END_ADDRESS = INT8_END_ADDRESS - (ARRAY_END - ARRAY_START + 1);

    UINT16_START_ADDRESS = UINT16_START_ADDRESS - (ARRAY_END - ARRAY_START + 1);
    UINT16_END_ADDRESS = UINT16_END_ADDRESS - (ARRAY_END - ARRAY_START + 1);

    INT16_END_ADDRESS = INT16_END_ADDRESS - (ARRAY_END - ARRAY_START + 1);

  }else if(ARRAY_START < UINT16_END_ADDRESS)
  {

    CHAR_END_ADDRESS = CHAR_END_ADDRESS - (ARRAY_END - ARRAY_START + 1);
    CHAR_START_ADDRESS = CHAR_START_ADDRESS - (ARRAY_END - ARRAY_START + 1);

    BOOL_START_ADDRESS = BOOL_START_ADDRESS - (ARRAY_END - ARRAY_START + 1);
    BOOL_END_ADDRESS = BOOL_END_ADDRESS - (ARRAY_END - ARRAY_START + 1);

    UINT8_START_ADDRESS = UINT8_START_ADDRESS - (ARRAY_END - ARRAY_START + 1);
    UINT8_END_ADDRESS = UINT8_END_ADDRESS - (ARRAY_END - ARRAY_START + 1);

    INT8_START_ADDRESS = INT8_START_ADDRESS - (ARRAY_END - ARRAY_START + 1);
    INT8_END_ADDRESS = INT8_END_ADDRESS - (ARRAY_END - ARRAY_START + 1);

    UINT16_END_ADDRESS = UINT16_END_ADDRESS - (ARRAY_END - ARRAY_START + 1);

  }else if(ARRAY_START < INT8_END_ADDRESS)
  {

    CHAR_END_ADDRESS = CHAR_END_ADDRESS - (ARRAY_END - ARRAY_START + 1);
    CHAR_START_ADDRESS = CHAR_START_ADDRESS - (ARRAY_END - ARRAY_START + 1);

    BOOL_START_ADDRESS = BOOL_START_ADDRESS - (ARRAY_END - ARRAY_START + 1);
    BOOL_END_ADDRESS = BOOL_END_ADDRESS - (ARRAY_END - ARRAY_START + 1);

    UINT8_START_ADDRESS = UINT8_START_ADDRESS - (ARRAY_END - ARRAY_START + 1);
    UINT8_END_ADDRESS = UINT8_END_ADDRESS - (ARRAY_END - ARRAY_START + 1);

    INT8_END_ADDRESS = INT8_END_ADDRESS - (ARRAY_END - ARRAY_START + 1);

  }else if(ARRAY_START < UINT8_END_ADDRESS)
  {

    CHAR_END_ADDRESS = CHAR_END_ADDRESS - (ARRAY_END - ARRAY_START + 1);
    CHAR_START_ADDRESS = CHAR_START_ADDRESS - (ARRAY_END - ARRAY_START + 1);

    BOOL_START_ADDRESS = BOOL_START_ADDRESS - (ARRAY_END - ARRAY_START + 1);
    BOOL_END_ADDRESS = BOOL_END_ADDRESS - (ARRAY_END - ARRAY_START + 1);

    UINT8_END_ADDRESS = UINT8_END_ADDRESS - (ARRAY_END - ARRAY_START + 1);

  }else if(ARRAY_START < BOOL_END_ADDRESS)
  {

    CHAR_END_ADDRESS = CHAR_END_ADDRESS - (ARRAY_END - ARRAY_START + 1);
    CHAR_START_ADDRESS = CHAR_START_ADDRESS - (ARRAY_END - ARRAY_START + 1);

    BOOL_END_ADDRESS = BOOL_END_ADDRESS - (ARRAY_END - ARRAY_START + 1);

  }else if(ARRAY_START < CHAR_END_ADDRESS)
  {

    CHAR_END_ADDRESS = CHAR_END_ADDRESS - (ARRAY_END - ARRAY_START + 1);

  }

  for(int i = 0; i <= ARRAY_END - ARRAY_START; i++)
  {
    INTERNAL_RAM[CHAR_END_ADDRESS + i + 1] = 0;
  }

  //FROM HERE UNTIL THE NEXT COMMENT THIS FUNCTION WILL ERASE ALL THE INFO OF THE DELETED ARRAY FROM THE ARRAY DATA SETION.

  uint32_t ARRAY_INFO_START = GET_ARRAY_INFO(ARRAY_NAME, "NULL", 2);
  uint32_t ARRAY_INFO_END = GET_ARRAY_INFO(ARRAY_NAME, "NULL", 2) + 15;
  
  for(int i = ARRAY_INFO_START - 1; i >= ARRAY_DATA_START_ADDRESS; i--)
  {
    INTERNAL_RAM[i + 16] = INTERNAL_RAM[i];
  }

  ARRAY_DATA_START_ADDRESS = ARRAY_DATA_START_ADDRESS + 16;

  for(int i = 0; i < 16; i++)
  {
    INTERNAL_RAM[ARRAY_DATA_START_ADDRESS - i - 1] = 0;
  }

  //-------------------------------------------------------------------------------------------------------------------

  for(int i = 0; i < (ARRAY_DATA_END_ADDRESS - ARRAY_DATA_START_ADDRESS) / 16; i++)
  {
    uint32_t startValue = 0;
    uint32_t endValue = 0;
    uint32_t n = 1;
    for (int k = 0; k < 4; k++) 
    {
      uint8_t valueByte = INTERNAL_RAM[ARRAY_DATA_START_ADDRESS + (i * 16) + 11 - k];

      for (int l = 0; l < 8; l++) 
      {
        uint8_t mask = 1 << l;
        if (mask & valueByte) 
        {
          startValue = startValue + n;
        }

        n = n * 2;
      }
    }

    n = 1;
    for (int k = 0; k < 4; k++) 
    {
      uint8_t valueByte = INTERNAL_RAM[ARRAY_DATA_START_ADDRESS + (i * 16) + 15 - k];

      for (int l = 0; l < 8; l++) 
      {
        uint8_t mask = 1 << l;
        if (mask & valueByte) 
        {
          endValue = endValue + n;
        }

        n = n * 2;
      }
    }

    if(startValue > ARRAY_END)
    {
      uint32_t newArrayStart = startValue - ARRAY_LENGTH;
      uint32_t newArrayEnd = endValue - ARRAY_LENGTH;

      for(int l = 0; l < 8; l++)
      {
        INTERNAL_RAM[ARRAY_DATA_START_ADDRESS + (i * 16) + 15 - l] = 0;
      }

      byte* ptr1 = (byte*)&newArrayStart;
      for (byte l = 0; l < sizeof(newArrayStart); l++) 
      {
        INTERNAL_RAM[ARRAY_DATA_START_ADDRESS + (i * 16) + 11 - l] = ptr1[l];
      }

      byte* ptr2 = (byte*)&newArrayEnd;
      for (byte l = 0; l < sizeof(newArrayEnd); l++) 
      {
        INTERNAL_RAM[ARRAY_DATA_START_ADDRESS + (i * 16) + 15 - l] = ptr2[l];
      }
    } 
  }

  return 1;
}

int DELETE_VARIABLE(String VARNAME) 
{
  VARNAME.trim();

  //EACH SECTION OF CODE LIKE THIS FIRST CHECKS IF VARIABLE DATA TYPE ADDRESS BEGIN AND END ARE AT GREATER OR AT LEAST EQUAL TO THE DATA TYPE SIZE, OTHERWISE DON`T EVEN CHECK
  if (UINT64_END_ADDRESS - (UINT64_START_ADDRESS + 1) >= INT64_SIZE - 1)
  {
    for (int i = 0; i < (UINT64_END_ADDRESS - (UINT64_START_ADDRESS + 1) + 1) / INT64_SIZE; i++) 
    {
      for (int j = 0; j < 8; j++) 
      {
        if (INTERNAL_RAM[(UINT64_START_ADDRESS + 1 + j) + (i * INT64_SIZE)] != VARNAME.charAt(j)) 
        {
          break;
        }

        if (j == 7) 
        {
          uint32_t VAR_ADDRESS = (UINT64_START_ADDRESS + 1) + (i * INT64_SIZE);

          for (int i = VAR_ADDRESS; i < CHAR_END_ADDRESS; i++) 
          {
            INTERNAL_RAM[i] = INTERNAL_RAM[i + 16];
          }

          CHAR_END_ADDRESS = CHAR_END_ADDRESS - INT64_SIZE;
          CHAR_START_ADDRESS = CHAR_START_ADDRESS - INT64_SIZE;

          BOOL_START_ADDRESS = BOOL_START_ADDRESS - INT64_SIZE;
          BOOL_END_ADDRESS = BOOL_END_ADDRESS - INT64_SIZE;

          UINT8_START_ADDRESS = UINT8_START_ADDRESS - INT64_SIZE;
          UINT8_END_ADDRESS = UINT8_END_ADDRESS - INT64_SIZE;

          INT8_START_ADDRESS = INT8_START_ADDRESS - INT64_SIZE;
          INT8_END_ADDRESS = INT8_END_ADDRESS - INT64_SIZE;

          UINT16_START_ADDRESS = UINT16_START_ADDRESS - INT64_SIZE;
          UINT16_END_ADDRESS = UINT16_END_ADDRESS - INT64_SIZE;

          INT16_START_ADDRESS = INT16_START_ADDRESS - INT64_SIZE;
          INT16_END_ADDRESS = INT16_END_ADDRESS - INT64_SIZE;

          UINT32_START_ADDRESS = UINT32_START_ADDRESS - INT64_SIZE;
          UINT32_END_ADDRESS = UINT32_END_ADDRESS - INT64_SIZE;

          INT32_START_ADDRESS = INT32_START_ADDRESS - INT64_SIZE;
          INT32_END_ADDRESS = INT32_END_ADDRESS - INT64_SIZE;

          UINT64_END_ADDRESS = UINT64_END_ADDRESS - INT64_SIZE;

          for (int i = CHAR_END_ADDRESS + 1; i < (CHAR_END_ADDRESS + 1) + 16; i++) 
          {
            INTERNAL_RAM[i] = 0;
          }

          //GIVE EVERY ARRAY IN ARRAY DATA MEMORY SECTION NEW START AND END ADDRESSES
          for(int m = 0; m < (ARRAY_DATA_END_ADDRESS - ARRAY_DATA_START_ADDRESS) / 16; m++)
          {
            uint32_t startValue = 0;
            uint32_t endValue = 0;
            uint32_t n = 1;
            for (int k = 0; k < 4; k++) 
            {
              uint8_t valueByte = INTERNAL_RAM[ARRAY_DATA_START_ADDRESS + (m * 16) + 11 - k];

              for (int l = 0; l < 8; l++) 
              {
                uint8_t mask = 1 << l;
                if (mask & valueByte) 
                {
                  startValue = startValue + n;
                }

                n = n * 2;
              }
            }

            n = 1;
            for (int k = 0; k < 4; k++) 
            {
              uint8_t valueByte = INTERNAL_RAM[ARRAY_DATA_START_ADDRESS + (m * 16) + 15 - k];

              for (int l = 0; l < 8; l++) 
              {
                uint8_t mask = 1 << l;
                if (mask & valueByte) 
                {
                  endValue = endValue + n;
                }

                n = n * 2;
              }
            }

            if(startValue > (UINT64_START_ADDRESS + 1) + (i * INT64_SIZE))
            {
              uint32_t newArrayStart = startValue - INT64_SIZE;
              uint32_t newArrayEnd = endValue - INT64_SIZE;

              for(int l = 0; l < 8; l++)
              {
                INTERNAL_RAM[ARRAY_DATA_START_ADDRESS + (m * 16) + 15 - l] = 0;
              }

              byte* ptr1 = (byte*)&newArrayStart;
              for (byte l = 0; l < sizeof(newArrayStart); l++) 
              {
                INTERNAL_RAM[ARRAY_DATA_START_ADDRESS + (m * 16) + 11 - l] = ptr1[l];
              }

              byte* ptr2 = (byte*)&newArrayEnd;
              for (byte l = 0; l < sizeof(newArrayEnd); l++) 
              {
                INTERNAL_RAM[ARRAY_DATA_START_ADDRESS + (m * 16) + 15 - l] = ptr2[l];
              }
            } 
          }

          return 1;
        }
      }
    }
  }

  if (INT64_END_ADDRESS - (INT64_START_ADDRESS + 1) >= INT64_SIZE - 1) 
  {
    for (int i = 0; i < (INT64_END_ADDRESS - (INT64_START_ADDRESS + 1) + 1) / INT64_SIZE; i++) {
      for (int j = 0; j < 8; j++) {
        if (INTERNAL_RAM[(INT64_START_ADDRESS + 1 + j) + (i * INT64_SIZE)] != VARNAME.charAt(j)) {
          break;
        }

        if (j == 7) {
          uint32_t VAR_ADDRESS = (INT64_START_ADDRESS + 1) + (i * INT64_SIZE);

          for (int i = VAR_ADDRESS; i < CHAR_END_ADDRESS; i++) {
            INTERNAL_RAM[i] = INTERNAL_RAM[i + 16];
          }

          CHAR_END_ADDRESS = CHAR_END_ADDRESS - INT64_SIZE;
          CHAR_START_ADDRESS = CHAR_START_ADDRESS - INT64_SIZE;

          BOOL_START_ADDRESS = BOOL_START_ADDRESS - INT64_SIZE;
          BOOL_END_ADDRESS = BOOL_END_ADDRESS - INT64_SIZE;

          UINT8_START_ADDRESS = UINT8_START_ADDRESS - INT64_SIZE;
          UINT8_END_ADDRESS = UINT8_END_ADDRESS - INT64_SIZE;

          INT8_START_ADDRESS = INT8_START_ADDRESS - INT64_SIZE;
          INT8_END_ADDRESS = INT8_END_ADDRESS - INT64_SIZE;

          UINT16_START_ADDRESS = UINT16_START_ADDRESS - INT64_SIZE;
          UINT16_END_ADDRESS = UINT16_END_ADDRESS - INT64_SIZE;

          INT16_START_ADDRESS = INT16_START_ADDRESS - INT64_SIZE;
          INT16_END_ADDRESS = INT16_END_ADDRESS - INT64_SIZE;

          UINT32_START_ADDRESS = UINT32_START_ADDRESS - INT64_SIZE;
          UINT32_END_ADDRESS = UINT32_END_ADDRESS - INT64_SIZE;

          INT32_START_ADDRESS = INT32_START_ADDRESS - INT64_SIZE;
          INT32_END_ADDRESS = INT32_END_ADDRESS - INT64_SIZE;

          UINT64_START_ADDRESS = UINT64_START_ADDRESS - INT64_SIZE;
          UINT64_END_ADDRESS = UINT64_END_ADDRESS - INT64_SIZE;

          INT64_END_ADDRESS = INT64_END_ADDRESS - INT64_SIZE;

          for (int i = CHAR_END_ADDRESS + 1; i < (CHAR_END_ADDRESS + 1) + 16; i++) {
            INTERNAL_RAM[i] = 0;
          }

          //GIVE EVERY ARRAY IN ARRAY DATA MEMORY SECTION NEW START AND END ADDRESSES
          for(int m = 0; m < (ARRAY_DATA_END_ADDRESS - ARRAY_DATA_START_ADDRESS) / 16; m++)
          {
            uint32_t startValue = 0;
            uint32_t endValue = 0;
            uint32_t n = 1;
            for (int k = 0; k < 4; k++) 
            {
              uint8_t valueByte = INTERNAL_RAM[ARRAY_DATA_START_ADDRESS + (m * 16) + 11 - k];

              for (int l = 0; l < 8; l++) 
              {
                uint8_t mask = 1 << l;
                if (mask & valueByte) 
                {
                  startValue = startValue + n;
                }

                n = n * 2;
              }
            }

            n = 1;
            for (int k = 0; k < 4; k++) 
            {
              uint8_t valueByte = INTERNAL_RAM[ARRAY_DATA_START_ADDRESS + (m * 16) + 15 - k];

              for (int l = 0; l < 8; l++) 
              {
                uint8_t mask = 1 << l;
                if (mask & valueByte) 
                {
                  endValue = endValue + n;
                }

                n = n * 2;
              }
            }

            if(startValue > (INT64_START_ADDRESS + 1) + (i * INT64_SIZE))
            {
              uint32_t newArrayStart = startValue - INT64_SIZE;
              uint32_t newArrayEnd = endValue - INT64_SIZE;

              for(int l = 0; l < 8; l++)
              {
                INTERNAL_RAM[ARRAY_DATA_START_ADDRESS + (m * 16) + 15 - l] = 0;
              }

              byte* ptr1 = (byte*)&newArrayStart;
              for (byte l = 0; l < sizeof(newArrayStart); l++) 
              {
                INTERNAL_RAM[ARRAY_DATA_START_ADDRESS + (m * 16) + 11 - l] = ptr1[l];
              }

              byte* ptr2 = (byte*)&newArrayEnd;
              for (byte l = 0; l < sizeof(newArrayEnd); l++) 
              {
                INTERNAL_RAM[ARRAY_DATA_START_ADDRESS + (m * 16) + 15 - l] = ptr2[l];
              }
            } 
          }

          return 1;
        }
      }
    }
  }

  if (DOUBLE_END_ADDRESS - (DOUBLE_START_ADDRESS + 1) >= DOUBLE_SIZE - 1) {
    for (int i = 0; i < (DOUBLE_END_ADDRESS - (DOUBLE_START_ADDRESS + 1) + 1) / DOUBLE_SIZE; i++) 
    {
      for (int j = 0; j < 8; j++) {
        if (INTERNAL_RAM[(DOUBLE_START_ADDRESS + 1 + j) + (i * DOUBLE_SIZE)] != VARNAME.charAt(j)) {
          break;
        }

        if (j == 7) 
        {
          uint32_t VAR_ADDRESS = (DOUBLE_START_ADDRESS + 1) + (i * DOUBLE_SIZE);

          for (int i = VAR_ADDRESS; i < CHAR_END_ADDRESS; i++) {
            INTERNAL_RAM[i] = INTERNAL_RAM[i + 16];
          }

          CHAR_END_ADDRESS = CHAR_END_ADDRESS - DOUBLE_SIZE;
          CHAR_START_ADDRESS = CHAR_START_ADDRESS - DOUBLE_SIZE;

          BOOL_START_ADDRESS = BOOL_START_ADDRESS - DOUBLE_SIZE;
          BOOL_END_ADDRESS = BOOL_END_ADDRESS - DOUBLE_SIZE;

          UINT8_START_ADDRESS = UINT8_START_ADDRESS - DOUBLE_SIZE;
          UINT8_END_ADDRESS = UINT8_END_ADDRESS - DOUBLE_SIZE;

          INT8_START_ADDRESS = INT8_START_ADDRESS - DOUBLE_SIZE;
          INT8_END_ADDRESS = INT8_END_ADDRESS - DOUBLE_SIZE;

          UINT16_START_ADDRESS = UINT16_START_ADDRESS - DOUBLE_SIZE;
          UINT16_END_ADDRESS = UINT16_END_ADDRESS - DOUBLE_SIZE;

          INT16_START_ADDRESS = INT16_START_ADDRESS - DOUBLE_SIZE;
          INT16_END_ADDRESS = INT16_END_ADDRESS - DOUBLE_SIZE;

          UINT32_START_ADDRESS = UINT32_START_ADDRESS - DOUBLE_SIZE;
          UINT32_END_ADDRESS = UINT32_END_ADDRESS - DOUBLE_SIZE;

          INT32_START_ADDRESS = INT32_START_ADDRESS - DOUBLE_SIZE;
          INT32_END_ADDRESS = INT32_END_ADDRESS - DOUBLE_SIZE;

          UINT64_START_ADDRESS = UINT64_START_ADDRESS - DOUBLE_SIZE;
          UINT64_END_ADDRESS = UINT64_END_ADDRESS - DOUBLE_SIZE;

          INT64_START_ADDRESS = INT64_START_ADDRESS - DOUBLE_SIZE;
          INT64_END_ADDRESS = INT64_END_ADDRESS - DOUBLE_SIZE;

          DOUBLE_END_ADDRESS = DOUBLE_END_ADDRESS - DOUBLE_SIZE;

          for (int i = CHAR_END_ADDRESS + 1; i < (CHAR_END_ADDRESS + 1) + 16; i++) {
            INTERNAL_RAM[i] = 0;
          }

          //GIVE EVERY ARRAY IN ARRAY DATA MEMORY SECTION NEW START AND END ADDRESSES
          for(int m = 0; m < (ARRAY_DATA_END_ADDRESS - ARRAY_DATA_START_ADDRESS) / 16; m++)
          {
            uint32_t startValue = 0;
            uint32_t endValue = 0;
            uint32_t n = 1;
            for (int k = 0; k < 4; k++) 
            {
              uint8_t valueByte = INTERNAL_RAM[ARRAY_DATA_START_ADDRESS + (m * 16) + 11 - k];

              for (int l = 0; l < 8; l++) 
              {
                uint8_t mask = 1 << l;
                if (mask & valueByte) 
                {
                  startValue = startValue + n;
                }

                n = n * 2;
              }
            }

            n = 1;
            for (int k = 0; k < 4; k++) 
            {
              uint8_t valueByte = INTERNAL_RAM[ARRAY_DATA_START_ADDRESS + (m * 16) + 15 - k];

              for (int l = 0; l < 8; l++) 
              {
                uint8_t mask = 1 << l;
                if (mask & valueByte) 
                {
                  endValue = endValue + n;
                }

                n = n * 2;
              }
            }

            if(startValue > (DOUBLE_START_ADDRESS + 1) + (i * DOUBLE_SIZE))
            {
              uint32_t newArrayStart = startValue - DOUBLE_SIZE;
              uint32_t newArrayEnd = endValue - DOUBLE_SIZE;

              for(int l = 0; l < 8; l++)
              {
                INTERNAL_RAM[ARRAY_DATA_START_ADDRESS + (m * 16) + 15 - l] = 0;
              }

              byte* ptr1 = (byte*)&newArrayStart;
              for (byte l = 0; l < sizeof(newArrayStart); l++) 
              {
                INTERNAL_RAM[ARRAY_DATA_START_ADDRESS + (m * 16) + 11 - l] = ptr1[l];
              }

              byte* ptr2 = (byte*)&newArrayEnd;
              for (byte l = 0; l < sizeof(newArrayEnd); l++) 
              {
                INTERNAL_RAM[ARRAY_DATA_START_ADDRESS + (m * 16) + 15 - l] = ptr2[l];
              }
            } 
          }

          return 1;
        }
      }
    }
  }

  if (INT32_END_ADDRESS - (INT32_START_ADDRESS + 1) >= INT32_SIZE - 1) {
    for (int i = 0; i < (INT32_END_ADDRESS - (INT32_START_ADDRESS + 1) + 1) / INT32_SIZE; i++) {
      for (int j = 0; j < 8; j++) {
        if (INTERNAL_RAM[(INT32_START_ADDRESS + 1 + j) + (i * INT32_SIZE)] != VARNAME.charAt(j)) {
          break;
        }

        if (j == 7) {
          uint32_t VAR_ADDRESS = (INT32_START_ADDRESS + 1) + (i * INT32_SIZE);

          for (int i = VAR_ADDRESS; i < CHAR_END_ADDRESS; i++) {
            INTERNAL_RAM[i] = INTERNAL_RAM[i + 12];
          }

          CHAR_END_ADDRESS = CHAR_END_ADDRESS - INT32_SIZE;
          CHAR_START_ADDRESS = CHAR_START_ADDRESS - INT32_SIZE;

          BOOL_START_ADDRESS = BOOL_START_ADDRESS - INT32_SIZE;
          BOOL_END_ADDRESS = BOOL_END_ADDRESS - INT32_SIZE;

          UINT8_START_ADDRESS = UINT8_START_ADDRESS - INT32_SIZE;
          UINT8_END_ADDRESS = UINT8_END_ADDRESS - INT32_SIZE;

          INT8_START_ADDRESS = INT8_START_ADDRESS - INT32_SIZE;
          INT8_END_ADDRESS = INT8_END_ADDRESS - INT32_SIZE;

          UINT16_START_ADDRESS = UINT16_START_ADDRESS - INT32_SIZE;
          UINT16_END_ADDRESS = UINT16_END_ADDRESS - INT32_SIZE;

          INT16_START_ADDRESS = INT16_START_ADDRESS - INT32_SIZE;
          INT16_END_ADDRESS = INT16_END_ADDRESS - INT32_SIZE;

          UINT32_START_ADDRESS = UINT32_START_ADDRESS - INT32_SIZE;
          UINT32_END_ADDRESS = UINT32_END_ADDRESS - INT32_SIZE;

          INT32_END_ADDRESS = INT32_END_ADDRESS - INT32_SIZE;

          for (int i = CHAR_END_ADDRESS + 1; i < (CHAR_END_ADDRESS + 1) + 12; i++) {
            INTERNAL_RAM[i] = 0;
          }

          //GIVE EVERY ARRAY IN ARRAY DATA MEMORY SECTION NEW START AND END ADDRESSES
          for(int m = 0; m < (ARRAY_DATA_END_ADDRESS - ARRAY_DATA_START_ADDRESS) / 16; m++)
          {
            uint32_t startValue = 0;
            uint32_t endValue = 0;
            uint32_t n = 1;
            for (int k = 0; k < 4; k++) 
            {
              uint8_t valueByte = INTERNAL_RAM[ARRAY_DATA_START_ADDRESS + (m * 16) + 11 - k];

              for (int l = 0; l < 8; l++) 
              {
                uint8_t mask = 1 << l;
                if (mask & valueByte) 
                {
                  startValue = startValue + n;
                }

                n = n * 2;
              }
            }

            n = 1;
            for (int k = 0; k < 4; k++) 
            {
              uint8_t valueByte = INTERNAL_RAM[ARRAY_DATA_START_ADDRESS + (m * 16) + 15 - k];

              for (int l = 0; l < 8; l++) 
              {
                uint8_t mask = 1 << l;
                if (mask & valueByte) 
                {
                  endValue = endValue + n;
                }

                n = n * 2;
              }
            }

            if(startValue > (INT32_START_ADDRESS + 1) + (i * INT32_SIZE))
            {
              uint32_t newArrayStart = startValue - INT32_SIZE;
              uint32_t newArrayEnd = endValue - INT32_SIZE;

              for(int l = 0; l < 8; l++)
              {
                INTERNAL_RAM[ARRAY_DATA_START_ADDRESS + (m * 16) + 15 - l] = 0;
              }

              byte* ptr1 = (byte*)&newArrayStart;
              for (byte l = 0; l < sizeof(newArrayStart); l++) 
              {
                INTERNAL_RAM[ARRAY_DATA_START_ADDRESS + (m * 16) + 11 - l] = ptr1[l];
              }

              byte* ptr2 = (byte*)&newArrayEnd;
              for (byte l = 0; l < sizeof(newArrayEnd); l++) 
              {
                INTERNAL_RAM[ARRAY_DATA_START_ADDRESS + (m * 16) + 15 - l] = ptr2[l];
              }
            } 
          }

          return 1;
        }
      }
    }
  }

  if (UINT32_END_ADDRESS - (UINT32_START_ADDRESS + 1) >= INT32_SIZE - 1) {
    for (int i = 0; i < (UINT32_END_ADDRESS - (UINT32_START_ADDRESS + 1) + 1) / INT32_SIZE; i++) {
      for (int j = 0; j < 8; j++) {
        if (INTERNAL_RAM[(UINT32_START_ADDRESS + 1 + j) + (i * INT32_SIZE)] != VARNAME.charAt(j)) {
          break;
        }

        if (j == 7) {
          uint32_t VAR_ADDRESS = (UINT32_START_ADDRESS + 1) + (i * INT32_SIZE);

          for (int i = VAR_ADDRESS; i < CHAR_END_ADDRESS; i++) {
            INTERNAL_RAM[i] = INTERNAL_RAM[i + 12];
          }

          CHAR_END_ADDRESS = CHAR_END_ADDRESS - INT32_SIZE;
          CHAR_START_ADDRESS = CHAR_START_ADDRESS - INT32_SIZE;

          BOOL_START_ADDRESS = BOOL_START_ADDRESS - INT32_SIZE;
          BOOL_END_ADDRESS = BOOL_END_ADDRESS - INT32_SIZE;

          UINT8_START_ADDRESS = UINT8_START_ADDRESS - INT32_SIZE;
          UINT8_END_ADDRESS = UINT8_END_ADDRESS - INT32_SIZE;

          INT8_START_ADDRESS = INT8_START_ADDRESS - INT32_SIZE;
          INT8_END_ADDRESS = INT8_END_ADDRESS - INT32_SIZE;

          UINT16_START_ADDRESS = UINT16_START_ADDRESS - INT32_SIZE;
          UINT16_END_ADDRESS = UINT16_END_ADDRESS - INT32_SIZE;

          INT16_START_ADDRESS = INT16_START_ADDRESS - INT32_SIZE;
          INT16_END_ADDRESS = INT16_END_ADDRESS - INT32_SIZE;

          UINT32_END_ADDRESS = UINT32_END_ADDRESS - INT32_SIZE;

          for (int i = CHAR_END_ADDRESS + 1; i < (CHAR_END_ADDRESS + 1) + 12; i++) {
            INTERNAL_RAM[i] = 0;
          }

          //GIVE EVERY ARRAY IN ARRAY DATA MEMORY SECTION NEW START AND END ADDRESSES
          for(int m = 0; m < (ARRAY_DATA_END_ADDRESS - ARRAY_DATA_START_ADDRESS) / 16; m++)
          {
            uint32_t startValue = 0;
            uint32_t endValue = 0;
            uint32_t n = 1;
            for (int k = 0; k < 4; k++) 
            {
              uint8_t valueByte = INTERNAL_RAM[ARRAY_DATA_START_ADDRESS + (m * 16) + 11 - k];

              for (int l = 0; l < 8; l++) 
              {
                uint8_t mask = 1 << l;
                if (mask & valueByte) 
                {
                  startValue = startValue + n;
                }

                n = n * 2;
              }
            }

            n = 1;
            for (int k = 0; k < 4; k++) 
            {
              uint8_t valueByte = INTERNAL_RAM[ARRAY_DATA_START_ADDRESS + (m * 16) + 15 - k];

              for (int l = 0; l < 8; l++) 
              {
                uint8_t mask = 1 << l;
                if (mask & valueByte) 
                {
                  endValue = endValue + n;
                }

                n = n * 2;
              }
            }

            if(startValue > (UINT32_START_ADDRESS + 1) + (i * INT32_SIZE))
            {
              uint32_t newArrayStart = startValue - INT32_SIZE;
              uint32_t newArrayEnd = endValue - INT32_SIZE;

              for(int l = 0; l < 8; l++)
              {
                INTERNAL_RAM[ARRAY_DATA_START_ADDRESS + (m * 16) + 15 - l] = 0;
              }

              byte* ptr1 = (byte*)&newArrayStart;
              for (byte l = 0; l < sizeof(newArrayStart); l++) 
              {
                INTERNAL_RAM[ARRAY_DATA_START_ADDRESS + (m * 16) + 11 - l] = ptr1[l];
              }

              byte* ptr2 = (byte*)&newArrayEnd;
              for (byte l = 0; l < sizeof(newArrayEnd); l++) 
              {
                INTERNAL_RAM[ARRAY_DATA_START_ADDRESS + (m * 16) + 15 - l] = ptr2[l];
              }
            } 
          }

          return 1;
        }
      }
    }
  }

  if (INT16_END_ADDRESS - (INT16_START_ADDRESS + 1) >= INT16_SIZE - 1) {
    for (int i = 0; i < (INT16_END_ADDRESS - (INT16_START_ADDRESS + 1) + 1) / INT16_SIZE; i++) {
      for (int j = 0; j < 8; j++) {
        if (INTERNAL_RAM[(INT16_START_ADDRESS + 1 + j) + (i * INT16_SIZE)] != VARNAME.charAt(j)) {
          break;
        }

        if (j == 7) {
          uint32_t VAR_ADDRESS = (INT16_START_ADDRESS + 1) + (i * INT16_SIZE);

          for (int i = VAR_ADDRESS; i < CHAR_END_ADDRESS; i++) {
            INTERNAL_RAM[i] = INTERNAL_RAM[i + 10];
          }

          CHAR_END_ADDRESS = CHAR_END_ADDRESS - 10;
          CHAR_START_ADDRESS = CHAR_START_ADDRESS - 10;

          BOOL_START_ADDRESS = BOOL_START_ADDRESS - 10;
          BOOL_END_ADDRESS = BOOL_END_ADDRESS - 10;

          UINT8_START_ADDRESS = UINT8_START_ADDRESS - 10;
          UINT8_END_ADDRESS = UINT8_END_ADDRESS - 10;

          INT8_START_ADDRESS = INT8_START_ADDRESS - 10;
          INT8_END_ADDRESS = INT8_END_ADDRESS - 10;

          UINT16_START_ADDRESS = UINT16_START_ADDRESS - 10;
          UINT16_END_ADDRESS = UINT16_END_ADDRESS - 10;

          INT16_END_ADDRESS = INT16_END_ADDRESS - 10;

          for (int i = CHAR_END_ADDRESS + 1; i < (CHAR_END_ADDRESS + 1) + 10; i++) {
            INTERNAL_RAM[i] = 0;
          }

          //GIVE EVERY ARRAY IN ARRAY DATA MEMORY SECTION NEW START AND END ADDRESSES
          for(int m = 0; m < (ARRAY_DATA_END_ADDRESS - ARRAY_DATA_START_ADDRESS) / 16; m++)
          {
            uint32_t startValue = 0;
            uint32_t endValue = 0;
            uint32_t n = 1;
            for (int k = 0; k < 4; k++) 
            {
              uint8_t valueByte = INTERNAL_RAM[ARRAY_DATA_START_ADDRESS + (m * 16) + 11 - k];

              for (int l = 0; l < 8; l++) 
              {
                uint8_t mask = 1 << l;
                if (mask & valueByte) 
                {
                  startValue = startValue + n;
                }

                n = n * 2;
              }
            }

            n = 1;
            for (int k = 0; k < 4; k++) 
            {
              uint8_t valueByte = INTERNAL_RAM[ARRAY_DATA_START_ADDRESS + (m * 16) + 15 - k];

              for (int l = 0; l < 8; l++) 
              {
                uint8_t mask = 1 << l;
                if (mask & valueByte) 
                {
                  endValue = endValue + n;
                }

                n = n * 2;
              }
            }

            if(startValue > (INT16_START_ADDRESS + 1) + (i * INT16_SIZE))
            {
              uint32_t newArrayStart = startValue - INT16_SIZE;
              uint32_t newArrayEnd = endValue - INT16_SIZE;

              for(int l = 0; l < 8; l++)
              {
                INTERNAL_RAM[ARRAY_DATA_START_ADDRESS + (m * 16) + 15 - l] = 0;
              }

              byte* ptr1 = (byte*)&newArrayStart;
              for (byte l = 0; l < sizeof(newArrayStart); l++) 
              {
                INTERNAL_RAM[ARRAY_DATA_START_ADDRESS + (m * 16) + 11 - l] = ptr1[l];
              }

              byte* ptr2 = (byte*)&newArrayEnd;
              for (byte l = 0; l < sizeof(newArrayEnd); l++) 
              {
                INTERNAL_RAM[ARRAY_DATA_START_ADDRESS + (m * 16) + 15 - l] = ptr2[l];
              }
            } 
          }

          return 1;
        }
      }
    }
  }

  if (UINT16_END_ADDRESS - (UINT16_START_ADDRESS + 1) >= INT16_SIZE - 1) {
    for (int i = 0; i < (UINT16_END_ADDRESS - (UINT16_START_ADDRESS + 1) + 1) / INT16_SIZE; i++) {
      for (int j = 0; j < 8; j++) {
        if (INTERNAL_RAM[(UINT16_START_ADDRESS + 1 + j) + (i * INT16_SIZE)] != VARNAME.charAt(j)) {
          break;
        }

        if (j == 7) {
          uint32_t VAR_ADDRESS = (UINT16_START_ADDRESS + 1) + (i * INT16_SIZE);

          for (int i = VAR_ADDRESS; i < CHAR_END_ADDRESS; i++) {
            INTERNAL_RAM[i] = INTERNAL_RAM[i + 10];
          }

          CHAR_END_ADDRESS = CHAR_END_ADDRESS - 10;
          CHAR_START_ADDRESS = CHAR_START_ADDRESS - 10;

          BOOL_START_ADDRESS = BOOL_START_ADDRESS - 10;
          BOOL_END_ADDRESS = BOOL_END_ADDRESS - 10;

          UINT8_START_ADDRESS = UINT8_START_ADDRESS - 10;
          UINT8_END_ADDRESS = UINT8_END_ADDRESS - 10;

          INT8_START_ADDRESS = INT8_START_ADDRESS - 10;
          INT8_END_ADDRESS = INT8_END_ADDRESS - 10;

          UINT16_END_ADDRESS = UINT16_END_ADDRESS - 10;

          for (int i = CHAR_END_ADDRESS + 1; i < (CHAR_END_ADDRESS + 1) + 10; i++) {
            INTERNAL_RAM[i] = 0;
          }

          //GIVE EVERY ARRAY IN ARRAY DATA MEMORY SECTION NEW START AND END ADDRESSES
          for(int m = 0; m < (ARRAY_DATA_END_ADDRESS - ARRAY_DATA_START_ADDRESS) / 16; m++)
          {
            uint32_t startValue = 0;
            uint32_t endValue = 0;
            uint32_t n = 1;
            for (int k = 0; k < 4; k++) 
            {
              uint8_t valueByte = INTERNAL_RAM[ARRAY_DATA_START_ADDRESS + (m * 16) + 11 - k];

              for (int l = 0; l < 8; l++) 
              {
                uint8_t mask = 1 << l;
                if (mask & valueByte) 
                {
                  startValue = startValue + n;
                }

                n = n * 2;
              }
            }

            n = 1;
            for (int k = 0; k < 4; k++) 
            {
              uint8_t valueByte = INTERNAL_RAM[ARRAY_DATA_START_ADDRESS + (m * 16) + 15 - k];

              for (int l = 0; l < 8; l++) 
              {
                uint8_t mask = 1 << l;
                if (mask & valueByte) 
                {
                  endValue = endValue + n;
                }

                n = n * 2;
              }
            }

            if(startValue > (UINT16_START_ADDRESS + 1) + (i * INT16_SIZE))
            {
              uint32_t newArrayStart = startValue - INT16_SIZE;
              uint32_t newArrayEnd = endValue - INT16_SIZE;

              for(int l = 0; l < 8; l++)
              {
                INTERNAL_RAM[ARRAY_DATA_START_ADDRESS + (m * 16) + 15 - l] = 0;
              }

              byte* ptr1 = (byte*)&newArrayStart;
              for (byte l = 0; l < sizeof(newArrayStart); l++) 
              {
                INTERNAL_RAM[ARRAY_DATA_START_ADDRESS + (m * 16) + 11 - l] = ptr1[l];
              }

              byte* ptr2 = (byte*)&newArrayEnd;
              for (byte l = 0; l < sizeof(newArrayEnd); l++) 
              {
                INTERNAL_RAM[ARRAY_DATA_START_ADDRESS + (m * 16) + 15 - l] = ptr2[l];
              }
            } 
          }

          return 1;
        }
      }
    }
  }

  if (INT8_END_ADDRESS - (INT8_START_ADDRESS + 1) >= INT8_SIZE - 1) {
    for (int i = 0; i < (INT8_END_ADDRESS - (INT8_START_ADDRESS + 1) + 1) / INT8_SIZE; i++) {
      for (int j = 0; j < 8; j++) {
        if (INTERNAL_RAM[(INT8_START_ADDRESS + 1 + j) + (i * INT8_SIZE)] != VARNAME.charAt(j)) {
          break;
        }

        if (j == 7) {
          uint32_t VAR_ADDRESS = (INT8_START_ADDRESS + 1) + (i * INT8_SIZE);

          for (int i = VAR_ADDRESS; i < CHAR_END_ADDRESS; i++) {
            INTERNAL_RAM[i] = INTERNAL_RAM[i + 9];
          }

          CHAR_END_ADDRESS = CHAR_END_ADDRESS - INT8_SIZE;
          CHAR_START_ADDRESS = CHAR_START_ADDRESS - INT8_SIZE;

          BOOL_START_ADDRESS = BOOL_START_ADDRESS - INT8_SIZE;
          BOOL_END_ADDRESS = BOOL_END_ADDRESS - INT8_SIZE;

          UINT8_START_ADDRESS = UINT8_START_ADDRESS - INT8_SIZE;
          UINT8_END_ADDRESS = UINT8_END_ADDRESS - INT8_SIZE;

          INT8_END_ADDRESS = INT8_END_ADDRESS - INT8_SIZE;

          for (int i = CHAR_END_ADDRESS + 1; i < (CHAR_END_ADDRESS + 1) + 9; i++) {
            INTERNAL_RAM[i] = 0;
          }

          //GIVE EVERY ARRAY IN ARRAY DATA MEMORY SECTION NEW START AND END ADDRESSES
          for(int m = 0; m < (ARRAY_DATA_END_ADDRESS - ARRAY_DATA_START_ADDRESS) / 16; m++)
          {
            uint32_t startValue = 0;
            uint32_t endValue = 0;
            uint32_t n = 1;
            for (int k = 0; k < 4; k++) 
            {
              uint8_t valueByte = INTERNAL_RAM[ARRAY_DATA_START_ADDRESS + (m * 16) + 11 - k];

              for (int l = 0; l < 8; l++) 
              {
                uint8_t mask = 1 << l;
                if (mask & valueByte) 
                {
                  startValue = startValue + n;
                }

                n = n * 2;
              }
            }

            n = 1;
            for (int k = 0; k < 4; k++) 
            {
              uint8_t valueByte = INTERNAL_RAM[ARRAY_DATA_START_ADDRESS + (m * 16) + 15 - k];

              for (int l = 0; l < 8; l++) 
              {
                uint8_t mask = 1 << l;
                if (mask & valueByte) 
                {
                  endValue = endValue + n;
                }

                n = n * 2;
              }
            }

            if(startValue > (INT8_START_ADDRESS + 1) + (i * INT8_SIZE))
            {
              uint32_t newArrayStart = startValue - INT8_SIZE;
              uint32_t newArrayEnd = endValue - INT8_SIZE;

              for(int l = 0; l < 8; l++)
              {
                INTERNAL_RAM[ARRAY_DATA_START_ADDRESS + (m * 16) + 15 - l] = 0;
              }

              byte* ptr1 = (byte*)&newArrayStart;
              for (byte l = 0; l < sizeof(newArrayStart); l++) 
              {
                INTERNAL_RAM[ARRAY_DATA_START_ADDRESS + (m * 16) + 11 - l] = ptr1[l];
              }

              byte* ptr2 = (byte*)&newArrayEnd;
              for (byte l = 0; l < sizeof(newArrayEnd); l++) 
              {
                INTERNAL_RAM[ARRAY_DATA_START_ADDRESS + (m * 16) + 15 - l] = ptr2[l];
              }
            } 
          }

          return 1;
        }
      }
    }
  }

  if (UINT8_END_ADDRESS - (UINT8_START_ADDRESS + 1) >= INT8_SIZE - 1) {
    for (int i = 0; i < (UINT8_END_ADDRESS - (UINT8_START_ADDRESS + 1) + 1) / INT8_SIZE; i++) {
      for (int j = 0; j < 8; j++) {
        if (INTERNAL_RAM[(UINT8_START_ADDRESS + 1 + j) + (i * INT8_SIZE)] != VARNAME.charAt(j)) {
          break;
        }

        if (j == 7) {
          uint32_t VAR_ADDRESS = (UINT8_START_ADDRESS + 1) + (i * INT8_SIZE);

          for (int i = VAR_ADDRESS; i < CHAR_END_ADDRESS; i++) {
            INTERNAL_RAM[i] = INTERNAL_RAM[i + 9];
          }

          CHAR_END_ADDRESS = CHAR_END_ADDRESS - INT8_SIZE;
          CHAR_START_ADDRESS = CHAR_START_ADDRESS - INT8_SIZE;

          BOOL_START_ADDRESS = BOOL_START_ADDRESS - INT8_SIZE;
          BOOL_END_ADDRESS = BOOL_END_ADDRESS - INT8_SIZE;

          UINT8_END_ADDRESS = UINT8_END_ADDRESS - INT8_SIZE;

          for (int i = CHAR_END_ADDRESS + 1; i < (CHAR_END_ADDRESS + 1) + 9; i++) {
            INTERNAL_RAM[i] = 0;
          }

          //GIVE EVERY ARRAY IN ARRAY DATA MEMORY SECTION NEW START AND END ADDRESSES
          for(int m = 0; m < (ARRAY_DATA_END_ADDRESS - ARRAY_DATA_START_ADDRESS) / 16; m++)
          {
            uint32_t startValue = 0;
            uint32_t endValue = 0;
            uint32_t n = 1;
            for (int k = 0; k < 4; k++) 
            {
              uint8_t valueByte = INTERNAL_RAM[ARRAY_DATA_START_ADDRESS + (m * 16) + 11 - k];

              for (int l = 0; l < 8; l++) 
              {
                uint8_t mask = 1 << l;
                if (mask & valueByte) 
                {
                  startValue = startValue + n;
                }

                n = n * 2;
              }
            }

            n = 1;
            for (int k = 0; k < 4; k++) 
            {
              uint8_t valueByte = INTERNAL_RAM[ARRAY_DATA_START_ADDRESS + (m * 16) + 15 - k];

              for (int l = 0; l < 8; l++) 
              {
                uint8_t mask = 1 << l;
                if (mask & valueByte) 
                {
                  endValue = endValue + n;
                }

                n = n * 2;
              }
            }

            if(startValue > (UINT8_START_ADDRESS + 1) + (i * INT8_SIZE))
            {
              uint32_t newArrayStart = startValue - INT8_SIZE;
              uint32_t newArrayEnd = endValue - INT8_SIZE;

              for(int l = 0; l < 8; l++)
              {
                INTERNAL_RAM[ARRAY_DATA_START_ADDRESS + (m * 16) + 15 - l] = 0;
              }

              byte* ptr1 = (byte*)&newArrayStart;
              for (byte l = 0; l < sizeof(newArrayStart); l++) 
              {
                INTERNAL_RAM[ARRAY_DATA_START_ADDRESS + (m * 16) + 11 - l] = ptr1[l];
              }

              byte* ptr2 = (byte*)&newArrayEnd;
              for (byte l = 0; l < sizeof(newArrayEnd); l++) 
              {
                INTERNAL_RAM[ARRAY_DATA_START_ADDRESS + (m * 16) + 15 - l] = ptr2[l];
              }
            } 
          }

          return 1;
        }
      }
    }
  }

  if (BOOL_END_ADDRESS - (BOOL_START_ADDRESS + 1) >= BOOL_SIZE - 1) {
    for (int i = 0; i < (BOOL_END_ADDRESS - (BOOL_START_ADDRESS + 1) + 1) / BOOL_SIZE; i++) {
      for (int j = 0; j < 8; j++) {
        if (INTERNAL_RAM[(BOOL_START_ADDRESS + 1 + j) + (i * BOOL_SIZE)] != VARNAME.charAt(j)) {
          break;
        }

        if (j == 7) {
          uint32_t VAR_ADDRESS = (BOOL_START_ADDRESS + 1) + (i * BOOL_SIZE);

          for (int i = VAR_ADDRESS; i < CHAR_END_ADDRESS; i++) {
            INTERNAL_RAM[i] = INTERNAL_RAM[i + 9];
          }

          CHAR_END_ADDRESS = CHAR_END_ADDRESS - BOOL_SIZE;
          CHAR_START_ADDRESS = CHAR_START_ADDRESS - BOOL_SIZE;

          BOOL_START_ADDRESS = BOOL_START_ADDRESS - BOOL_SIZE;

          for (int i = CHAR_END_ADDRESS + 1; i < (CHAR_END_ADDRESS + 1) + 9; i++) {
            INTERNAL_RAM[i] = 0;
          }

          //GIVE EVERY ARRAY IN ARRAY DATA MEMORY SECTION NEW START AND END ADDRESSES
          for(int m = 0; m < (ARRAY_DATA_END_ADDRESS - ARRAY_DATA_START_ADDRESS) / 16; m++)
          {
            uint32_t startValue = 0;
            uint32_t endValue = 0;
            uint32_t n = 1;
            for (int k = 0; k < 4; k++) 
            {
              uint8_t valueByte = INTERNAL_RAM[ARRAY_DATA_START_ADDRESS + (m * 16) + 11 - k];

              for (int l = 0; l < 8; l++) 
              {
                uint8_t mask = 1 << l;
                if (mask & valueByte) 
                {
                  startValue = startValue + n;
                }

                n = n * 2;
              }
            }

            n = 1;
            for (int k = 0; k < 4; k++) 
            {
              uint8_t valueByte = INTERNAL_RAM[ARRAY_DATA_START_ADDRESS + (m * 16) + 15 - k];

              for (int l = 0; l < 8; l++) 
              {
                uint8_t mask = 1 << l;
                if (mask & valueByte) 
                {
                  endValue = endValue + n;
                }

                n = n * 2;
              }
            }

            if(startValue > (BOOL_START_ADDRESS + 1) + (i * BOOL_SIZE))
            {
              uint32_t newArrayStart = startValue - BOOL_SIZE;
              uint32_t newArrayEnd = endValue - BOOL_SIZE;

              for(int l = 0; l < 8; l++)
              {
                INTERNAL_RAM[ARRAY_DATA_START_ADDRESS + (m * 16) + 15 - l] = 0;
              }

              byte* ptr1 = (byte*)&newArrayStart;
              for (byte l = 0; l < sizeof(newArrayStart); l++) 
              {
                INTERNAL_RAM[ARRAY_DATA_START_ADDRESS + (m * 16) + 11 - l] = ptr1[l];
              }

              byte* ptr2 = (byte*)&newArrayEnd;
              for (byte l = 0; l < sizeof(newArrayEnd); l++) 
              {
                INTERNAL_RAM[ARRAY_DATA_START_ADDRESS + (m * 16) + 15 - l] = ptr2[l];
              }
            } 
          }

          return 1;
        }
      }
    }
  }

  if (CHAR_END_ADDRESS - (CHAR_START_ADDRESS + 1) >= CHAR_SIZE - 1) {
    for (int i = 0; i < (CHAR_END_ADDRESS - (CHAR_START_ADDRESS + 1) + 1) / CHAR_SIZE; i++) {
      for (int j = 0; j < 8; j++) {
        if (INTERNAL_RAM[(CHAR_START_ADDRESS + 1 + j) + (i * CHAR_SIZE)] != VARNAME.charAt(j)) {
          break;
        }

        if (j == 7) {
          uint32_t VAR_ADDRESS = (CHAR_START_ADDRESS + 1) + (i * CHAR_SIZE);

          for (int i = VAR_ADDRESS; i < CHAR_END_ADDRESS; i++) {
            INTERNAL_RAM[i] = INTERNAL_RAM[i + 9];
          }

          CHAR_END_ADDRESS = CHAR_END_ADDRESS - CHAR_SIZE;

          for (int i = CHAR_END_ADDRESS + 1; i < (CHAR_END_ADDRESS + 1) + 9; i++) {
            INTERNAL_RAM[i] = 0;
          }

          //GIVE EVERY ARRAY IN ARRAY DATA MEMORY SECTION NEW START AND END ADDRESSES
          for(int m = 0; m < (ARRAY_DATA_END_ADDRESS - ARRAY_DATA_START_ADDRESS) / 16; m++)
          {
            uint32_t startValue = 0;
            uint32_t endValue = 0;
            uint32_t n = 1;
            for (int k = 0; k < 4; k++) 
            {
              uint8_t valueByte = INTERNAL_RAM[ARRAY_DATA_START_ADDRESS + (m * 16) + 11 - k];

              for (int l = 0; l < 8; l++) 
              {
                uint8_t mask = 1 << l;
                if (mask & valueByte) 
                {
                  startValue = startValue + n;
                }

                n = n * 2;
              }
            }

            n = 1;
            for (int k = 0; k < 4; k++) 
            {
              uint8_t valueByte = INTERNAL_RAM[ARRAY_DATA_START_ADDRESS + (m * 16) + 15 - k];

              for (int l = 0; l < 8; l++) 
              {
                uint8_t mask = 1 << l;
                if (mask & valueByte) 
                {
                  endValue = endValue + n;
                }

                n = n * 2;
              }
            }

            if(startValue > (CHAR_START_ADDRESS + 1) + (i * CHAR_SIZE))
            {
              uint32_t newArrayStart = startValue - CHAR_SIZE;
              uint32_t newArrayEnd = endValue - CHAR_SIZE;

              for(int l = 0; l < 8; l++)
              {
                INTERNAL_RAM[ARRAY_DATA_START_ADDRESS + (m * 16) + 15 - l] = 0;
              }

              byte* ptr1 = (byte*)&newArrayStart;
              for (byte l = 0; l < sizeof(newArrayStart); l++) 
              {
                INTERNAL_RAM[ARRAY_DATA_START_ADDRESS + (m * 16) + 11 - l] = ptr1[l];
              }

              byte* ptr2 = (byte*)&newArrayEnd;
              for (byte l = 0; l < sizeof(newArrayEnd); l++) 
              {
                INTERNAL_RAM[ARRAY_DATA_START_ADDRESS + (m * 16) + 15 - l] = ptr2[l];
              }
            } 
          }

          return 1;
        }
      }
    }
  }

  return -1;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------


//----------------------------DOUBLE FUNCTIONS------------------------------------------------------------------------------------------------------------------------

int ALLOC_DOUBLE() 
{

  for (int j = CHAR_END_ADDRESS; j >= DOUBLE_END_ADDRESS + 1; j--) {
    INTERNAL_RAM[j + DOUBLE_SIZE] = INTERNAL_RAM[j];
  }

  DOUBLE_END_ADDRESS = DOUBLE_END_ADDRESS + DOUBLE_SIZE;
  for (int i = DOUBLE_END_ADDRESS; i > (DOUBLE_END_ADDRESS - DOUBLE_SIZE); i--) {
    INTERNAL_RAM[i] = 0;
  }

  CHAR_END_ADDRESS = CHAR_END_ADDRESS + DOUBLE_SIZE;
  CHAR_START_ADDRESS = CHAR_START_ADDRESS + DOUBLE_SIZE;

  BOOL_START_ADDRESS = BOOL_START_ADDRESS + DOUBLE_SIZE;
  BOOL_END_ADDRESS = BOOL_END_ADDRESS + DOUBLE_SIZE;

  UINT8_START_ADDRESS = UINT8_START_ADDRESS + DOUBLE_SIZE;
  UINT8_END_ADDRESS = UINT8_END_ADDRESS + DOUBLE_SIZE;

  INT8_START_ADDRESS = INT8_START_ADDRESS + DOUBLE_SIZE;
  INT8_END_ADDRESS = INT8_END_ADDRESS + DOUBLE_SIZE;

  UINT16_START_ADDRESS = UINT16_START_ADDRESS + DOUBLE_SIZE;
  UINT16_END_ADDRESS = UINT16_END_ADDRESS + DOUBLE_SIZE;

  INT16_START_ADDRESS = INT16_START_ADDRESS + DOUBLE_SIZE;
  INT16_END_ADDRESS = INT16_END_ADDRESS + DOUBLE_SIZE;

  UINT32_START_ADDRESS = UINT32_START_ADDRESS + DOUBLE_SIZE;
  UINT32_END_ADDRESS = UINT32_END_ADDRESS + DOUBLE_SIZE;

  INT32_START_ADDRESS = INT32_START_ADDRESS + DOUBLE_SIZE;
  INT32_END_ADDRESS = INT32_END_ADDRESS + DOUBLE_SIZE;

  UINT64_START_ADDRESS = UINT64_START_ADDRESS + DOUBLE_SIZE;
  UINT64_END_ADDRESS = UINT64_END_ADDRESS + DOUBLE_SIZE;

  INT64_START_ADDRESS = INT64_START_ADDRESS + DOUBLE_SIZE;
  INT64_END_ADDRESS = INT64_END_ADDRESS + DOUBLE_SIZE;

  return 1;
}

int8_t ALLOC_DOUBLE_ARRAY(String ARRAY_NAME, uint16_t ARRAY_LENGTH)  //RETURNS -1 IF OPERATION WAS  UNSUCCESSFULL, RETURNS 1 IF OPERATION WAS SUCCESSFULL.
{
  ARRAY_NAME.trim();

  if(CHECK_VARIABLE_EXISTS(ARRAY_NAME, "NULL") != -1) 
  {
    return -1;
  }

  if(ARRAY_LENGTH > 2000 || ARRAY_NAME.length() != 8 || (ARRAY_DATA_START_ADDRESS - 1) - (CHAR_END_ADDRESS + 1) < 16)
  {
    return -1;
  }

  uint32_t ARRAY_START = DOUBLE_END_ADDRESS + 1;

  for(int i = 0; i < ARRAY_LENGTH; i++)
  {
    ALLOC_DOUBLE();
  }

  uint32_t ARRAY_END = DOUBLE_END_ADDRESS;

  ARRAY_DATA_START_ADDRESS = ARRAY_DATA_START_ADDRESS - 16;

  for(int i = ARRAY_DATA_START_ADDRESS + 16; i <= ARRAY_DATA_END_ADDRESS; i++)
  {
    INTERNAL_RAM[i - 16] = INTERNAL_RAM[i];
  }

  for(int i = 0; i < 16; i++)
  {
    INTERNAL_RAM[(ARRAY_DATA_END_ADDRESS - 16) + i] = 0;
  }

  //FROM HERE IT WILL WRITE ARRAY NAME AND START AND END ADDRESSES
  for(int i = 0; i < 8; i++)
  {
    INTERNAL_RAM[(ARRAY_DATA_END_ADDRESS - 16) + i] = ARRAY_NAME.charAt(i);
  }

  byte* ptr1 = (byte*)&ARRAY_START;
  for (byte i = 0; i < sizeof(ARRAY_START); i++) 
  {
    INTERNAL_RAM[(ARRAY_DATA_END_ADDRESS - 8) + i] = ptr1[3 - i];
  }

  byte* ptr2 = (byte*)&ARRAY_END;
  for (byte i = 0; i < sizeof(ARRAY_END); i++) 
  {
    INTERNAL_RAM[(ARRAY_DATA_END_ADDRESS - 4) + i] = ptr2[3 - i];
  }

  return 1;
}

//ALL VARIABLE WRITES BELOW RETURN 1 IF OPERATION SUCCEDED AND -1 IF OPERATION FAILED DUE TO VARIABLE ALREADY EXISTING.
int8_t WRITE_DOUBLE_INTERNAL(String VARNAME, double VALUE) {
  VARNAME.trim();

  while(VARNAME.length() < 8)
  {
    VARNAME += "_";
  }

  if(CHECK_VARIABLE_EXISTS(VARNAME, "NULL") != -1) 
  {
    return -1;
  }

  ALLOC_DOUBLE();
  for (int i = 0; i < 8; i++) {
    INTERNAL_RAM[(DOUBLE_END_ADDRESS - 15) + i] = VARNAME.charAt(i);
  }

  byte* ptr = (byte*)&VALUE;
  for (byte i = 0; i < sizeof(VALUE); i++) {
    INTERNAL_RAM[DOUBLE_END_ADDRESS - i] = ptr[i];
  }

  return 1;
}

//RETURNS -1 IF VARIABLE/ARRAY WITH VARNAME NAME DOES NOT EXIST. ARRAY_INDEX MUST BE NEGATIVE IF YOU ARE NOT TRYING TO WRITE TO ARRAYS.
int8_t STORE_IN_DOUBLE(String VARNAME, int32_t ARRAY_INDEX, double VALUE)
{
  VARNAME.trim();
  int32_t VAR_ADDRESS = CHECK_VARIABLE_EXISTS(VARNAME, "DOUBLE");
  int32_t ARRAY_START = GET_ARRAY_INFO(VARNAME, "DOUBLE", 0);
  int32_t ARRAY_END = GET_ARRAY_INFO(VARNAME, "DOUBLE", 1);
  uint8_t ARRAY_ELEMENT_SIZE;
  uint32_t ARRAY_LENGTH;

  

  if(VAR_ADDRESS < 0) 
  {
    return -3; // NO SUCH ARRAY OR VARIABLE WITH PROVIDED NAME EXISTS
  }

  if(ARRAY_INDEX > 0 && ARRAY_START == -1)
  {
    return -1; // VARIABLE ISN`T ARRAY
  }

  if(ARRAY_INDEX < 0 && ARRAY_START > -1)
  {
    return -1; // ARRAY ISNT VARIABLE LOL
  }

  if(ARRAY_START > -1)
  {

    if(ARRAY_END <= DOUBLE_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 16;
    }else if(ARRAY_END <= INT64_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 16;
    }else if(ARRAY_END <= UINT64_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 16;
    }else if(ARRAY_END <= INT32_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 12;
    }else if(ARRAY_END <= UINT32_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 12;
    }else if(ARRAY_END <= INT16_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 10;
    }else if(ARRAY_END <= UINT16_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 10;
    }else if(ARRAY_END <= INT8_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 9;
    }else if(ARRAY_END <= UINT8_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 9;
    }else if(ARRAY_END <= BOOL_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 9;
    }else if(ARRAY_END <= CHAR_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 9;
    }

    ARRAY_LENGTH = (ARRAY_END - ARRAY_START + 1) / ARRAY_ELEMENT_SIZE;

    if(ARRAY_INDEX > ARRAY_LENGTH - 1)
    {
      return -2; // TRYING TO ACCESS OUT OF ARRAY BOUNDS
    }

    byte* ptr = (byte*)&VALUE;
    for (byte i = 0; i < sizeof(VALUE); i++) 
    {
      INTERNAL_RAM[(ARRAY_START + (ARRAY_INDEX * ARRAY_ELEMENT_SIZE) + 15) - i] = ptr[i];
    }
  }

  if(ARRAY_START < 0)
  {
    byte* ptr = (byte*)&VALUE;
    for (byte i = 0; i < sizeof(VALUE); i++) 
    {
      INTERNAL_RAM[(VAR_ADDRESS + 15) - i] = ptr[i];
    }
  }

  return 1;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------


//----------------------------INT64 FUNCTIONS------------------------------------------------------------------------------------------------------------------------

int ALLOC_INT64() 
{
  for (int j = CHAR_END_ADDRESS; j >= INT64_END_ADDRESS + 1; j--) 
  {
    INTERNAL_RAM[j + INT64_SIZE] = INTERNAL_RAM[j];
  }

  INT64_END_ADDRESS = INT64_END_ADDRESS + INT64_SIZE;
  for (int i = INT64_END_ADDRESS; i > (INT64_END_ADDRESS - INT64_SIZE); i--) 
  {
    INTERNAL_RAM[i] = 0;
  }


  CHAR_END_ADDRESS = CHAR_END_ADDRESS + INT64_SIZE;
  CHAR_START_ADDRESS = CHAR_START_ADDRESS + INT64_SIZE;

  BOOL_START_ADDRESS = BOOL_START_ADDRESS + INT64_SIZE;
  BOOL_END_ADDRESS = BOOL_END_ADDRESS + INT64_SIZE;

  UINT8_START_ADDRESS = UINT8_START_ADDRESS + INT64_SIZE;
  UINT8_END_ADDRESS = UINT8_END_ADDRESS + INT64_SIZE;

  INT8_START_ADDRESS = INT8_START_ADDRESS + INT64_SIZE;
  INT8_END_ADDRESS = INT8_END_ADDRESS + INT64_SIZE;

  UINT16_START_ADDRESS = UINT16_START_ADDRESS + INT64_SIZE;
  UINT16_END_ADDRESS = UINT16_END_ADDRESS + INT64_SIZE;

  INT16_START_ADDRESS = INT16_START_ADDRESS + INT64_SIZE;
  INT16_END_ADDRESS = INT16_END_ADDRESS + INT64_SIZE;

  UINT32_START_ADDRESS = UINT32_START_ADDRESS + INT64_SIZE;
  UINT32_END_ADDRESS = UINT32_END_ADDRESS + INT64_SIZE;

  INT32_START_ADDRESS = INT32_START_ADDRESS + INT64_SIZE;
  INT32_END_ADDRESS = INT32_END_ADDRESS + INT64_SIZE;

  UINT64_START_ADDRESS = UINT64_START_ADDRESS + INT64_SIZE;
  UINT64_END_ADDRESS = UINT64_END_ADDRESS + INT64_SIZE;

  return 1;
}

int8_t ALLOC_INT64_ARRAY(String ARRAY_NAME, uint16_t ARRAY_LENGTH)  //RETURNS -1 IF OPERATION WAS NOT SUCCESSFULL, RETURNS 1 IF OPERATION WAS SUCCESSFULL.
{
  ARRAY_NAME.trim();

  if(CHECK_VARIABLE_EXISTS(ARRAY_NAME, "NULL") != -1) 
  {
    return -1;
  }

  if(ARRAY_LENGTH > 2000 || ARRAY_NAME.length() != 8 || (ARRAY_DATA_START_ADDRESS - 1) - (CHAR_END_ADDRESS + 1) < 16)
  {
    return -1;
  }

  uint32_t ARRAY_START = INT64_END_ADDRESS + 1; //SAVE THE NEW ARRAY START ADDRESS

  //ALLOCATE ARRAY WITH LENGTH GIVEN IN FUNCTION ARGS
  for(int i = 0; i < ARRAY_LENGTH; i++)
  {
    ALLOC_INT64();
  }

  uint32_t ARRAY_END = INT64_END_ADDRESS; //SAVE THE NEW ARRAY END ADDRESS

  ARRAY_DATA_START_ADDRESS = ARRAY_DATA_START_ADDRESS - 16;

  //ALLOCATE ARRAY DATA AREA
  for(int i = ARRAY_DATA_START_ADDRESS + 16; i <= ARRAY_DATA_END_ADDRESS; i++)
  {
    INTERNAL_RAM[i - 16] = INTERNAL_RAM[i];
  }

  //CLEANUP NEWLY ALLOCATED ARRAY DATA AREA
  for(int i = 0; i < 16; i++)
  {
    INTERNAL_RAM[(ARRAY_DATA_END_ADDRESS - 16) + i] = 0;
  }

  //FROM HERE TO THE END WE WILL WRITE ARRAY NAME AND START AND END ADDRESSES
  for(int i = 0; i < 8; i++)
  {
    INTERNAL_RAM[(ARRAY_DATA_END_ADDRESS - 16) + i] = ARRAY_NAME.charAt(i);
  }

  //WRITE NEW ARRAY START ADDRESS AT ARRAY DATA START ADDRESS
  byte* ptr1 = (byte*)&ARRAY_START;
  for (byte i = 0; i < sizeof(ARRAY_START); i++) 
  {
    INTERNAL_RAM[(ARRAY_DATA_END_ADDRESS - 8) + i] = ptr1[3 - i];
  }

  //WRITE NEW ARRAY END ADDRESS AT ARRAY DATA END ADDRESS
  byte* ptr2 = (byte*)&ARRAY_END;
  for (byte i = 0; i < sizeof(ARRAY_END); i++) 
  {
    INTERNAL_RAM[(ARRAY_DATA_END_ADDRESS - 4) + i] = ptr2[3 - i];
  }

  return 1;
}

int8_t WRITE_INT64_INTERNAL(String VARNAME, int64_t VALUE) 
{
  VARNAME.trim();

  while(VARNAME.length() < 8)
  {
    VARNAME += "_";
  }

  if (CHECK_VARIABLE_EXISTS(VARNAME, "NULL") != -1) {
    return -1;
  }

  ALLOC_INT64();
  for (int i = 0; i < 8; i++) 
  {
    INTERNAL_RAM[(INT64_END_ADDRESS - 15) + i] = VARNAME.charAt(i);
  }

  byte* ptr = (byte*)&VALUE;
  for (byte i = 0; i < sizeof(VALUE); i++) 
  {
    INTERNAL_RAM[INT64_END_ADDRESS - i] = ptr[i];
  }

  return 1;
}

//RETURNS -1 IF VARIABLE/ARRAY WITH VARNAME NAME DOES NOT EXIST. ARRAY_INDEX MUST BE NEGATIVE IF YOU ARE NOT TRYING TO WRITE TO ARRAYS.
int8_t STORE_IN_INT64(String VARNAME, int32_t ARRAY_INDEX, int64_t VALUE)
{
  VARNAME.trim();
  int32_t VAR_ADDRESS = CHECK_VARIABLE_EXISTS(VARNAME, "INT64");
  int32_t ARRAY_START = GET_ARRAY_INFO(VARNAME, "INT64", 0);
  int32_t ARRAY_END = GET_ARRAY_INFO(VARNAME, "INT64", 1);
  uint8_t ARRAY_ELEMENT_SIZE;
  uint32_t ARRAY_LENGTH;

  

  if(VAR_ADDRESS < 0) 
  {
    return -3; // NO SUCH ARRAY OR VARIABLE WITH PROVIDED NAME EXISTS
  }

  if(ARRAY_INDEX > 0 && ARRAY_START == -1)
  {
    return -1; // VARIABLE ISN`T ARRAY
  }

  if(ARRAY_INDEX < 0 && ARRAY_START > -1)
  {
    return -1; // ARRAY ISNT VARIABLE LOL
  }

  if(ARRAY_START > -1)
  {

    if(ARRAY_END <= DOUBLE_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 16;
    }else if(ARRAY_END <= INT64_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 16;
    }else if(ARRAY_END <= UINT64_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 16;
    }else if(ARRAY_END <= INT32_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 12;
    }else if(ARRAY_END <= UINT32_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 12;
    }else if(ARRAY_END <= INT16_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 10;
    }else if(ARRAY_END <= UINT16_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 10;
    }else if(ARRAY_END <= INT8_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 9;
    }else if(ARRAY_END <= UINT8_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 9;
    }else if(ARRAY_END <= BOOL_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 9;
    }else if(ARRAY_END <= CHAR_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 9;
    }

    ARRAY_LENGTH = (ARRAY_END - ARRAY_START + 1) / ARRAY_ELEMENT_SIZE;

    if(ARRAY_INDEX > ARRAY_LENGTH - 1)
    {
      return -2; // TRYING TO ACCESS OUT OF ARRAY BOUNDS
    }

    byte* ptr = (byte*)&VALUE;
    for (byte i = 0; i < sizeof(VALUE); i++) 
    {
      INTERNAL_RAM[(ARRAY_START + (ARRAY_INDEX * ARRAY_ELEMENT_SIZE) + 15) - i] = ptr[i];
    }
  }

  if(ARRAY_START < 0)
  {
    byte* ptr = (byte*)&VALUE;
    for (byte i = 0; i < sizeof(VALUE); i++) 
    {
      INTERNAL_RAM[(VAR_ADDRESS + 15) - i] = ptr[i];
    }
  }

  return 1;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------


//----------------------------UINT64 FUNCTIONS------------------------------------------------------------------------------------------------------------------------

int ALLOC_UINT64() 
{

  for (int j = CHAR_END_ADDRESS; j >= UINT64_END_ADDRESS + 1; j--) 
  {
    INTERNAL_RAM[j + INT64_SIZE] = INTERNAL_RAM[j];
  }

  UINT64_END_ADDRESS = UINT64_END_ADDRESS + INT64_SIZE;
  for (int i = UINT64_END_ADDRESS; i > (UINT64_END_ADDRESS - INT64_SIZE); i--) 
  {
    INTERNAL_RAM[i] = 0;
  }

  CHAR_END_ADDRESS = CHAR_END_ADDRESS + INT64_SIZE;
  CHAR_START_ADDRESS = CHAR_START_ADDRESS + INT64_SIZE;

  BOOL_START_ADDRESS = BOOL_START_ADDRESS + INT64_SIZE;
  BOOL_END_ADDRESS = BOOL_END_ADDRESS + INT64_SIZE;

  UINT8_START_ADDRESS = UINT8_START_ADDRESS + INT64_SIZE;
  UINT8_END_ADDRESS = UINT8_END_ADDRESS + INT64_SIZE;

  INT8_START_ADDRESS = INT8_START_ADDRESS + INT64_SIZE;
  INT8_END_ADDRESS = INT8_END_ADDRESS + INT64_SIZE;

  UINT16_START_ADDRESS = UINT16_START_ADDRESS + INT64_SIZE;
  UINT16_END_ADDRESS = UINT16_END_ADDRESS + INT64_SIZE;

  INT16_START_ADDRESS = INT16_START_ADDRESS + INT64_SIZE;
  INT16_END_ADDRESS = INT16_END_ADDRESS + INT64_SIZE;

  UINT32_START_ADDRESS = UINT32_START_ADDRESS + INT64_SIZE;
  UINT32_END_ADDRESS = UINT32_END_ADDRESS + INT64_SIZE;

  INT32_START_ADDRESS = INT32_START_ADDRESS + INT64_SIZE;
  INT32_END_ADDRESS = INT32_END_ADDRESS + INT64_SIZE;

  return 1;
}

int8_t ALLOC_UINT64_ARRAY(String ARRAY_NAME, uint16_t ARRAY_LENGTH)  //RETURNS -1 IF OPERATION WAS NOT SUCCESSFULL, RETURNS 1 IF OPERATION WAS SUCCESSFULL.
{
  ARRAY_NAME.trim();

  if(CHECK_VARIABLE_EXISTS(ARRAY_NAME, "NULL") != -1) 
  {
    return -1;
  }

  if(ARRAY_LENGTH > 2000 || ARRAY_NAME.length() != 8 || (ARRAY_DATA_START_ADDRESS - 1) - (CHAR_END_ADDRESS + 1) < 16)
  {
    return -1;
  }

  uint32_t ARRAY_START = UINT64_END_ADDRESS + 1; //SAVE THE NEW ARRAY START ADDRESS

  //ALLOCATE ARRAY WITH LENGTH GIVEN IN FUNCTION ARGS
  for(int i = 0; i < ARRAY_LENGTH; i++)
  {
    ALLOC_UINT64();
  }

  uint32_t ARRAY_END = UINT64_END_ADDRESS; //SAVE THE NEW ARRAY END ADDRESS

  ARRAY_DATA_START_ADDRESS = ARRAY_DATA_START_ADDRESS - 16;

  //ALLOCATE ARRAY DATA AREA
  for(int i = ARRAY_DATA_START_ADDRESS + 16; i <= ARRAY_DATA_END_ADDRESS; i++)
  {
    INTERNAL_RAM[i - 16] = INTERNAL_RAM[i];
  }

  //CLEANUP NEWLY ALLOCATED ARRAY DATA AREA
  for(int i = 0; i < 16; i++)
  {
    INTERNAL_RAM[(ARRAY_DATA_END_ADDRESS - 16) + i] = 0;
  }

  //FROM HERE TO THE END WE WILL WRITE ARRAY NAME AND START AND END ADDRESSES
  for(int i = 0; i < 8; i++)
  {
    INTERNAL_RAM[(ARRAY_DATA_END_ADDRESS - 16) + i] = ARRAY_NAME.charAt(i);
  }

  //WRITE NEW ARRAY START ADDRESS AT ARRAY DATA START ADDRESS
  byte* ptr1 = (byte*)&ARRAY_START;
  for (byte i = 0; i < sizeof(ARRAY_START); i++) 
  {
    INTERNAL_RAM[(ARRAY_DATA_END_ADDRESS - 8) + i] = ptr1[3 - i];
  }

  //WRITE NEW ARRAY END ADDRESS AT ARRAY DATA END ADDRESS
  byte* ptr2 = (byte*)&ARRAY_END;
  for (byte i = 0; i < sizeof(ARRAY_END); i++) 
  {
    INTERNAL_RAM[(ARRAY_DATA_END_ADDRESS - 4) + i] = ptr2[3 - i];
  }

  return 1;
}

int8_t WRITE_UINT64_INTERNAL(String VARNAME, uint64_t VALUE) 
{
  VARNAME.trim();

  while(VARNAME.length() < 8)
  {
    VARNAME += "_";
  }

  if (CHECK_VARIABLE_EXISTS(VARNAME, "NULL") != -1) 
  {
    return -1;
  }

  ALLOC_UINT64();
  for (int i = 0; i < 8; i++) 
  {
    INTERNAL_RAM[(UINT64_END_ADDRESS - 15) + i] = VARNAME.charAt(i);
  }

  byte* ptr = (byte*)&VALUE;
  for (byte i = 0; i < sizeof(VALUE); i++) 
  {
    INTERNAL_RAM[UINT64_END_ADDRESS - i] = ptr[i];
  }

  return 1;
}

//RETURNS -1 IF VARIABLE/ARRAY WITH VARNAME NAME DOES NOT EXIST. ARRAY_INDEX MUST BE NEGATIVE IF YOU ARE NOT TRYING TO WRITE TO ARRAYS.
int8_t STORE_IN_UINT64(String VARNAME, int32_t ARRAY_INDEX, uint64_t VALUE)
{
  VARNAME.trim();
  int32_t VAR_ADDRESS = CHECK_VARIABLE_EXISTS(VARNAME, "UINT64");
  int32_t ARRAY_START = GET_ARRAY_INFO(VARNAME, "UINT64", 0);
  int32_t ARRAY_END = GET_ARRAY_INFO(VARNAME, "UINT64", 1);
  uint8_t ARRAY_ELEMENT_SIZE;
  uint32_t ARRAY_LENGTH;

  

  if(VAR_ADDRESS < 0) 
  {
    return -3; // NO SUCH ARRAY OR VARIABLE WITH PROVIDED NAME EXISTS
  }

  if(ARRAY_INDEX > 0 && ARRAY_START == -1)
  {
    return -1; // VARIABLE ISN`T ARRAY
  }

  if(ARRAY_INDEX < 0 && ARRAY_START > -1)
  {
    return -1; // ARRAY ISNT VARIABLE LOL
  }

  if(ARRAY_START > -1)
  {

    if(ARRAY_END <= DOUBLE_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 16;
    }else if(ARRAY_END <= INT64_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 16;
    }else if(ARRAY_END <= UINT64_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 16;
    }else if(ARRAY_END <= INT32_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 12;
    }else if(ARRAY_END <= UINT32_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 12;
    }else if(ARRAY_END <= INT16_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 10;
    }else if(ARRAY_END <= UINT16_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 10;
    }else if(ARRAY_END <= INT8_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 9;
    }else if(ARRAY_END <= UINT8_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 9;
    }else if(ARRAY_END <= BOOL_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 9;
    }else if(ARRAY_END <= CHAR_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 9;
    }

    ARRAY_LENGTH = (ARRAY_END - ARRAY_START + 1) / ARRAY_ELEMENT_SIZE;

    if(ARRAY_INDEX > ARRAY_LENGTH - 1)
    {
      return -2; // TRYING TO ACCESS OUT OF ARRAY BOUNDS
    }

    byte* ptr = (byte*)&VALUE;
    for (byte i = 0; i < sizeof(VALUE); i++) 
    {
      INTERNAL_RAM[(ARRAY_START + (ARRAY_INDEX * ARRAY_ELEMENT_SIZE) + 15) - i] = ptr[i];
    }
  }

  if(ARRAY_START < 0)
  {
    byte* ptr = (byte*)&VALUE;
    for (byte i = 0; i < sizeof(VALUE); i++) 
    {
      INTERNAL_RAM[(VAR_ADDRESS + 15) - i] = ptr[i];
    }
  }

  return 1;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------


//----------------------------INT32 FUNCTIONS------------------------------------------------------------------------------------------------------------------------

int ALLOC_INT32() {

  for (int j = CHAR_END_ADDRESS; j >= INT32_END_ADDRESS + 1; j--) {
    INTERNAL_RAM[j + INT32_SIZE] = INTERNAL_RAM[j];
  }

  INT32_END_ADDRESS = INT32_END_ADDRESS + INT32_SIZE;
  for (int i = INT32_END_ADDRESS; i > (INT32_END_ADDRESS - INT32_SIZE); i--) {
    INTERNAL_RAM[i] = 0;
  }

  CHAR_END_ADDRESS = CHAR_END_ADDRESS + INT32_SIZE;
  CHAR_START_ADDRESS = CHAR_START_ADDRESS + INT32_SIZE;

  BOOL_START_ADDRESS = BOOL_START_ADDRESS + INT32_SIZE;
  BOOL_END_ADDRESS = BOOL_END_ADDRESS + INT32_SIZE;

  UINT8_START_ADDRESS = UINT8_START_ADDRESS + INT32_SIZE;
  UINT8_END_ADDRESS = UINT8_END_ADDRESS + INT32_SIZE;

  INT8_START_ADDRESS = INT8_START_ADDRESS + INT32_SIZE;
  INT8_END_ADDRESS = INT8_END_ADDRESS + INT32_SIZE;

  UINT16_START_ADDRESS = UINT16_START_ADDRESS + INT32_SIZE;
  UINT16_END_ADDRESS = UINT16_END_ADDRESS + INT32_SIZE;

  INT16_START_ADDRESS = INT16_START_ADDRESS + INT32_SIZE;
  INT16_END_ADDRESS = INT16_END_ADDRESS + INT32_SIZE;

  UINT32_START_ADDRESS = UINT32_START_ADDRESS + INT32_SIZE;
  UINT32_END_ADDRESS = UINT32_END_ADDRESS + INT32_SIZE;

  return 1;
}

int8_t ALLOC_INT32_ARRAY(String ARRAY_NAME, uint16_t ARRAY_LENGTH)  //RETURNS -1 IF OPERATION WAS  UNSUCCESSFULL, RETURNS 1 IF OPERATION WAS SUCCESSFULL.
{
  ARRAY_NAME.trim();

  // JUST CHECKING IF VARIABLES WITH THIS NAME ALREADY EXIST
  if(CHECK_VARIABLE_EXISTS(ARRAY_NAME, "NULL") != -1) 
  {
    return -1;
  }

  //IF ARRAY IS LARGER THAN 2000, OR IF ARRAY_NAME HAS WRONG SIZE, OR IF MEMORY IS FULL, ERROR THIS SH*T 
  if(ARRAY_LENGTH > 2000 || ARRAY_NAME.length() != 8 || (ARRAY_DATA_START_ADDRESS - 1) - (CHAR_END_ADDRESS + 1) < 16)
  {
    return -1;
  }

  //ALLOCATING ARRAY SPACE AND SAVING ITS START AND END ADDRESSES
  uint32_t ARRAY_START = INT32_END_ADDRESS + 1;

  for(int i = 0; i < ARRAY_LENGTH; i++)
  {
    ALLOC_INT32();
  }

  uint32_t ARRAY_END = INT32_END_ADDRESS;

  //FROM HERE IT WILL WRITE ARRAY NAME AND START AND END ADDRESSES IN ARRAY DATA MEMORY SECTION
  ARRAY_DATA_START_ADDRESS = ARRAY_DATA_START_ADDRESS - 16;

  //THE FOR LOOP BELLOW SHIFTS ARRAY DATA 16 BYTES LEFT 
  for(int i = ARRAY_DATA_START_ADDRESS + 16; i <= ARRAY_DATA_END_ADDRESS; i++)
  {
    INTERNAL_RAM[i - 16] = INTERNAL_RAM[i];
  }

  //THE FOR LOOP BELLOW FILLS THE NEW SPACE WITH ZEROS, JUST TO BE SAFE
  for(int i = 0; i < 16; i++)
  {
    INTERNAL_RAM[(ARRAY_DATA_END_ADDRESS - 16) + i] = 0;
  }

  //THE FOR LOOP BELLOW WRITES ARRAY NAME IN FIRST 8 BYTES OF THE NEW SPACE
  for(int i = 0; i < 8; i++)
  {
    INTERNAL_RAM[(ARRAY_DATA_END_ADDRESS - 16) + i] = ARRAY_NAME.charAt(i);
  }

  //THE FOR LOOP BELLOW WRITES ARRAY START ADDRESS IN NEXT 4 BYTES OF THE NEW SPACE
  byte* ptr1 = (byte*)&ARRAY_START;
  for (byte i = 0; i < sizeof(ARRAY_START); i++) 
  {
    INTERNAL_RAM[(ARRAY_DATA_END_ADDRESS - 8) + i] = ptr1[3 - i];
  }

  //THE FOR LOOP BELLOW WRITES ARRAY END ADDRESS IN NEXT 4 BYTES OF THE NEW SPACE
  byte* ptr2 = (byte*)&ARRAY_END;
  for (byte i = 0; i < sizeof(ARRAY_END); i++) 
  {
    INTERNAL_RAM[(ARRAY_DATA_END_ADDRESS - 4) + i] = ptr2[3 - i];
  }

  return 1;
}

int8_t WRITE_INT32_INTERNAL(String VARNAME, int32_t VALUE) {
  VARNAME.trim();

  while(VARNAME.length() < 8)
  {
    VARNAME += "_";
  }

  if (CHECK_VARIABLE_EXISTS(VARNAME, "NULL") != -1) {
    return -1;
  }

  ALLOC_INT32();
  byte* ptr = (byte*)&VALUE;
  for (int i = 0; i < 8; i++) {
    INTERNAL_RAM[(INT32_END_ADDRESS - 11) + i] = VARNAME.charAt(i);
  }

  for (byte i = 0; i < sizeof(VALUE); i++) {
    INTERNAL_RAM[INT32_END_ADDRESS - i] = ptr[i];
  }

  return 1;
}

int8_t STORE_IN_INT32(String VARNAME, int32_t ARRAY_INDEX, int32_t VALUE)
{
  VARNAME.trim();
  int32_t VAR_ADDRESS = CHECK_VARIABLE_EXISTS(VARNAME, "INT32");
  int32_t ARRAY_START = GET_ARRAY_INFO(VARNAME, "INT32", 0);
  int32_t ARRAY_END = GET_ARRAY_INFO(VARNAME, "INT32", 1);
  uint8_t ARRAY_ELEMENT_SIZE;
  uint32_t ARRAY_LENGTH;

  

  if(VAR_ADDRESS < 0) 
  {
    return -3; // NO SUCH ARRAY OR VARIABLE WITH PROVIDED NAME EXISTS
  }

  if(ARRAY_INDEX > 0 && ARRAY_START == -1)
  {
    return -1; // VARIABLE ISN`T ARRAY
  }

  if(ARRAY_INDEX < 0 && ARRAY_START > -1)
  {
    return -1; // ARRAY ISNT VARIABLE LOL
  }

  if(ARRAY_START > -1)
  {

    if(ARRAY_END <= DOUBLE_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 16;
    }else if(ARRAY_END <= INT64_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 16;
    }else if(ARRAY_END <= UINT64_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 16;
    }else if(ARRAY_END <= INT32_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 12;
    }else if(ARRAY_END <= UINT32_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 12;
    }else if(ARRAY_END <= INT16_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 10;
    }else if(ARRAY_END <= UINT16_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 10;
    }else if(ARRAY_END <= INT8_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 9;
    }else if(ARRAY_END <= UINT8_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 9;
    }else if(ARRAY_END <= BOOL_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 9;
    }else if(ARRAY_END <= CHAR_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 9;
    }

    ARRAY_LENGTH = (ARRAY_END - ARRAY_START + 1) / ARRAY_ELEMENT_SIZE;

    if(ARRAY_INDEX > ARRAY_LENGTH - 1)
    {
      return -2; // TRYING TO ACCESS OUT OF ARRAY BOUNDS
    }

    byte* ptr = (byte*)&VALUE;
    for (byte i = 0; i < sizeof(VALUE); i++) 
    {
      INTERNAL_RAM[(ARRAY_START + (ARRAY_INDEX * ARRAY_ELEMENT_SIZE) + 11) - i] = ptr[i];
    }
  }

  if(ARRAY_START < 0)
  {
    byte* ptr = (byte*)&VALUE;
    for (byte i = 0; i < sizeof(VALUE); i++) 
    {
      INTERNAL_RAM[(VAR_ADDRESS + 11) - i] = ptr[i];
    }
  }

  return 1;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------


//----------------------------UINT32 FUNCTIONS------------------------------------------------------------------------------------------------------------------------

int ALLOC_UINT32() {

  for (int j = CHAR_END_ADDRESS; j >= UINT32_END_ADDRESS + 1; j--) {
    INTERNAL_RAM[j + INT32_SIZE] = INTERNAL_RAM[j];
  }

  UINT32_END_ADDRESS = UINT32_END_ADDRESS + INT32_SIZE;
  for (int i = UINT32_END_ADDRESS; i > (UINT32_END_ADDRESS - INT32_SIZE); i--) {
    INTERNAL_RAM[i] = 0;
  }

  CHAR_END_ADDRESS = CHAR_END_ADDRESS + INT32_SIZE;
  CHAR_START_ADDRESS = CHAR_START_ADDRESS + INT32_SIZE;

  BOOL_START_ADDRESS = BOOL_START_ADDRESS + INT32_SIZE;
  BOOL_END_ADDRESS = BOOL_END_ADDRESS + INT32_SIZE;

  UINT8_START_ADDRESS = UINT8_START_ADDRESS + INT32_SIZE;
  UINT8_END_ADDRESS = UINT8_END_ADDRESS + INT32_SIZE;

  INT8_START_ADDRESS = INT8_START_ADDRESS + INT32_SIZE;
  INT8_END_ADDRESS = INT8_END_ADDRESS + INT32_SIZE;

  UINT16_START_ADDRESS = UINT16_START_ADDRESS + INT32_SIZE;
  UINT16_END_ADDRESS = UINT16_END_ADDRESS + INT32_SIZE;

  INT16_START_ADDRESS = INT16_START_ADDRESS + INT32_SIZE;
  INT16_END_ADDRESS = INT16_END_ADDRESS + INT32_SIZE;

  return 1;
}

int8_t ALLOC_UINT32_ARRAY(String ARRAY_NAME, uint16_t ARRAY_LENGTH)  //RETURNS -1 IF OPERATION WAS  UNSUCCESSFULL, RETURNS 1 IF OPERATION WAS SUCCESSFULL.
{
  ARRAY_NAME.trim();

  // JUST CHECKING IF VARIABLES WITH THIS NAME ALREADY EXIST
  if(CHECK_VARIABLE_EXISTS(ARRAY_NAME, "NULL") != -1) 
  {
    return -1;
  }

  //IF ARRAY IS LARGER THAN 2000, OR IF ARRAY_NAME HAS WRONG SIZE, OR IF MEMORY IS FULL, ERROR THIS SH*T 
  if(ARRAY_LENGTH > 2000 || ARRAY_NAME.length() != 8 || (ARRAY_DATA_START_ADDRESS - 1) - (CHAR_END_ADDRESS + 1) < 16)
  {
    return -1;
  }

  //ALLOCATING ARRAY SPACE AND SAVING ITS START AND END ADDRESSES
  uint32_t ARRAY_START = UINT32_END_ADDRESS + 1;

  for(int i = 0; i < ARRAY_LENGTH; i++)
  {
    ALLOC_UINT32();
  }

  uint32_t ARRAY_END = UINT32_END_ADDRESS;

  //FROM HERE IT WILL WRITE ARRAY NAME AND START AND END ADDRESSES IN ARRAY DATA MEMORY SECTION
  ARRAY_DATA_START_ADDRESS = ARRAY_DATA_START_ADDRESS - 16;

  //THE FOR LOOP BELLOW SHIFTS ARRAY DATA 16 BYTES LEFT 
  for(int i = ARRAY_DATA_START_ADDRESS + 16; i <= ARRAY_DATA_END_ADDRESS; i++)
  {
    INTERNAL_RAM[i - 16] = INTERNAL_RAM[i];
  }

  //THE FOR LOOP BELLOW FILLS THE NEW SPACE WITH ZEROS, JUST TO BE SAFE
  for(int i = 0; i < 16; i++)
  {
    INTERNAL_RAM[(ARRAY_DATA_END_ADDRESS - 16) + i] = 0;
  }

  //THE FOR LOOP BELLOW WRITES ARRAY NAME IN FIRST 8 BYTES OF THE NEW SPACE
  for(int i = 0; i < 8; i++)
  {
    INTERNAL_RAM[(ARRAY_DATA_END_ADDRESS - 16) + i] = ARRAY_NAME.charAt(i);
  }

  //THE FOR LOOP BELLOW WRITES ARRAY START ADDRESS IN NEXT 4 BYTES OF THE NEW SPACE
  byte* ptr1 = (byte*)&ARRAY_START;
  for (byte i = 0; i < sizeof(ARRAY_START); i++) 
  {
    INTERNAL_RAM[(ARRAY_DATA_END_ADDRESS - 8) + i] = ptr1[3 - i];
  }

  //THE FOR LOOP BELLOW WRITES ARRAY END ADDRESS IN NEXT 4 BYTES OF THE NEW SPACE
  byte* ptr2 = (byte*)&ARRAY_END;
  for (byte i = 0; i < sizeof(ARRAY_END); i++) 
  {
    INTERNAL_RAM[(ARRAY_DATA_END_ADDRESS - 4) + i] = ptr2[3 - i];
  }

  return 1;
}

int8_t WRITE_UINT32_INTERNAL(String VARNAME, uint32_t VALUE) {
  VARNAME.trim();

  while(VARNAME.length() < 8)
  {
    VARNAME += "_";
  }

  if (CHECK_VARIABLE_EXISTS(VARNAME, "NULL") != -1) {
    return -1;
  }

  ALLOC_UINT32();
  byte* ptr = (byte*)&VALUE;
  for (int i = 0; i < 8; i++) {
    INTERNAL_RAM[(UINT32_END_ADDRESS - 11) + i] = VARNAME.charAt(i);
  }

  for (byte i = 0; i < sizeof(VALUE); i++) {
    INTERNAL_RAM[UINT32_END_ADDRESS - i] = ptr[i];
  }

  return 1;
}

int8_t STORE_IN_UINT32(String VARNAME, int32_t ARRAY_INDEX, uint32_t VALUE)
{
  VARNAME.trim();
  int32_t VAR_ADDRESS = CHECK_VARIABLE_EXISTS(VARNAME, "UINT32");
  int32_t ARRAY_START = GET_ARRAY_INFO(VARNAME, "UINT32", 0);
  int32_t ARRAY_END = GET_ARRAY_INFO(VARNAME, "UINT32", 1);
  uint8_t ARRAY_ELEMENT_SIZE;
  uint32_t ARRAY_LENGTH;

  

  if(VAR_ADDRESS < 0) 
  {
    return -3; // NO SUCH ARRAY OR VARIABLE WITH PROVIDED NAME EXISTS
  }

  if(ARRAY_INDEX > 0 && ARRAY_START == -1)
  {
    return -1; // VARIABLE ISN`T ARRAY
  }

  if(ARRAY_INDEX < 0 && ARRAY_START > -1)
  {
    return -1; // ARRAY ISNT VARIABLE LOL
  }

  if(ARRAY_START > -1)
  {

    if(ARRAY_END <= DOUBLE_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 16;
    }else if(ARRAY_END <= INT64_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 16;
    }else if(ARRAY_END <= UINT64_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 16;
    }else if(ARRAY_END <= INT32_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 12;
    }else if(ARRAY_END <= UINT32_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 12;
    }else if(ARRAY_END <= INT16_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 10;
    }else if(ARRAY_END <= UINT16_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 10;
    }else if(ARRAY_END <= INT8_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 9;
    }else if(ARRAY_END <= UINT8_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 9;
    }else if(ARRAY_END <= BOOL_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 9;
    }else if(ARRAY_END <= CHAR_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 9;
    }

    ARRAY_LENGTH = (ARRAY_END - ARRAY_START + 1) / ARRAY_ELEMENT_SIZE;

    if(ARRAY_INDEX > ARRAY_LENGTH - 1)
    {
      return -2; // TRYING TO ACCESS OUT OF ARRAY BOUNDS
    }

    byte* ptr = (byte*)&VALUE;
    for (byte i = 0; i < sizeof(VALUE); i++) 
    {
      INTERNAL_RAM[(ARRAY_START + (ARRAY_INDEX * ARRAY_ELEMENT_SIZE) + 11) - i] = ptr[i];
    }
  }

  if(ARRAY_START < 0)
  {
    byte* ptr = (byte*)&VALUE;
    for (byte i = 0; i < sizeof(VALUE); i++) 
    {
      INTERNAL_RAM[(VAR_ADDRESS + 11) - i] = ptr[i];
    }
  }

  return 1;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------


//----------------------------INT16 FUNCTIONS------------------------------------------------------------------------------------------------------------------------

int ALLOC_INT16() {

  for (int j = CHAR_END_ADDRESS; j >= INT16_END_ADDRESS + 1; j--) {
    INTERNAL_RAM[j + INT16_SIZE] = INTERNAL_RAM[j];
  }

  INT16_END_ADDRESS = INT16_END_ADDRESS + INT16_SIZE;
  for (int i = INT16_END_ADDRESS; i > (INT16_END_ADDRESS - INT16_SIZE); i--) {
    INTERNAL_RAM[i] = 0;
  }

  CHAR_END_ADDRESS = CHAR_END_ADDRESS + 10;
  CHAR_START_ADDRESS = CHAR_START_ADDRESS + 10;

  BOOL_START_ADDRESS = BOOL_START_ADDRESS + 10;
  BOOL_END_ADDRESS = BOOL_END_ADDRESS + 10;

  UINT8_START_ADDRESS = UINT8_START_ADDRESS + 10;
  UINT8_END_ADDRESS = UINT8_END_ADDRESS + 10;

  INT8_START_ADDRESS = INT8_START_ADDRESS + 10;
  INT8_END_ADDRESS = INT8_END_ADDRESS + 10;

  UINT16_START_ADDRESS = UINT16_START_ADDRESS + 10;
  UINT16_END_ADDRESS = UINT16_END_ADDRESS + 10;

  return 1;
}

int8_t ALLOC_INT16_ARRAY(String ARRAY_NAME, uint16_t ARRAY_LENGTH)  //RETURNS -1 IF OPERATION WAS  UNSUCCESSFULL, RETURNS 1 IF OPERATION WAS SUCCESSFULL.
{
  ARRAY_NAME.trim();

  // JUST CHECKING IF VARIABLES WITH THIS NAME ALREADY EXIST
  if(CHECK_VARIABLE_EXISTS(ARRAY_NAME, "NULL") != -1) 
  {
    return -1;
  }

  //IF ARRAY IS LARGER THAN 2000, OR IF ARRAY_NAME HAS WRONG SIZE, OR IF MEMORY IS FULL, ERROR THIS SH*T 
  if(ARRAY_LENGTH > 2000 || ARRAY_NAME.length() != 8 || (ARRAY_DATA_START_ADDRESS - 1) - (CHAR_END_ADDRESS + 1) < 16)
  {
    return -1;
  }

  //ALLOCATING ARRAY SPACE AND SAVING ITS START AND END ADDRESSES
  uint32_t ARRAY_START = INT16_END_ADDRESS + 1;

  for(int i = 0; i < ARRAY_LENGTH; i++)
  {
    ALLOC_INT16();
  }

  uint32_t ARRAY_END = INT16_END_ADDRESS;

  //FROM HERE IT WILL WRITE ARRAY NAME AND START AND END ADDRESSES IN ARRAY DATA MEMORY SECTION
  ARRAY_DATA_START_ADDRESS = ARRAY_DATA_START_ADDRESS - 16;

  //THE FOR LOOP BELLOW SHIFTS ARRAY DATA 16 BYTES LEFT 
  for(int i = ARRAY_DATA_START_ADDRESS + 16; i <= ARRAY_DATA_END_ADDRESS; i++)
  {
    INTERNAL_RAM[i - 16] = INTERNAL_RAM[i];
  }

  //THE FOR LOOP BELLOW FILLS THE NEW SPACE WITH ZEROS, JUST TO BE SAFE
  for(int i = 0; i < 16; i++)
  {
    INTERNAL_RAM[(ARRAY_DATA_END_ADDRESS - 16) + i] = 0;
  }

  //THE FOR LOOP BELLOW WRITES ARRAY NAME IN FIRST 8 BYTES OF THE NEW SPACE
  for(int i = 0; i < 8; i++)
  {
    INTERNAL_RAM[(ARRAY_DATA_END_ADDRESS - 16) + i] = ARRAY_NAME.charAt(i);
  }

  //THE FOR LOOP BELLOW WRITES ARRAY START ADDRESS IN NEXT 4 BYTES OF THE NEW SPACE
  byte* ptr1 = (byte*)&ARRAY_START;
  for (byte i = 0; i < sizeof(ARRAY_START); i++) 
  {
    INTERNAL_RAM[(ARRAY_DATA_END_ADDRESS - 8) + i] = ptr1[3 - i];
  }

  //THE FOR LOOP BELLOW WRITES ARRAY END ADDRESS IN NEXT 4 BYTES OF THE NEW SPACE
  byte* ptr2 = (byte*)&ARRAY_END;
  for (byte i = 0; i < sizeof(ARRAY_END); i++) 
  {
    INTERNAL_RAM[(ARRAY_DATA_END_ADDRESS - 4) + i] = ptr2[3 - i];
  }

  return 1;
}

int8_t WRITE_INT16_INTERNAL(String VARNAME, int16_t VALUE) {
  VARNAME.trim();

  while(VARNAME.length() < 8)
  {
    VARNAME += "_";
  }

  if (CHECK_VARIABLE_EXISTS(VARNAME, "NULL") != -1) {
    return -1;
  }

  ALLOC_INT16();
  byte* ptr = (byte*)&VALUE;
  for (int i = 0; i < 8; i++) {
    INTERNAL_RAM[(INT16_END_ADDRESS - 9) + i] = VARNAME.charAt(i);
  }

  for (byte i = 0; i < sizeof(VALUE); i++) {
    INTERNAL_RAM[INT16_END_ADDRESS - i] = ptr[i];
  }

  return 1;
}

int8_t STORE_IN_INT16(String VARNAME, int32_t ARRAY_INDEX, int16_t VALUE)
{
  VARNAME.trim();
  int32_t VAR_ADDRESS = CHECK_VARIABLE_EXISTS(VARNAME, "INT16");
  int32_t ARRAY_START = GET_ARRAY_INFO(VARNAME, "INT16", 0);
  int32_t ARRAY_END = GET_ARRAY_INFO(VARNAME, "INT16", 1);
  uint8_t ARRAY_ELEMENT_SIZE;
  uint32_t ARRAY_LENGTH;

  

  if(VAR_ADDRESS < 0) 
  {
    return -3; // NO SUCH ARRAY OR VARIABLE WITH PROVIDED NAME EXISTS
  }

  if(ARRAY_INDEX > 0 && ARRAY_START == -1)
  {
    return -1; // VARIABLE ISN`T ARRAY
  }

  if(ARRAY_INDEX < 0 && ARRAY_START > -1)
  {
    return -1; // ARRAY ISNT VARIABLE LOL
  }

  if(ARRAY_START > -1)
  {

    if(ARRAY_END <= DOUBLE_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 16;
    }else if(ARRAY_END <= INT64_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 16;
    }else if(ARRAY_END <= UINT64_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 16;
    }else if(ARRAY_END <= INT32_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 12;
    }else if(ARRAY_END <= UINT32_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 12;
    }else if(ARRAY_END <= INT16_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 10;
    }else if(ARRAY_END <= UINT16_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 10;
    }else if(ARRAY_END <= INT8_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 9;
    }else if(ARRAY_END <= UINT8_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 9;
    }else if(ARRAY_END <= BOOL_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 9;
    }else if(ARRAY_END <= CHAR_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 9;
    }

    ARRAY_LENGTH = (ARRAY_END - ARRAY_START + 1) / ARRAY_ELEMENT_SIZE;

    if(ARRAY_INDEX > ARRAY_LENGTH - 1)
    {
      return -2; // TRYING TO ACCESS OUT OF ARRAY BOUNDS
    }

    byte* ptr = (byte*)&VALUE;
    for (byte i = 0; i < sizeof(VALUE); i++) 
    {
      INTERNAL_RAM[(ARRAY_START + (ARRAY_INDEX * ARRAY_ELEMENT_SIZE) + 9) - i] = ptr[i];
    }
  }

  if(ARRAY_START < 0)
  {
    byte* ptr = (byte*)&VALUE;
    for (byte i = 0; i < sizeof(VALUE); i++) 
    {
      INTERNAL_RAM[(VAR_ADDRESS + 9) - i] = ptr[i];
    }
  }

  return 1;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------


//----------------------------UINT16 FUNCTIONS------------------------------------------------------------------------------------------------------------------------

int ALLOC_UINT16() {

  for (int j = CHAR_END_ADDRESS; j >= UINT16_END_ADDRESS + 1; j--) {
    INTERNAL_RAM[j + INT16_SIZE] = INTERNAL_RAM[j];
  }

  UINT16_END_ADDRESS = UINT16_END_ADDRESS + INT16_SIZE;
  for (int i = UINT16_END_ADDRESS; i > (UINT16_END_ADDRESS - INT16_SIZE); i--) {
    INTERNAL_RAM[i] = 0;
  }

  CHAR_END_ADDRESS = CHAR_END_ADDRESS + 10;
  CHAR_START_ADDRESS = CHAR_START_ADDRESS + 10;

  BOOL_START_ADDRESS = BOOL_START_ADDRESS + 10;
  BOOL_END_ADDRESS = BOOL_END_ADDRESS + 10;

  UINT8_START_ADDRESS = UINT8_START_ADDRESS + 10;
  UINT8_END_ADDRESS = UINT8_END_ADDRESS + 10;

  INT8_START_ADDRESS = INT8_START_ADDRESS + 10;
  INT8_END_ADDRESS = INT8_END_ADDRESS + 10;

  return 1;
}

int8_t ALLOC_UINT16_ARRAY(String ARRAY_NAME, uint16_t ARRAY_LENGTH)  //RETURNS -1 IF OPERATION WAS  UNSUCCESSFULL, RETURNS 1 IF OPERATION WAS SUCCESSFULL.
{
  ARRAY_NAME.trim();

  // JUST CHECKING IF VARIABLES WITH THIS NAME ALREADY EXIST
  if(CHECK_VARIABLE_EXISTS(ARRAY_NAME, "NULL") != -1) 
  {
    return -1;
  }

  //IF ARRAY IS LARGER THAN 2000, OR IF ARRAY_NAME HAS WRONG SIZE, OR IF MEMORY IS FULL, ERROR THIS SH*T 
  if(ARRAY_LENGTH > 2000 || ARRAY_NAME.length() != 8 || (ARRAY_DATA_START_ADDRESS - 1) - (CHAR_END_ADDRESS + 1) < 16)
  {
    return -1;
  }

  //ALLOCATING ARRAY SPACE AND SAVING ITS START AND END ADDRESSES
  uint32_t ARRAY_START = UINT16_END_ADDRESS + 1;

  for(int i = 0; i < ARRAY_LENGTH; i++)
  {
    ALLOC_UINT16();
  }

  uint32_t ARRAY_END = UINT16_END_ADDRESS;

  //FROM HERE IT WILL WRITE ARRAY NAME AND START AND END ADDRESSES IN ARRAY DATA MEMORY SECTION
  ARRAY_DATA_START_ADDRESS = ARRAY_DATA_START_ADDRESS - 16;

  //THE FOR LOOP BELLOW SHIFTS ARRAY DATA 16 BYTES LEFT 
  for(int i = ARRAY_DATA_START_ADDRESS + 16; i <= ARRAY_DATA_END_ADDRESS; i++)
  {
    INTERNAL_RAM[i - 16] = INTERNAL_RAM[i];
  }

  //THE FOR LOOP BELLOW FILLS THE NEW SPACE WITH ZEROS, JUST TO BE SAFE
  for(int i = 0; i < 16; i++)
  {
    INTERNAL_RAM[(ARRAY_DATA_END_ADDRESS - 16) + i] = 0;
  }

  //THE FOR LOOP BELLOW WRITES ARRAY NAME IN FIRST 8 BYTES OF THE NEW SPACE
  for(int i = 0; i < 8; i++)
  {
    INTERNAL_RAM[(ARRAY_DATA_END_ADDRESS - 16) + i] = ARRAY_NAME.charAt(i);
  }

  //THE FOR LOOP BELLOW WRITES ARRAY START ADDRESS IN NEXT 4 BYTES OF THE NEW SPACE
  byte* ptr1 = (byte*)&ARRAY_START;
  for (byte i = 0; i < sizeof(ARRAY_START); i++) 
  {
    INTERNAL_RAM[(ARRAY_DATA_END_ADDRESS - 8) + i] = ptr1[3 - i];
  }

  //THE FOR LOOP BELLOW WRITES ARRAY END ADDRESS IN NEXT 4 BYTES OF THE NEW SPACE
  byte* ptr2 = (byte*)&ARRAY_END;
  for (byte i = 0; i < sizeof(ARRAY_END); i++) 
  {
    INTERNAL_RAM[(ARRAY_DATA_END_ADDRESS - 4) + i] = ptr2[3 - i];
  }

  return 1;
}

int8_t WRITE_UINT16_INTERNAL(String VARNAME, uint16_t VALUE) {
  VARNAME.trim();

  while(VARNAME.length() < 8)
  {
    VARNAME += "_";
  }

  if (CHECK_VARIABLE_EXISTS(VARNAME, "NULL") != -1) {
    return -1;
  }

  ALLOC_UINT16();
  byte* ptr = (byte*)&VALUE;
  for (int i = 0; i < 8; i++) {
    INTERNAL_RAM[(UINT16_END_ADDRESS - 9) + i] = VARNAME.charAt(i);
  }

  for (byte i = 0; i < sizeof(VALUE); i++) {
    INTERNAL_RAM[UINT16_END_ADDRESS - i] = ptr[i];
  }

  return 1;
}

int8_t STORE_IN_UINT16(String VARNAME, int32_t ARRAY_INDEX, uint16_t VALUE)
{
  VARNAME.trim();
  int32_t VAR_ADDRESS = CHECK_VARIABLE_EXISTS(VARNAME, "UINT16");
  int32_t ARRAY_START = GET_ARRAY_INFO(VARNAME, "UINT16", 0);
  int32_t ARRAY_END = GET_ARRAY_INFO(VARNAME, "UINT16", 1);
  uint8_t ARRAY_ELEMENT_SIZE;
  uint32_t ARRAY_LENGTH;

  

  if(VAR_ADDRESS < 0) 
  {
    return -3; // NO SUCH ARRAY OR VARIABLE WITH PROVIDED NAME EXISTS
  }

  if(ARRAY_INDEX > 0 && ARRAY_START == -1)
  {
    return -1; // VARIABLE ISN`T ARRAY
  }

  if(ARRAY_INDEX < 0 && ARRAY_START > -1)
  {
    return -1; // ARRAY ISNT VARIABLE LOL
  }

  if(ARRAY_START > -1)
  {

    if(ARRAY_END <= DOUBLE_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 16;
    }else if(ARRAY_END <= INT64_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 16;
    }else if(ARRAY_END <= UINT64_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 16;
    }else if(ARRAY_END <= INT32_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 12;
    }else if(ARRAY_END <= UINT32_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 12;
    }else if(ARRAY_END <= INT16_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 10;
    }else if(ARRAY_END <= UINT16_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 10;
    }else if(ARRAY_END <= INT8_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 9;
    }else if(ARRAY_END <= UINT8_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 9;
    }else if(ARRAY_END <= BOOL_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 9;
    }else if(ARRAY_END <= CHAR_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 9;
    }

    ARRAY_LENGTH = (ARRAY_END - ARRAY_START + 1) / ARRAY_ELEMENT_SIZE;

    if(ARRAY_INDEX > ARRAY_LENGTH - 1)
    {
      return -2; // TRYING TO ACCESS OUT OF ARRAY BOUNDS
    }

    byte* ptr = (byte*)&VALUE;
    for (byte i = 0; i < sizeof(VALUE); i++) 
    {
      INTERNAL_RAM[(ARRAY_START + (ARRAY_INDEX * ARRAY_ELEMENT_SIZE) + 9) - i] = ptr[i];
    }
  }

  if(ARRAY_START < 0)
  {
    byte* ptr = (byte*)&VALUE;
    for (byte i = 0; i < sizeof(VALUE); i++) 
    {
      INTERNAL_RAM[(VAR_ADDRESS + 9) - i] = ptr[i];
    }
  }

  return 1;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------


//----------------------------INT8 FUNCTIONS------------------------------------------------------------------------------------------------------------------------

int ALLOC_INT8() {

  for (int j = CHAR_END_ADDRESS; j >= INT8_END_ADDRESS + 1; j--) {
    INTERNAL_RAM[j + INT8_SIZE] = INTERNAL_RAM[j];
  }

  INT8_END_ADDRESS = INT8_END_ADDRESS + INT8_SIZE;
  for (int i = INT8_END_ADDRESS; i > (INT8_END_ADDRESS - INT8_SIZE); i--) {
    INTERNAL_RAM[i] = 0;
  }

  CHAR_END_ADDRESS = CHAR_END_ADDRESS + INT8_SIZE;
  CHAR_START_ADDRESS = CHAR_START_ADDRESS + INT8_SIZE;

  BOOL_START_ADDRESS = BOOL_START_ADDRESS + INT8_SIZE;
  BOOL_END_ADDRESS = BOOL_END_ADDRESS + INT8_SIZE;

  UINT8_START_ADDRESS = UINT8_START_ADDRESS + INT8_SIZE;
  UINT8_END_ADDRESS = UINT8_END_ADDRESS + INT8_SIZE;

  return 1;
}

int8_t ALLOC_INT8_ARRAY(String ARRAY_NAME, uint16_t ARRAY_LENGTH)  //RETURNS -1 IF OPERATION WAS  UNSUCCESSFULL, RETURNS 1 IF OPERATION WAS SUCCESSFULL.
{
  ARRAY_NAME.trim();

  // JUST CHECKING IF VARIABLES WITH THIS NAME ALREADY EXIST
  if(CHECK_VARIABLE_EXISTS(ARRAY_NAME, "NULL") != -1) 
  {
    return -1;
  }

  //IF ARRAY IS LARGER THAN 2000, OR IF ARRAY_NAME HAS WRONG SIZE, OR IF MEMORY IS FULL, ERROR THIS SH*T 
  if(ARRAY_LENGTH > 2000 || ARRAY_NAME.length() != 8 || (ARRAY_DATA_START_ADDRESS - 1) - (CHAR_END_ADDRESS + 1) < 16)
  {
    return -1;
  }

  //ALLOCATING ARRAY SPACE AND SAVING ITS START AND END ADDRESSES
  uint32_t ARRAY_START = INT8_END_ADDRESS + 1;

  for(int i = 0; i < ARRAY_LENGTH; i++)
  {
    ALLOC_INT8();
  }

  uint32_t ARRAY_END = INT8_END_ADDRESS;

  //FROM HERE IT WILL WRITE ARRAY NAME AND START AND END ADDRESSES IN ARRAY DATA MEMORY SECTION
  ARRAY_DATA_START_ADDRESS = ARRAY_DATA_START_ADDRESS - 16;

  //THE FOR LOOP BELLOW SHIFTS ARRAY DATA 16 BYTES LEFT 
  for(int i = ARRAY_DATA_START_ADDRESS + 16; i <= ARRAY_DATA_END_ADDRESS; i++)
  {
    INTERNAL_RAM[i - 16] = INTERNAL_RAM[i];
  }

  //THE FOR LOOP BELLOW FILLS THE NEW SPACE WITH ZEROS, JUST TO BE SAFE
  for(int i = 0; i < 16; i++)
  {
    INTERNAL_RAM[(ARRAY_DATA_END_ADDRESS - 16) + i] = 0;
  }

  //THE FOR LOOP BELLOW WRITES ARRAY NAME IN FIRST 8 BYTES OF THE NEW SPACE
  for(int i = 0; i < 8; i++)
  {
    INTERNAL_RAM[(ARRAY_DATA_END_ADDRESS - 16) + i] = ARRAY_NAME.charAt(i);
  }

  //THE FOR LOOP BELLOW WRITES ARRAY START ADDRESS IN NEXT 4 BYTES OF THE NEW SPACE
  byte* ptr1 = (byte*)&ARRAY_START;
  for (byte i = 0; i < sizeof(ARRAY_START); i++) 
  {
    INTERNAL_RAM[(ARRAY_DATA_END_ADDRESS - 8) + i] = ptr1[3 - i];
  }

  //THE FOR LOOP BELLOW WRITES ARRAY END ADDRESS IN NEXT 4 BYTES OF THE NEW SPACE
  byte* ptr2 = (byte*)&ARRAY_END;
  for (byte i = 0; i < sizeof(ARRAY_END); i++) 
  {
    INTERNAL_RAM[(ARRAY_DATA_END_ADDRESS - 4) + i] = ptr2[3 - i];
  }

  return 1;
}

int8_t WRITE_INT8_INTERNAL(String VARNAME, int8_t VALUE) {
  VARNAME.trim();

  while(VARNAME.length() < 8)
  {
    VARNAME += "_";
  }

  if (CHECK_VARIABLE_EXISTS(VARNAME, "NULL") != -1) {
    return -1;
  }

  ALLOC_INT8();
  byte* ptr = (byte*)&VALUE;
  for (int i = 0; i < 8; i++) {
    INTERNAL_RAM[(INT8_END_ADDRESS - 8) + i] = VARNAME.charAt(i);
  }

  for (byte i = 0; i < sizeof(VALUE); i++) {
    INTERNAL_RAM[INT8_END_ADDRESS - i] = ptr[i];
  }

  return 1;
}

int8_t STORE_IN_INT8(String VARNAME, int32_t ARRAY_INDEX, int8_t VALUE)
{
  VARNAME.trim();
  int32_t VAR_ADDRESS = CHECK_VARIABLE_EXISTS(VARNAME, "INT8");
  int32_t ARRAY_START = GET_ARRAY_INFO(VARNAME, "INT8", 0);
  int32_t ARRAY_END = GET_ARRAY_INFO(VARNAME, "INT8", 1);
  uint8_t ARRAY_ELEMENT_SIZE;
  uint32_t ARRAY_LENGTH;

  

  if(VAR_ADDRESS < 0) 
  {
    return -3; // NO SUCH ARRAY OR VARIABLE WITH PROVIDED NAME EXISTS
  }

  if(ARRAY_INDEX > 0 && ARRAY_START == -1)
  {
    return -1; // VARIABLE ISN`T ARRAY
  }

  if(ARRAY_INDEX < 0 && ARRAY_START > -1)
  {
    return -1; // ARRAY ISNT VARIABLE LOL
  }

  if(ARRAY_START > -1)
  {

    if(ARRAY_END <= DOUBLE_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 16;
    }else if(ARRAY_END <= INT64_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 16;
    }else if(ARRAY_END <= UINT64_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 16;
    }else if(ARRAY_END <= INT32_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 12;
    }else if(ARRAY_END <= UINT32_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 12;
    }else if(ARRAY_END <= INT16_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 10;
    }else if(ARRAY_END <= UINT16_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 10;
    }else if(ARRAY_END <= INT8_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 9;
    }else if(ARRAY_END <= UINT8_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 9;
    }else if(ARRAY_END <= BOOL_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 9;
    }else if(ARRAY_END <= CHAR_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 9;
    }

    ARRAY_LENGTH = (ARRAY_END - ARRAY_START + 1) / ARRAY_ELEMENT_SIZE;

    if(ARRAY_INDEX > ARRAY_LENGTH - 1)
    {
      return -2; // TRYING TO ACCESS OUT OF ARRAY BOUNDS
    }

    byte* ptr = (byte*)&VALUE;
    for (byte i = 0; i < sizeof(VALUE); i++) 
    {
      INTERNAL_RAM[(ARRAY_START + (ARRAY_INDEX * ARRAY_ELEMENT_SIZE) + 8) - i] = ptr[i];
    }
  }

  if(ARRAY_START < 0)
  {
    byte* ptr = (byte*)&VALUE;
    for (byte i = 0; i < sizeof(VALUE); i++) 
    {
      INTERNAL_RAM[(VAR_ADDRESS + 8) - i] = ptr[i];
    }
  }

  return 1;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------


//----------------------------UINT8 FUNCTIONS------------------------------------------------------------------------------------------------------------------------

int ALLOC_UINT8() {

  for (int j = CHAR_END_ADDRESS; j >= UINT8_END_ADDRESS + 1; j--) {
    INTERNAL_RAM[j + INT8_SIZE] = INTERNAL_RAM[j];
  }

  UINT8_END_ADDRESS = UINT8_END_ADDRESS + INT8_SIZE;
  for (int i = UINT8_END_ADDRESS; i > (UINT8_END_ADDRESS - INT8_SIZE); i--) {
    INTERNAL_RAM[i] = 0;
  }

  CHAR_END_ADDRESS = CHAR_END_ADDRESS + INT8_SIZE;
  CHAR_START_ADDRESS = CHAR_START_ADDRESS + INT8_SIZE;

  BOOL_START_ADDRESS = BOOL_START_ADDRESS + INT8_SIZE;
  BOOL_END_ADDRESS = BOOL_END_ADDRESS + INT8_SIZE;

  return 1;
}

int8_t ALLOC_UINT8_ARRAY(String ARRAY_NAME, uint16_t ARRAY_LENGTH)  //RETURNS -1 IF OPERATION WAS  UNSUCCESSFULL, RETURNS 1 IF OPERATION WAS SUCCESSFULL.
{
  ARRAY_NAME.trim();

  // JUST CHECKING IF VARIABLES WITH THIS NAME ALREADY EXIST
  if(CHECK_VARIABLE_EXISTS(ARRAY_NAME, "NULL") != -1) 
  {
    return -1;
  }

  //IF ARRAY IS LARGER THAN 2000, OR IF ARRAY_NAME HAS WRONG SIZE, OR IF MEMORY IS FULL, ERROR THIS SH*T 
  if(ARRAY_LENGTH > 2000 || ARRAY_NAME.length() != 8 || (ARRAY_DATA_START_ADDRESS - 1) - (CHAR_END_ADDRESS + 1) < 16)
  {
    return -1;
  }

  //ALLOCATING ARRAY SPACE AND SAVING ITS START AND END ADDRESSES
  uint32_t ARRAY_START = UINT8_END_ADDRESS + 1;

  for(int i = 0; i < ARRAY_LENGTH; i++)
  {
    ALLOC_UINT8();
  }

  uint32_t ARRAY_END = UINT8_END_ADDRESS;

  //FROM HERE IT WILL WRITE ARRAY NAME AND START AND END ADDRESSES IN ARRAY DATA MEMORY SECTION
  ARRAY_DATA_START_ADDRESS = ARRAY_DATA_START_ADDRESS - 16;

  //THE FOR LOOP BELLOW SHIFTS ARRAY DATA 16 BYTES LEFT 
  for(int i = ARRAY_DATA_START_ADDRESS + 16; i <= ARRAY_DATA_END_ADDRESS; i++)
  {
    INTERNAL_RAM[i - 16] = INTERNAL_RAM[i];
  }

  //THE FOR LOOP BELLOW FILLS THE NEW SPACE WITH ZEROS, JUST TO BE SAFE
  for(int i = 0; i < 16; i++)
  {
    INTERNAL_RAM[(ARRAY_DATA_END_ADDRESS - 16) + i] = 0;
  }

  //THE FOR LOOP BELLOW WRITES ARRAY NAME IN FIRST 8 BYTES OF THE NEW SPACE
  for(int i = 0; i < 8; i++)
  {
    INTERNAL_RAM[(ARRAY_DATA_END_ADDRESS - 16) + i] = ARRAY_NAME.charAt(i);
  }

  //THE FOR LOOP BELLOW WRITES ARRAY START ADDRESS IN NEXT 4 BYTES OF THE NEW SPACE
  byte* ptr1 = (byte*)&ARRAY_START;
  for (byte i = 0; i < sizeof(ARRAY_START); i++) 
  {
    INTERNAL_RAM[(ARRAY_DATA_END_ADDRESS - 8) + i] = ptr1[3 - i];
  }

  //THE FOR LOOP BELLOW WRITES ARRAY END ADDRESS IN NEXT 4 BYTES OF THE NEW SPACE
  byte* ptr2 = (byte*)&ARRAY_END;
  for (byte i = 0; i < sizeof(ARRAY_END); i++) 
  {
    INTERNAL_RAM[(ARRAY_DATA_END_ADDRESS - 4) + i] = ptr2[3 - i];
  }

  return 1;
}

int8_t WRITE_UINT8_INTERNAL(String VARNAME, uint8_t VALUE) {
  VARNAME.trim();

  while(VARNAME.length() < 8)
  {
    VARNAME += "_";
  }

  if (CHECK_VARIABLE_EXISTS(VARNAME, "NULL") != -1) {
    return -1;
  }

  ALLOC_UINT8();
  byte* ptr = (byte*)&VALUE;
  for (int i = 0; i < 8; i++) {
    INTERNAL_RAM[(UINT8_END_ADDRESS - 8) + i] = VARNAME.charAt(i);
  }

  for (byte i = 0; i < sizeof(VALUE); i++) {
    INTERNAL_RAM[UINT8_END_ADDRESS - i] = ptr[i];
  }

  return 1;
}

int8_t STORE_IN_UINT8(String VARNAME, int32_t ARRAY_INDEX, uint8_t VALUE)
{
  VARNAME.trim();

  while(VARNAME.length() < 8)
  {
    VARNAME += "_";
  }
  
  int32_t VAR_ADDRESS = CHECK_VARIABLE_EXISTS(VARNAME, "UINT8");
  int32_t ARRAY_START = GET_ARRAY_INFO(VARNAME, "UINT8", 0);
  int32_t ARRAY_END = GET_ARRAY_INFO(VARNAME, "UINT8", 1);
  uint8_t ARRAY_ELEMENT_SIZE;
  uint32_t ARRAY_LENGTH;

  if(VAR_ADDRESS < 0) 
  {
    return -3; // NO SUCH ARRAY OR VARIABLE WITH PROVIDED NAME EXISTS
  }

  if(ARRAY_INDEX > 0 && ARRAY_START == -1)
  {
    return -1; // VARIABLE ISN`T ARRAY
  }

  if(ARRAY_INDEX < 0 && ARRAY_START > -1)
  {
    return -1; // ARRAY ISNT VARIABLE LOL
  }

  if(ARRAY_START > -1)
  {

    if(ARRAY_END <= DOUBLE_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 16;
    }else if(ARRAY_END <= INT64_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 16;
    }else if(ARRAY_END <= UINT64_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 16;
    }else if(ARRAY_END <= INT32_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 12;
    }else if(ARRAY_END <= UINT32_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 12;
    }else if(ARRAY_END <= INT16_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 10;
    }else if(ARRAY_END <= UINT16_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 10;
    }else if(ARRAY_END <= INT8_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 9;
    }else if(ARRAY_END <= UINT8_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 9;
    }else if(ARRAY_END <= BOOL_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 9;
    }else if(ARRAY_END <= CHAR_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 9;
    }

    ARRAY_LENGTH = (ARRAY_END - ARRAY_START + 1) / ARRAY_ELEMENT_SIZE;

    if(ARRAY_INDEX > ARRAY_LENGTH - 1)
    {
      return -2; // TRYING TO ACCESS OUT OF ARRAY BOUNDS
    }

    byte* ptr = (byte*)&VALUE;
    for (byte i = 0; i < sizeof(VALUE); i++) 
    {
      INTERNAL_RAM[(ARRAY_START + (ARRAY_INDEX * ARRAY_ELEMENT_SIZE) + 8) - i] = ptr[i];
    }
  }

  if(ARRAY_START < 0)
  {
    byte* ptr = (byte*)&VALUE;
    for (byte i = 0; i < sizeof(VALUE); i++) 
    {
      INTERNAL_RAM[(VAR_ADDRESS + 8) - i] = ptr[i];
    }
  }

  return 1;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------


//----------------------------BOOL FUNCTIONS------------------------------------------------------------------------------------------------------------------------

int ALLOC_BOOL() {

  for (int j = CHAR_END_ADDRESS; j >= BOOL_END_ADDRESS + 1; j--) 
  {
    INTERNAL_RAM[j + BOOL_SIZE] = INTERNAL_RAM[j];
  }

  BOOL_END_ADDRESS = BOOL_END_ADDRESS + BOOL_SIZE;
  for (int i = BOOL_END_ADDRESS; i > (BOOL_END_ADDRESS - BOOL_SIZE); i--) 
  {
    INTERNAL_RAM[i] = 0;
  }

  CHAR_END_ADDRESS = CHAR_END_ADDRESS + BOOL_SIZE;
  CHAR_START_ADDRESS = CHAR_START_ADDRESS + BOOL_SIZE;

  return 1;
}

int8_t ALLOC_BOOL_ARRAY(String ARRAY_NAME, uint16_t ARRAY_LENGTH)  //RETURNS -1 IF OPERATION WAS  UNSUCCESSFULL, RETURNS 1 IF OPERATION WAS SUCCESSFULL.
{
  ARRAY_NAME.trim();

  // JUST CHECKING IF VARIABLES WITH THIS NAME ALREADY EXIST
  if(CHECK_VARIABLE_EXISTS(ARRAY_NAME, "NULL") != -1) 
  {
    return -1;
  }

  //IF ARRAY IS LARGER THAN 2000, OR IF ARRAY_NAME HAS WRONG SIZE, OR IF MEMORY IS FULL, ERROR THIS SH*T 
  if(ARRAY_LENGTH > 2000 || ARRAY_NAME.length() != 8 || (ARRAY_DATA_START_ADDRESS - 1) - (CHAR_END_ADDRESS + 1) < 16)
  {
    return -1;
  }

  //ALLOCATING ARRAY SPACE AND SAVING ITS START AND END ADDRESSES
  uint32_t ARRAY_START = BOOL_END_ADDRESS + 1;

  for(int i = 0; i < ARRAY_LENGTH; i++)
  {
    ALLOC_BOOL();
  }

  uint32_t ARRAY_END = BOOL_END_ADDRESS;

  //FROM HERE IT WILL WRITE ARRAY NAME AND START AND END ADDRESSES IN ARRAY DATA MEMORY SECTION
  ARRAY_DATA_START_ADDRESS = ARRAY_DATA_START_ADDRESS - 16;

  //THE FOR LOOP BELLOW SHIFTS ARRAY DATA 16 BYTES LEFT 
  for(int i = ARRAY_DATA_START_ADDRESS + 16; i <= ARRAY_DATA_END_ADDRESS; i++)
  {
    INTERNAL_RAM[i - 16] = INTERNAL_RAM[i];
  }

  //THE FOR LOOP BELLOW FILLS THE NEW SPACE WITH ZEROS, JUST TO BE SAFE
  for(int i = 0; i < 16; i++)
  {
    INTERNAL_RAM[(ARRAY_DATA_END_ADDRESS - 16) + i] = 0;
  }

  //THE FOR LOOP BELLOW WRITES ARRAY NAME IN FIRST 8 BYTES OF THE NEW SPACE
  for(int i = 0; i < 8; i++)
  {
    INTERNAL_RAM[(ARRAY_DATA_END_ADDRESS - 16) + i] = ARRAY_NAME.charAt(i);
  }

  //THE FOR LOOP BELLOW WRITES ARRAY START ADDRESS IN NEXT 4 BYTES OF THE NEW SPACE
  byte* ptr1 = (byte*)&ARRAY_START;
  for (byte i = 0; i < sizeof(ARRAY_START); i++) 
  {
    INTERNAL_RAM[(ARRAY_DATA_END_ADDRESS - 8) + i] = ptr1[3 - i];
  }

  //THE FOR LOOP BELLOW WRITES ARRAY END ADDRESS IN NEXT 4 BYTES OF THE NEW SPACE
  byte* ptr2 = (byte*)&ARRAY_END;
  for (byte i = 0; i < sizeof(ARRAY_END); i++) 
  {
    INTERNAL_RAM[(ARRAY_DATA_END_ADDRESS - 4) + i] = ptr2[3 - i];
  }

  return 1;
}

int8_t WRITE_BOOL_INTERNAL(String VARNAME, bool VALUE) 
{
  VARNAME.trim();

  while(VARNAME.length() < 8)
  {
    VARNAME += "_";
  }

  if (CHECK_VARIABLE_EXISTS(VARNAME, "NULL") != -1) {
    return -1;
  }

  ALLOC_BOOL();
  byte* ptr = (byte*)&VALUE;
  for (int i = 0; i < 8; i++) {
    INTERNAL_RAM[(BOOL_END_ADDRESS - 8) + i] = VARNAME.charAt(i);
  }

  for (byte i = 0; i < sizeof(VALUE); i++) {
    INTERNAL_RAM[BOOL_END_ADDRESS - i] = ptr[i];
  }

  return 1;
}

int8_t STORE_IN_BOOL(String VARNAME, int32_t ARRAY_INDEX, bool VALUE)
{
  VARNAME.trim();
  int32_t VAR_ADDRESS = CHECK_VARIABLE_EXISTS(VARNAME, "BOOL");
  int32_t ARRAY_START = GET_ARRAY_INFO(VARNAME, "BOOL", 0);
  int32_t ARRAY_END = GET_ARRAY_INFO(VARNAME, "BOOL", 1);
  uint8_t ARRAY_ELEMENT_SIZE;
  uint32_t ARRAY_LENGTH;

  

  if(VAR_ADDRESS < 0) 
  {
    return -3; // NO SUCH ARRAY OR VARIABLE WITH PROVIDED NAME EXISTS
  }

  if(ARRAY_INDEX > 0 && ARRAY_START == -1)
  {
    return -1; // VARIABLE ISN`T ARRAY
  }

  if(ARRAY_INDEX < 0 && ARRAY_START > -1)
  {
    return -1; // ARRAY ISNT VARIABLE LOL
  }

  if(ARRAY_START > -1)
  {

    if(ARRAY_END <= DOUBLE_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 16;
    }else if(ARRAY_END <= INT64_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 16;
    }else if(ARRAY_END <= UINT64_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 16;
    }else if(ARRAY_END <= INT32_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 12;
    }else if(ARRAY_END <= UINT32_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 12;
    }else if(ARRAY_END <= INT16_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 10;
    }else if(ARRAY_END <= UINT16_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 10;
    }else if(ARRAY_END <= INT8_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 9;
    }else if(ARRAY_END <= UINT8_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 9;
    }else if(ARRAY_END <= BOOL_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 9;
    }else if(ARRAY_END <= CHAR_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 9;
    }

    ARRAY_LENGTH = (ARRAY_END - ARRAY_START + 1) / ARRAY_ELEMENT_SIZE;

    if(ARRAY_INDEX > ARRAY_LENGTH - 1)
    {
      return -2; // TRYING TO ACCESS OUT OF ARRAY BOUNDS
    }

    byte* ptr = (byte*)&VALUE;
    for (byte i = 0; i < sizeof(VALUE); i++) 
    {
      INTERNAL_RAM[(ARRAY_START + (ARRAY_INDEX * ARRAY_ELEMENT_SIZE) + 8) - i] = ptr[i];
    }
  }

  if(ARRAY_START < 0)
  {
    byte* ptr = (byte*)&VALUE;
    for (byte i = 0; i < sizeof(VALUE); i++) 
    {
      INTERNAL_RAM[(VAR_ADDRESS + 8) - i] = ptr[i];
    }
  }

  return 1;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------


//----------------------------CHAR FUNCTIONS------------------------------------------------------------------------------------------------------------------------

int ALLOC_CHAR() 
{

  CHAR_END_ADDRESS = CHAR_END_ADDRESS + CHAR_SIZE;
  for (int i = CHAR_END_ADDRESS; i > (CHAR_END_ADDRESS - CHAR_SIZE); i--) 
  {
    INTERNAL_RAM[i] = 0;
  }

  return 1;
}

int8_t ALLOC_CHAR_ARRAY(String ARRAY_NAME, uint16_t ARRAY_LENGTH)  //RETURNS -1 IF OPERATION WAS  UNSUCCESSFULL, RETURNS 1 IF OPERATION WAS SUCCESSFULL.
{
  ARRAY_NAME.trim();

  // JUST CHECKING IF VARIABLES WITH THIS NAME ALREADY EXIST
  if(CHECK_VARIABLE_EXISTS(ARRAY_NAME, "NULL") != -1) 
  {
    return -1;
  }

  //IF ARRAY IS LARGER THAN 2000, OR IF ARRAY_NAME HAS WRONG SIZE, OR IF MEMORY IS FULL, ERROR THIS SH*T 
  if(ARRAY_LENGTH > 2000 || ARRAY_NAME.length() != 8 || (ARRAY_DATA_START_ADDRESS - 1) - (CHAR_END_ADDRESS + 1) < 16)
  {
    return -1;
  }

  //ALLOCATING ARRAY SPACE AND SAVING ITS START AND END ADDRESSES
  uint32_t ARRAY_START = CHAR_END_ADDRESS + 1;

  for(int i = 0; i < ARRAY_LENGTH; i++)
  {
    ALLOC_CHAR();
  }

  uint32_t ARRAY_END = CHAR_END_ADDRESS;

  //FROM HERE IT WILL WRITE ARRAY NAME AND START AND END ADDRESSES IN ARRAY DATA MEMORY SECTION
  ARRAY_DATA_START_ADDRESS = ARRAY_DATA_START_ADDRESS - 16;

  //THE FOR LOOP BELLOW SHIFTS ARRAY DATA 16 BYTES LEFT 
  for(int i = ARRAY_DATA_START_ADDRESS + 16; i <= ARRAY_DATA_END_ADDRESS; i++)
  {
    INTERNAL_RAM[i - 16] = INTERNAL_RAM[i];
  }

  //THE FOR LOOP BELLOW FILLS THE NEW SPACE WITH ZEROS, JUST TO BE SAFE
  for(int i = 0; i < 16; i++)
  {
    INTERNAL_RAM[(ARRAY_DATA_END_ADDRESS - 16) + i] = 0;
  }

  //THE FOR LOOP BELLOW WRITES ARRAY NAME IN FIRST 8 BYTES OF THE NEW SPACE
  for(int i = 0; i < 8; i++)
  {
    INTERNAL_RAM[(ARRAY_DATA_END_ADDRESS - 16) + i] = ARRAY_NAME.charAt(i);
  }

  //THE FOR LOOP BELLOW WRITES ARRAY START ADDRESS IN NEXT 4 BYTES OF THE NEW SPACE
  byte* ptr1 = (byte*)&ARRAY_START;
  for (byte i = 0; i < sizeof(ARRAY_START); i++) 
  {
    INTERNAL_RAM[(ARRAY_DATA_END_ADDRESS - 8) + i] = ptr1[3 - i];
  }

  //THE FOR LOOP BELLOW WRITES ARRAY END ADDRESS IN NEXT 4 BYTES OF THE NEW SPACE
  byte* ptr2 = (byte*)&ARRAY_END;
  for (byte i = 0; i < sizeof(ARRAY_END); i++) 
  {
    INTERNAL_RAM[(ARRAY_DATA_END_ADDRESS - 4) + i] = ptr2[3 - i];
  }

  return 1;
}

int8_t WRITE_CHAR_INTERNAL(String VARNAME, char VALUE) 
{
  VARNAME.trim();

  while(VARNAME.length() < 8)
  {
    VARNAME += "_";
  }

  if (CHECK_VARIABLE_EXISTS(VARNAME, "NULL") != -1) 
  {
    return -1;
  }

  ALLOC_CHAR();
  byte* ptr = (byte*)&VALUE;
  for (int i = 0; i < 8; i++) {
    INTERNAL_RAM[(CHAR_END_ADDRESS - 8) + i] = VARNAME.charAt(i);
  }

  for (byte i = 0; i < sizeof(VALUE); i++) {
    INTERNAL_RAM[CHAR_END_ADDRESS - i] = ptr[i];
  }

  return 1;
}

int8_t STORE_IN_CHAR(String VARNAME, int32_t ARRAY_INDEX, char VALUE)
{
  VARNAME.trim();
  int32_t VAR_ADDRESS = CHECK_VARIABLE_EXISTS(VARNAME, "CHAR");
  int32_t ARRAY_START = GET_ARRAY_INFO(VARNAME, "CHAR", 0);
  int32_t ARRAY_END = GET_ARRAY_INFO(VARNAME, "CHAR", 1);
  uint8_t ARRAY_ELEMENT_SIZE;
  uint32_t ARRAY_LENGTH;

  

  if(VAR_ADDRESS < 0) 
  {
    return -3; // NO SUCH ARRAY OR VARIABLE WITH PROVIDED NAME EXISTS
  }

  if(ARRAY_INDEX > 0 && ARRAY_START == -1)
  {
    return -100; // VARIABLE ISN`T ARRAY
  }

  if(ARRAY_INDEX < 0 && ARRAY_START > -1)
  {
    return -1; // ARRAY ISNT VARIABLE LOL
  }

  if(ARRAY_START > -1)
  {

    if(ARRAY_END <= DOUBLE_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 16;
    }else if(ARRAY_END <= INT64_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 16;
    }else if(ARRAY_END <= UINT64_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 16;
    }else if(ARRAY_END <= INT32_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 12;
    }else if(ARRAY_END <= UINT32_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 12;
    }else if(ARRAY_END <= INT16_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 10;
    }else if(ARRAY_END <= UINT16_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 10;
    }else if(ARRAY_END <= INT8_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 9;
    }else if(ARRAY_END <= UINT8_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 9;
    }else if(ARRAY_END <= BOOL_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 9;
    }else if(ARRAY_END <= CHAR_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 9;
    }

    ARRAY_LENGTH = (ARRAY_END - ARRAY_START + 1) / ARRAY_ELEMENT_SIZE;

    if(ARRAY_INDEX > ARRAY_LENGTH - 1)
    {
      return -2; // TRYING TO ACCESS OUT OF ARRAY BOUNDS
    }

    byte* ptr = (byte*)&VALUE;
    for (byte i = 0; i < sizeof(VALUE); i++) 
    {
      INTERNAL_RAM[(ARRAY_START + (ARRAY_INDEX * ARRAY_ELEMENT_SIZE) + 8) - i] = ptr[i];
    }
  }

  if(ARRAY_START < 0)
  {
    byte* ptr = (byte*)&VALUE;
    for (byte i = 0; i < sizeof(VALUE); i++) 
    {
      INTERNAL_RAM[(VAR_ADDRESS + 8) - i] = ptr[i];
    }
  }

  return 1;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------------------


/*int CHANGE_VARIABLE_VALUE(String VARNAME) 
{

}*/

/*int REMAINING_RAM() 
{

}*/

double READ_VARIABLE(String VARNAME, int32_t ARRAY_INDEX) 
{
  VARNAME.trim();

  int32_t VAR_ADDRESS = CHECK_VARIABLE_EXISTS(VARNAME, "NULL");
  int32_t ARRAY_START = GET_ARRAY_INFO(VARNAME, "NULL", 0);
  int32_t ARRAY_END = GET_ARRAY_INFO(VARNAME, "NULL", 1);
  uint8_t ARRAY_ELEMENT_SIZE;
  uint32_t ARRAY_LENGTH;

  

  if(VAR_ADDRESS < 0) 
  {
    return -3; // NO SUCH ARRAY OR VARIABLE WITH PROVIDED NAME EXISTS
  }

  if(ARRAY_INDEX > 0 && ARRAY_START == -1)
  {
    return -1; // VARIABLE ISN`T ARRAY
  }

  if(ARRAY_INDEX < 0 && ARRAY_START > -1)
  {
    return -1; // ARRAY ISNT VARIABLE LOL
  }

  if(ARRAY_START > -1)
  {

    if(ARRAY_END <= DOUBLE_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 16;
    }else if(ARRAY_END <= INT64_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 16;
    }else if(ARRAY_END <= UINT64_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 16;
    }else if(ARRAY_END <= INT32_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 12;
    }else if(ARRAY_END <= UINT32_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 12;
    }else if(ARRAY_END <= INT16_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 10;
    }else if(ARRAY_END <= UINT16_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 10;
    }else if(ARRAY_END <= INT8_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 9;
    }else if(ARRAY_END <= UINT8_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 9;
    }else if(ARRAY_END <= BOOL_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 9;
    }else if(ARRAY_END <= CHAR_END_ADDRESS)
    {
      ARRAY_ELEMENT_SIZE = 9;
    }

    ARRAY_LENGTH = (ARRAY_END - ARRAY_START + 1) / ARRAY_ELEMENT_SIZE;

    if(ARRAY_INDEX > ARRAY_LENGTH - 1)
    {
      return -2; // TRYING TO ACCESS OUT OF ARRAY BOUNDS
    }
  }
  
  if(ARRAY_INDEX < 0)
  {
    ARRAY_INDEX = 0;
  }

  if(VAR_ADDRESS < DOUBLE_END_ADDRESS)
  {
    
    double value = 0;
    uint64_t n = 1;
    uint8_t valueBytes[8];
    for (int k = 0; k < 8; k++) 
    {
      valueBytes[k] = INTERNAL_RAM[VAR_ADDRESS + (ARRAY_INDEX * DOUBLE_SIZE) + 15 - k];
    }

    uint64_t combined = 0;
    for (int k = 0; k < 8; k++) 
    {
      combined |= ((uint64_t)valueBytes[k] << (k * 8));
    }

    memcpy(&value, &combined, sizeof(double));

    return value;
  }else if(VAR_ADDRESS < INT64_END_ADDRESS)
  {
    int64_t value = 0;
    uint64_t n = 1;
    for (int k = 0; k < 8; k++) 
    {
      uint8_t valueByte = INTERNAL_RAM[VAR_ADDRESS + (ARRAY_INDEX * INT64_SIZE) + 15 - k];

      for (int l = 0; l < 8; l++) 
      {
        uint8_t mask = 1 << l;
        if (mask & valueByte) 
        {
          value = value + n;
        }

        n = n * 2;
      }
    }

    return value;
  }else if(VAR_ADDRESS < UINT64_END_ADDRESS)
  {
    uint64_t value = 0;
    uint64_t n = 1;
    for (int k = 0; k < 8; k++) 
    {
      uint8_t valueByte = INTERNAL_RAM[VAR_ADDRESS + (ARRAY_INDEX * INT64_SIZE) + 15 - k];

      for (int l = 0; l < 8; l++) 
      {
        uint8_t mask = 1 << l;
        if (mask & valueByte) 
        {
          value = value + n;
        }

        n = n * 2;
      }
    }

    return value;
  }else if(VAR_ADDRESS < INT32_END_ADDRESS)
  {
    int32_t value = 0;
    uint64_t n = 1;
    for (int k = 0; k < 4; k++) 
    {
      uint8_t valueByte = INTERNAL_RAM[VAR_ADDRESS + (ARRAY_INDEX * INT32_SIZE) + 11 - k];

      for (int l = 0; l < 8; l++) 
      {
        uint8_t mask = 1 << l;
        if (mask & valueByte) 
        {
          value = value + n;
        }

        n = n * 2;
      }
    }

    return value;
  }else if(VAR_ADDRESS < UINT32_END_ADDRESS)
  {
    uint32_t value = 0;
    uint64_t n = 1;
    for (int k = 0; k < 4; k++) 
    {
      uint8_t valueByte = INTERNAL_RAM[VAR_ADDRESS + (ARRAY_INDEX * INT32_SIZE) + 11 - k];

      for (int l = 0; l < 8; l++) 
      {
        uint8_t mask = 1 << l;
        if (mask & valueByte) 
        {
          value = value + n;
        }

        n = n * 2;
      }
    }

    return value;
  }else if(VAR_ADDRESS < INT16_END_ADDRESS)
  {
    int16_t value = 0;
    uint64_t n = 1;
    for (int k = 0; k < 2; k++) 
    {
      uint8_t valueByte = INTERNAL_RAM[VAR_ADDRESS + (ARRAY_INDEX * INT16_SIZE) + 9 - k];

      for (int l = 0; l < 8; l++) 
      {
        uint8_t mask = 1 << l;
        if (mask & valueByte) 
        {
          value = value + n;
        }

        n = n * 2;
      }
    }

    return value;
  }else if(VAR_ADDRESS < UINT16_END_ADDRESS)
  {
    uint16_t value = 0;
    uint64_t n = 1;
    for (int k = 0; k < 2; k++) 
    {
      uint8_t valueByte = INTERNAL_RAM[VAR_ADDRESS + (ARRAY_INDEX * INT16_SIZE) + 9 - k];

      for (int l = 0; l < 8; l++) 
      {
        uint8_t mask = 1 << l;
        if (mask & valueByte) 
        {
          value = value + n;
        }

        n = n * 2;
      }
    }

    return value;
  }else if(VAR_ADDRESS < INT8_END_ADDRESS)
  {
    int8_t value = 0;
    uint64_t n = 1;
    
    uint8_t valueByte = INTERNAL_RAM[VAR_ADDRESS + (ARRAY_INDEX * INT8_SIZE) + 8];

    for (int l = 0; l < 8; l++) 
    {
      uint8_t mask = 1 << l;
      if (mask & valueByte) 
      {
        value = value + n;
      }

      n = n * 2;
    }

    return value;
  }else if(VAR_ADDRESS < UINT8_END_ADDRESS)
  {
    uint8_t value = 0;
    uint64_t n = 1;
    
    uint8_t valueByte = INTERNAL_RAM[VAR_ADDRESS + (ARRAY_INDEX * INT8_SIZE) + 8];

    for (int l = 0; l < 8; l++) 
    {
      uint8_t mask = 1 << l;
      if (mask & valueByte) 
      {
        value = value + n;
      }

      n = n * 2;
    }
    
    return value;
  }else if(VAR_ADDRESS < BOOL_END_ADDRESS)
  {
    bool valueByte = INTERNAL_RAM[VAR_ADDRESS + (ARRAY_INDEX * BOOL_SIZE) + 8];

    return valueByte;
  }else if(VAR_ADDRESS < CHAR_END_ADDRESS)
  {
    char valueByte = INTERNAL_RAM[VAR_ADDRESS + (ARRAY_INDEX * CHAR_SIZE) + 8];

    return valueByte;
  }  

  return -1;
}

/*void writeByte(uint16_t address, uint8_t value)
{
  //Serial.println(instruction, BIN);
  //Serial.println(address, BIN);
  //Serial.println(value, BIN);

  digitalWrite(20, LOW);
  SPI.transfer(2);
  SPI.transfer16(address);
  SPI.transfer(value);
  digitalWrite(20, HIGH);
}

void readByte(uint16_t address)
{

  digitalWrite(20, LOW);
  SPI.transfer(3);
  SPI.transfer16(address);
  uint8_t value = SPI.transfer(0);
  digitalWrite(20, HIGH);

  Serial.println(value);
}

int write_uint64_ext(uint16_t address, uint64_t value)
{

  byte* ptr = (byte*)&value;
  for (byte i = 0; i < sizeof(value); i++) 
  {
    digitalWrite(20, LOW);
    SPI.transfer(2);
    SPI.transfer16(address+i);
    SPI.transfer(ptr[7-i]);
    digitalWrite(20, HIGH);
  }

  return 1;
}

int read_uint64_ext(uint16_t address)
{
  //Serial.println(instruction, BIN);
  //Serial.println(address, BIN);
  //Serial.println(value, BIN);

  uint64_t value = 0;
  uint64_t n = 1;
  for(int i = 0; i < 8; i++)
  {
    digitalWrite(20, LOW);
    SPI.transfer(3);
    SPI.transfer16((address + 7) - i);
    uint8_t valueByte = SPI.transfer(0);
    digitalWrite(20, HIGH);
    
    for(int j = 0; j < 8; j++)
    {
      uint8_t mask = 1 << j;
      if(mask & valueByte)
      {
        value = value + n;
      }

      n = n * 2;
    }
  }

  Serial.print("\nValue Read: ");
  Serial.print(value);
  
  return 1;
}

int write_int64_ext(uint16_t address, int64_t value)
{
  byte* ptr = (byte*)&value;
  for (byte i = 0; i < sizeof(value); i++) 
  {
    digitalWrite(20, LOW);
    SPI.transfer(2);
    SPI.transfer16(address+i);
    SPI.transfer(ptr[7-i]);
    digitalWrite(20, HIGH);
  }
  return 1;
}

int read_int64_ext(uint16_t address)
{
  //Serial.println(instruction, BIN);
  //Serial.println(address, BIN);
  //Serial.println(value, BIN);

  int64_t value = 0;
  uint64_t n = 1;
  for(int i = 0; i < 8; i++)
  {
    digitalWrite(20, LOW);
    SPI.transfer(3);
    SPI.transfer16((address + 7) - i);
    uint8_t valueByte = SPI.transfer(0);
    digitalWrite(20, HIGH);

    
    for(int j = 0; j < 8; j++)
    {
      uint8_t mask = 1 << j;
      if(mask & valueByte)
      {
        value = value + n;
      }

      n = n * 2;
    }
  }

  Serial.print("\nValue Read: ");
  Serial.print(value);
  
  return 1;
}

void CLEAR_RAM()
{
  for(int i = 0; i < 8192; i++)
  {
    writeByte(i, 0);
  }
}

int RAM_CHECK()
{

  while(true)
  {
    digitalWrite(20, LOW);
    SPI.transfer(2);
    SPI.transfer16(0);
    SPI.transfer(10);
    digitalWrite(20, HIGH);

    digitalWrite(20, LOW);
    SPI.transfer(3);
    SPI.transfer16(0);
    uint8_t value = SPI.transfer(0);
    digitalWrite(20, HIGH);

    if(value == 10)
    {
      return 1;
    }else
    {
      Serial.print("\n");
      Serial.print(value);
      return -1;
    }
  }

}

void RDSR()
{

  digitalWrite(20, LOW);
  SPI.transfer(5);
  uint8_t value = SPI.transfer(0);
  digitalWrite(20, HIGH);

  Serial.println(value, BIN);
}

void WRSR(uint8_t mode)
{

  digitalWrite(20, LOW);
  SPI.transfer(1);
  SPI.transfer(mode);
  digitalWrite(20, HIGH);

}*/

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////      ^ ^             MEMORY  MANAGEMENT SYSTEM      ^ ^       ///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////