#include "pico/stdlib.h"
#include "pico/platform.h"
#include "Arduino.h"

#ifndef MY_FUNCTIONS_H
#define MY_FUNCTIONS_H

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////      V V             MEMORY  MANAGEMENT SYSTEM      V V       ///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//----------------------------GENERAL VARIABLE FUNCTIONS------------------------------------------------------------------------------------------------------------------------

int32_t GET_ARRAY_INFO(String ARRAY_NAME, String ARRAY_VARTYPE, uint8_t INFO_TYPE);

int32_t CHECK_VARIABLE_EXISTS(String VARNAME, String VARTYPE);

int8_t DELETE_ARRAY(String ARRAY_NAME);

int DELETE_VARIABLE(String VARNAME);

//----------------------------------------------------------------------------------------------------------------------------------------------------------------


//----------------------------DOUBLE FUNCTIONS------------------------------------------------------------------------------------------------------------------------

int ALLOC_DOUBLE();

int8_t ALLOC_DOUBLE_ARRAY(String ARRAY_NAME, uint16_t ARRAY_LENGTH);

//ALL VARIABLE WRITES BELOW RETURN 1 IF OPERATION SUCCEDED AND -1 IF OPERATION FAILED DUE TO VARIABLE ALREADY EXISTING.
int8_t WRITE_DOUBLE_INTERNAL(String VARNAME, double VALUE);

//RETURNS -1 IF VARIABLE/ARRAY WITH VARNAME NAME DOES NOT EXIST. ARRAY_INDEX MUST BE NEGATIVE IF YOU ARE NOT TRYING TO WRITE TO ARRAYS.
int8_t STORE_IN_DOUBLE(String VARNAME, int32_t ARRAY_INDEX, double VALUE);

//----------------------------------------------------------------------------------------------------------------------------------------------------------------


//----------------------------INT64 FUNCTIONS------------------------------------------------------------------------------------------------------------------------

int ALLOC_INT64();

int8_t ALLOC_INT64_ARRAY(String ARRAY_NAME, uint16_t ARRAY_LENGTH);

int8_t WRITE_INT64_INTERNAL(String VARNAME, int64_t VALUE);

//RETURNS -1 IF VARIABLE/ARRAY WITH VARNAME NAME DOES NOT EXIST. ARRAY_INDEX MUST BE NEGATIVE IF YOU ARE NOT TRYING TO WRITE TO ARRAYS.
int8_t STORE_IN_INT64(String VARNAME, int32_t ARRAY_INDEX, int64_t VALUE);

//----------------------------------------------------------------------------------------------------------------------------------------------------------------


//----------------------------UINT64 FUNCTIONS------------------------------------------------------------------------------------------------------------------------

int ALLOC_UINT64();

int8_t ALLOC_UINT64_ARRAY(String ARRAY_NAME, uint16_t ARRAY_LENGTH);

int8_t WRITE_UINT64_INTERNAL(String VARNAME, uint64_t VALUE);

//RETURNS -1 IF VARIABLE/ARRAY WITH VARNAME NAME DOES NOT EXIST. ARRAY_INDEX MUST BE NEGATIVE IF YOU ARE NOT TRYING TO WRITE TO ARRAYS.
int8_t STORE_IN_UINT64(String VARNAME, int32_t ARRAY_INDEX, uint64_t VALUE);

//----------------------------------------------------------------------------------------------------------------------------------------------------------------


//----------------------------INT32 FUNCTIONS------------------------------------------------------------------------------------------------------------------------

int ALLOC_INT32();

int8_t ALLOC_INT32_ARRAY(String ARRAY_NAME, uint16_t ARRAY_LENGTH);

int8_t WRITE_INT32_INTERNAL(String VARNAME, int32_t VALUE);

int8_t STORE_IN_INT32(String VARNAME, int32_t ARRAY_INDEX, int32_t VALUE);

//----------------------------------------------------------------------------------------------------------------------------------------------------------------


//----------------------------UINT32 FUNCTIONS------------------------------------------------------------------------------------------------------------------------

int ALLOC_UINT32();

int8_t ALLOC_UINT32_ARRAY(String ARRAY_NAME, uint16_t ARRAY_LENGTH);

int8_t WRITE_UINT32_INTERNAL(String VARNAME, uint32_t VALUE);

int8_t STORE_IN_UINT32(String VARNAME, int32_t ARRAY_INDEX, uint32_t VALUE);

//----------------------------------------------------------------------------------------------------------------------------------------------------------------


//----------------------------INT16 FUNCTIONS------------------------------------------------------------------------------------------------------------------------

int ALLOC_INT16();

int8_t ALLOC_INT16_ARRAY(String ARRAY_NAME, uint16_t ARRAY_LENGTH);

int8_t WRITE_INT16_INTERNAL(String VARNAME, int16_t VALUE);

int8_t STORE_IN_INT16(String VARNAME, int32_t ARRAY_INDEX, int16_t VALUE);

//----------------------------------------------------------------------------------------------------------------------------------------------------------------


//----------------------------UINT16 FUNCTIONS------------------------------------------------------------------------------------------------------------------------

int ALLOC_UINT16();

int8_t ALLOC_UINT16_ARRAY(String ARRAY_NAME, uint16_t ARRAY_LENGTH);

int8_t WRITE_UINT16_INTERNAL(String VARNAME, uint16_t VALUE);

int8_t STORE_IN_UINT16(String VARNAME, int32_t ARRAY_INDEX, uint16_t VALUE);

//----------------------------------------------------------------------------------------------------------------------------------------------------------------


//----------------------------INT8 FUNCTIONS------------------------------------------------------------------------------------------------------------------------

int ALLOC_INT8();

int8_t ALLOC_INT8_ARRAY(String ARRAY_NAME, uint16_t ARRAY_LENGTH);

int8_t WRITE_INT8_INTERNAL(String VARNAME, int8_t VALUE);

int8_t STORE_IN_INT8(String VARNAME, int32_t ARRAY_INDEX, int8_t VALUE);

//----------------------------------------------------------------------------------------------------------------------------------------------------------------


//----------------------------UINT8 FUNCTIONS------------------------------------------------------------------------------------------------------------------------

int ALLOC_UINT8();

int8_t ALLOC_UINT8_ARRAY(String ARRAY_NAME, uint16_t ARRAY_LENGTH);

int8_t WRITE_UINT8_INTERNAL(String VARNAME, uint8_t VALUE);

int8_t STORE_IN_UINT8(String VARNAME, int32_t ARRAY_INDEX, uint8_t VALUE);

//----------------------------------------------------------------------------------------------------------------------------------------------------------------


//----------------------------BOOL FUNCTIONS------------------------------------------------------------------------------------------------------------------------

int ALLOC_BOOL();

int8_t ALLOC_BOOL_ARRAY(String ARRAY_NAME, uint16_t ARRAY_LENGTH);

int8_t WRITE_BOOL_INTERNAL(String VARNAME, bool VALUE);

int8_t STORE_IN_BOOL(String VARNAME, int32_t ARRAY_INDEX, bool VALUE);

//----------------------------------------------------------------------------------------------------------------------------------------------------------------


//----------------------------CHAR FUNCTIONS------------------------------------------------------------------------------------------------------------------------

int ALLOC_CHAR();

int8_t ALLOC_CHAR_ARRAY(String ARRAY_NAME, uint16_t ARRAY_LENGTH);

int8_t WRITE_CHAR_INTERNAL(String VARNAME, char VALUE);

int8_t STORE_IN_CHAR(String VARNAME, int32_t ARRAY_INDEX, char VALUE);


//----------------------------------------------------------------------------------------------------------------------------------------------------------------


double READ_VARIABLE(String VARNAME, int32_t ARRAY_INDEX);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////      ^ ^             MEMORY  MANAGEMENT SYSTEM      ^ ^       ///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#endif