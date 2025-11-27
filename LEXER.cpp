#include <SPI.h>
#include <SD.h>
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
#include "mms.h"
#include "bytecode_interpreter.h"
#include "bytecode_compiler.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////      V V             LEXER                          V V       ///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

String line;
uint32_t errors = 0;
String errorMessage;

int precedenceCheck(char op)
{
  if(op == '+')
  {
    return 2;
  }else if(op == '-')
  {
    return 2;
  }else if(op == '*')
  {
    return 3;
  }else if(op == '/')
  {
    return 3;
  }

  return -1;
}

int minZero(int n)
{
  if(n < 0)
  {
    return 0;
  }else
  {
    return n;
  }
}

double mathsSolver(String expr)
{
  expr.trim();

  int lastTokenType = -1;
  char lastToken;

  bool negateNum = false;

  int exprPtr = 0;

  String outStack[100];
  int outStackPtr = 0;

  String opStack[100];
  int opStackPtr = 0;

  while(exprPtr < (expr.length() ))
  {
    char op = expr.charAt(exprPtr);

    if(op == ' ')
    {
      exprPtr++;
    }else if(isDigit(op) || isAlpha(op))
    {
      lastToken = '$';
      bool alphaIncluded = false;

      if(lastTokenType == 0)
      {
        //Serial.print("\nInvalid Expression.");
        return -11;
      }

      lastTokenType = 0;

      int pCount = 0;

      while(isDigit(op) || op == '.' || isAlpha(op))
      {
        if(isAlpha(op))
        {
          alphaIncluded = true;
        }

        if(op == '.')
        {
          pCount++;
        }

        outStack[outStackPtr] = outStack[outStackPtr] + String(op); 
        exprPtr++;
        op = expr.charAt(exprPtr);
      }

      if(pCount > 1 && alphaIncluded == false)
      {
        //Serial.print("\nInvalid Number: ");
        
        //Serial.print(outStack[outStackPtr]);
        return -2;
      }

      if(pCount > 0 && alphaIncluded == true)
      {
        //Serial.print("\nInvalid Variable: ");
        
        //Serial.print(outStack[outStackPtr]);
        return -3;
      }

      if(outStack[outStackPtr].length() > 8 && alphaIncluded == true)
      {
        //Serial.print("\nInvalid Variable: ");
        
        //Serial.print(outStack[outStackPtr]);
        return -4;
      }

      if(alphaIncluded == true && negateNum == true)
      {
        //errors++;

        //errorMessage = "Invalid Expression: missplaced minus before variable.";
        
        return -5;
      }

      /*if(CHECK_VARIABLE_EXISTS(outStack[outStackPtr], "NULL") == -1 && alphaIncluded == true) //MAYBE WILL USE THIS LOGIC IN THE FUTURE AFTER DELETING THE OLD MEMORY MANAGEMENT SYSTEM?
      {
        //Serial.print("\nVariable does not exist: ");
        
        //Serial.print(outStack[outStackPtr]);
        return -6;
      }*/

      /*if(alphaIncluded == true)//MAYBE WILL USE THIS LOGIC IN THE FUTURE AFTER DELETING THE OLD MEMORY MANAGEMENT SYSTEM?
      {
        
        outStack[outStackPtr] = String(READ_VARIABLE(outStack[outStackPtr], -1));
        
      }else*/ if(negateNum == true)
      {
        outStack[outStackPtr] = String(outStack[outStackPtr].toDouble() * -1);
      }
      
      negateNum = false;
      outStackPtr++;
    }else if(op == '(')
    {
      opStack[opStackPtr] = String(op);
      
      opStackPtr++;
      exprPtr++;
    }else if(op == ')')
    {
      if(lastTokenType == 1)
      {
        //Serial.print("\nInvalid Expression.");
        return -7;
      }

      opStackPtr--;
      while(opStack[opStackPtr].charAt(0) != '(')
      {
        outStack[outStackPtr] += opStack[opStackPtr];

        outStackPtr++;

        opStackPtr--;
      }

      exprPtr++;

    }else if(op == '+' || op == '/' || op == '-' || op == '*')
    {
      if(lastTokenType == 1 && lastToken == '*' && op != '-' || lastTokenType == 1 && lastToken != '*')
      {
        //Serial.print("\nInvalid Expression.");
        return -8;
      }

      if(lastTokenType == -1 && op != '-')
      {
        //Serial.print("\nInvalid Expression.");
        return -9;
      }

      if(lastToken == '*' && op == '-' || op == '-' && lastTokenType == -1)
      {
        negateNum = true;
        exprPtr++;
      }else
      {
        lastTokenType = 1;

        if(precedenceCheck(opStack[minZero(opStackPtr-1)].charAt(0)) >= precedenceCheck(op))
        { 
          opStackPtr = minZero(opStackPtr-1);
          while(precedenceCheck(opStack[opStackPtr].charAt(0)) >= precedenceCheck(op))
          {
            outStack[outStackPtr] = opStack[opStackPtr];

            outStackPtr++;
            opStackPtr--;
          }
          
          opStackPtr++;
        }
        
        lastToken = op;
        opStack[opStackPtr] = String(op);
        opStackPtr++;
        exprPtr++;
      }
      
    }else
    {
      //Serial.print("\nInvalid Expression.");
      return -10;
    }
    
  }

  opStackPtr--;
  while(opStackPtr > -1)
  {
    outStack[outStackPtr] += opStack[opStackPtr];

    outStackPtr++;

    opStackPtr--;
  }

  String solvStack[100];
  int solvStackPtr = 0;

  for(int i = 0; i < outStackPtr; i++)
  {
    if(isDigit(outStack[i].charAt(1)) && outStack[i].charAt(0) == '-' || isDigit(outStack[i].charAt(0)))
    {
      solvStack[solvStackPtr] += outStack[i];
      solvStackPtr++;
    }else if(outStack[i].charAt(0) == '+' || outStack[i].charAt(0) == '/' || outStack[i].charAt(0) == '-' || outStack[i].charAt(0) == '*')
    {
      if(outStack[i].charAt(0) == '+')
      {
        solvStack[solvStackPtr-2] = String(solvStack[solvStackPtr-2].toDouble() + solvStack[solvStackPtr-1].toDouble());
      }else if(outStack[i].charAt(0) == '-')
      {
        solvStack[solvStackPtr-2] = String(solvStack[solvStackPtr-2].toDouble() - solvStack[solvStackPtr-1].toDouble());
      }else if(outStack[i].charAt(0) == '*')
      {
        solvStack[solvStackPtr-2] = String(solvStack[solvStackPtr-2].toDouble() * solvStack[solvStackPtr-1].toDouble());
      }else if(outStack[i].charAt(0) == '/')
      {
        solvStack[solvStackPtr-2] = String(solvStack[solvStackPtr-2].toDouble() / solvStack[solvStackPtr-1].toDouble());
      }

      solvStack[solvStackPtr-1] = "";

      solvStackPtr = solvStackPtr - 1;
    }
  }
  
  return solvStack[0].toDouble();
}

String ARGUMENT_STRUCTURE_ANALYZER(String arg, String argStructure, int returnContentField)
{
  arg.trim();

  int whilecounter = 0;
  int argStructureCounter = 0;

  int textCounter = 0;
  int contentField = 0;

  int comma[2] = {0,0};
  int dquote[2] = {0,0};
  int squote[2] = {0,0};

  for(int i = 0; i < argStructure.length() ; i++)
  {
    if(argStructure.charAt(i) == 'C')
    {
      comma[0]++;
    }else if(argStructure.charAt(i) == 'Q')
    {
      dquote[0]++;
    }else if(argStructure.charAt(i) == 'q')
    {
      squote[0]++;
    }
  }

  for(int i = 0; i < arg.length() ; i++)
  {
    if(arg.charAt(i) == ',')
    {
      comma[1]++;
    }else if(arg.charAt(i) == '\"')
    {
      dquote[1]++;
    }else if(arg.charAt(i) == '\'')
    {
      squote[1]++;
    }
  }

  if(dquote[0] != dquote[1])
  {
    errors++;
    errorMessage = "ERROR -60: EXPECTED EXACTLY " + String((dquote[0] + 1) / 2) + " STRING FIELDS.";
    return "";
  }else if(squote[0] != squote[1])
  {
    errors++;
    errorMessage = "ERROR -61: EXPECTED EXACTLY " + String((squote[0] + 1) / 2) + " STRING FIELDS.";
    return "";
  }else if(comma[0] != comma[1])
  {
    errors++;
    errorMessage = "ERROR -62: EXPECTED EXACTLY " + String(comma[0] + 1) + " STRING FIELDS.";
    return "";
  }

  
  if(argStructure.charAt(0) == 'P')
  {
    if(arg.charAt(0) != '(' || arg.charAt(arg.length() - 1) != ')')
    {
      errors++;
      errorMessage = "ERROR: -50";
      return "";
    }
  }

  if(argStructure.charAt(0) == '[')
  {
    if(arg.charAt(0) != '[' || arg.charAt(arg.length() - 1) != ']')
    {
      errors++;
      errorMessage = "ERROR: -51";
      return "";
    }
  }

  if(argStructure.charAt(0) == '{')
  {
    if(arg.charAt(0) != '{' || arg.charAt(arg.length() - 1) != '}')
    {
      errors++;
      errorMessage = "ERROR: -52";
      return "";
    }
  }

  //check if symbol structure is correct
  while(argStructureCounter < argStructure.length())
  {

    if(argStructure[argStructureCounter] == 'p' && arg.charAt(arg.length() - 1) == ')') 
    {
      while(arg.charAt(whilecounter) != ')' && arg.charAt(whilecounter) != '\"' && arg.charAt(whilecounter) != '\'' && arg.charAt(whilecounter) != ',' && 
            arg.charAt(whilecounter) != '[' && arg.charAt(whilecounter) != ']' && arg.charAt(whilecounter) != '{' && arg.charAt(whilecounter) != '}' && arg.charAt(whilecounter) != ';' && whilecounter < arg.length())
      {
        whilecounter++;
      }

      whilecounter++;

    }else if(argStructure[argStructureCounter] == 'C')
    {
      while(arg.charAt(whilecounter) != '\"' && arg.charAt(whilecounter) != '\'' && arg.charAt(whilecounter) != ',' && 
            arg.charAt(whilecounter) != '[' && arg.charAt(whilecounter) != ']' && arg.charAt(whilecounter) != '{' && arg.charAt(whilecounter) != '}' && arg.charAt(whilecounter) != ';' && whilecounter < arg.length())
      {
        whilecounter++;
      }

      if(arg.charAt(whilecounter) != ',')
      {

        Serial.print("\nERROR: EXPECTED \',\' HERE -> \""); //28
        Serial.print(arg);
        Serial.print("\"\n");
        for(int i = 0; i < whilecounter + 29; i++)
        {
          Serial.print(" ");
        }
        Serial.print("^\n");
        return "\nERROR: -22";
      }
      comma[1]++;
      whilecounter++;

    }else if(argStructure[argStructureCounter] == 'Q')
    {
      while(arg.charAt(whilecounter) != '\"' && arg.charAt(whilecounter) != '\'' && arg.charAt(whilecounter) != ',' && 
            arg.charAt(whilecounter) != '[' && arg.charAt(whilecounter) != ']' && arg.charAt(whilecounter) != '{' && arg.charAt(whilecounter) != '}' && arg.charAt(whilecounter) != ';' && whilecounter < arg.length())
      {
        whilecounter++;
      }

      if(arg.charAt(whilecounter) != '\"')
      {
        Serial.print("\nERROR: EXPECTED \'\"\' HERE -> \""); //28
        Serial.print(arg);
        Serial.print("\"\n");
        for(int i = 0; i < whilecounter + 29; i++)
        {
          Serial.print(" ");
        }
        Serial.print("^\n");
        return "\nERROR: -23";
      }
      dquote[1]++;
      whilecounter++;

    }else if(argStructure[argStructureCounter] == 'q')
    {
      while(arg.charAt(whilecounter) != '\"' && arg.charAt(whilecounter) != '\'' && arg.charAt(whilecounter) != ',' && 
            arg.charAt(whilecounter) != '[' && arg.charAt(whilecounter) != ']' && arg.charAt(whilecounter) != '{' && arg.charAt(whilecounter) != '}' && arg.charAt(whilecounter) != ';' && whilecounter < arg.length())
      {
        whilecounter++;
      }

      if(arg.charAt(whilecounter) != '\'')
      {
        Serial.print("\nERROR: EXPECTED \'\'\' HERE -> \""); //28
        Serial.print(arg);
        Serial.print("\"\n");
        for(int i = 0; i < whilecounter + 29; i++)
        {
          Serial.print(" ");
        }
        Serial.print("^\n");
        return "\nERROR: -24";
      }
      squote[1]++;
      whilecounter++;

    }else if(argStructure[argStructureCounter] == ';')
    {
      while(arg.charAt(whilecounter) != '\"' && arg.charAt(whilecounter) != '\'' && arg.charAt(whilecounter) != ',' && 
            arg.charAt(whilecounter) != '[' && arg.charAt(whilecounter) != ']' && arg.charAt(whilecounter) != '{' && arg.charAt(whilecounter) != '}' && arg.charAt(whilecounter) != ';' && whilecounter < arg.length())
      {
        whilecounter++;
      }

      if(arg.charAt(whilecounter) != ';')
      {
        Serial.print("\nERROR: EXPECTED \';\' HERE -> \""); //28
        Serial.print(arg);
        Serial.print("\"\n");
        for(int i = 0; i < whilecounter + 29; i++)
        {
          Serial.print(" ");
        }
        Serial.print("^\n");
        return "\nERROR: -25";
      }
      whilecounter++;

    }else if(argStructure[argStructureCounter] == '}')
    {
      while(arg.charAt(whilecounter) != '\"' && arg.charAt(whilecounter) != '\'' && arg.charAt(whilecounter) != ',' && 
            arg.charAt(whilecounter) != '[' && arg.charAt(whilecounter) != ']' && arg.charAt(whilecounter) != '{' && arg.charAt(whilecounter) != '}' && arg.charAt(whilecounter) != ';' && whilecounter < arg.length())
      {
        whilecounter++;
      }

      if(arg.charAt(whilecounter) != '}')
      {
        Serial.print("\nERROR: EXPECTED \'}\' HERE -> \""); //28
        Serial.print(arg);
        Serial.print("\"\n");
        for(int i = 0; i < whilecounter + 29; i++)
        {
          Serial.print(" ");
        }
        Serial.print("^\n");
        return "\nERROR: -26";
      }
      whilecounter++;

    }else if(argStructure[argStructureCounter] == '{')
    {
      while(arg.charAt(whilecounter) != '\"' && arg.charAt(whilecounter) != '\'' && arg.charAt(whilecounter) != ',' && 
            arg.charAt(whilecounter) != '[' && arg.charAt(whilecounter) != ']' && arg.charAt(whilecounter) != '{' && arg.charAt(whilecounter) != '}' && arg.charAt(whilecounter) != ';' && whilecounter < arg.length())
      {
        whilecounter++;
      }

      if(arg.charAt(whilecounter) != '{')
      {
        Serial.print("\nERROR: EXPECTED \'{\' HERE -> \""); //28
        Serial.print(arg);
        Serial.print("\"\n");
        for(int i = 0; i < whilecounter + 29; i++)
        {
          Serial.print(" ");
        }
        Serial.print("^\n");
        return "\nERROR: -27";
      }
      whilecounter++;

    }else if(argStructure[argStructureCounter] == ']')
    {
      while(arg.charAt(whilecounter) != '\"' && arg.charAt(whilecounter) != '\'' && arg.charAt(whilecounter) != ',' && 
            arg.charAt(whilecounter) != '[' && arg.charAt(whilecounter) != ']' && arg.charAt(whilecounter) != '{' && arg.charAt(whilecounter) != '}' && arg.charAt(whilecounter) != ';' && whilecounter < arg.length())
      {
        whilecounter++;
      }

      if(arg.charAt(whilecounter) != ']')
      {
        Serial.print("\nERROR: EXPECTED \']\' HERE -> \""); //28
        Serial.print(arg);
        Serial.print("\"\n");
        for(int i = 0; i < whilecounter + 29; i++)
        {
          Serial.print(" ");
        }
        Serial.print("^\n");
        return "\nERROR: -28";
      }
      whilecounter++;

    }else if(argStructure[argStructureCounter] == '[')
    {
      while(arg.charAt(whilecounter) != '\"' && arg.charAt(whilecounter) != '\'' && arg.charAt(whilecounter) != ',' && 
            arg.charAt(whilecounter) != '[' && arg.charAt(whilecounter) != ']' && arg.charAt(whilecounter) != '{' && arg.charAt(whilecounter) != '}' && arg.charAt(whilecounter) != ';' && whilecounter < arg.length())
      {
        whilecounter++;
      }

      if(arg.charAt(whilecounter) != '[')
      {
        Serial.print("\nERROR: EXPECTED \'[\' HERE -> \""); //28
        Serial.print(arg);
        Serial.print("\"\n");
        for(int i = 0; i < whilecounter + 29; i++)
        {
          Serial.print(" ");
        }
        Serial.print("^\n");
        return "\nERROR: -29";
      }
      whilecounter++;

    }

    argStructureCounter++;

    /*if(argStructureCounter > argStructure.length() && whilecounter < arg.length())
    {
      return -30;
    }else if(whilecounter > arg.length() && argStructureCounter <= argStructure.length())
    {
      return -35;
    }*/
  }

  whilecounter = 0;
  argStructureCounter = 0;

  while(argStructureCounter < argStructure.length())
  {
    if(argStructure.charAt(argStructureCounter) == 'P') // (
    {

      while(arg.charAt(whilecounter) != '(' && arg.charAt(whilecounter) != ')' && arg.charAt(whilecounter) != '\"' && arg.charAt(whilecounter) != '\'' && arg.charAt(whilecounter) != ',' && 
            arg.charAt(whilecounter) != '[' && arg.charAt(whilecounter) != ']' && arg.charAt(whilecounter) != '{' && arg.charAt(whilecounter) != '}' && arg.charAt(whilecounter) != ';')
      {

        if(whilecounter < arg.length())
        {
          whilecounter++;
        }else
        {
          break;
        }
      }

      if(arg.charAt(whilecounter) == '(')
      {
        argStructureCounter++;
        whilecounter++;
      }else
      {
        errors++;
        errorMessage = "ERROR: -2";
        return "";
      }
    }

    if(argStructure.charAt(argStructureCounter) == 'p')// )
    {
      while(arg.charAt(whilecounter) != '(' && arg.charAt(whilecounter) != ')' && arg.charAt(whilecounter) != '\"' && arg.charAt(whilecounter) != '\'' && arg.charAt(whilecounter) != ',' && 
            arg.charAt(whilecounter) != '[' && arg.charAt(whilecounter) != ']' && arg.charAt(whilecounter) != '{' && arg.charAt(whilecounter) != '}' && arg.charAt(whilecounter) != ';')
      {

        if(whilecounter < arg.length())
        {
          whilecounter++;
        }else
        {
          break;
        }
      }

      if(arg.charAt(whilecounter) == ')')
      {
        argStructureCounter++;
        whilecounter++;
      }else
      {
        errors++;
        errorMessage = "ERROR: -4";
        return "";
      }
    }

    if(argStructure.charAt(argStructureCounter) == 'C')// ,
    {
      while(arg.charAt(whilecounter) != '(' && arg.charAt(whilecounter) != ')' && arg.charAt(whilecounter) != '\"' && arg.charAt(whilecounter) != '\'' && arg.charAt(whilecounter) != ',' && 
            arg.charAt(whilecounter) != '[' && arg.charAt(whilecounter) != ']' && arg.charAt(whilecounter) != '{' && arg.charAt(whilecounter) != '}' && arg.charAt(whilecounter) != ';')
      {

        if(whilecounter < arg.length())
        {
          whilecounter++;
        }else
        {
          break;
        }
      }

      if(arg.charAt(whilecounter) == ',')
      {
        argStructureCounter++;
        whilecounter++;
      }else
      {
        errors++;
        errorMessage = "ERROR: -6";
        return "";
      }
    }

    if(argStructure.charAt(argStructureCounter) == 'Q')// "
    {
      while(arg.charAt(whilecounter) != '(' && arg.charAt(whilecounter) != ')' && arg.charAt(whilecounter) != '\"' && arg.charAt(whilecounter) != '\'' && arg.charAt(whilecounter) != ',' && 
            arg.charAt(whilecounter) != '[' && arg.charAt(whilecounter) != ']' && arg.charAt(whilecounter) != '{' && arg.charAt(whilecounter) != '}' && arg.charAt(whilecounter) != ';')
      {

        if(argStructure.charAt(argStructureCounter-1) == ',' || argStructure.charAt(argStructureCounter-1) == '(')
        {
          if(arg.charAt(whilecounter) != ' ')
          {
            errors++;
            errorMessage = "ERROR: -81";
            return "";
          }
        }

        if(whilecounter < arg.length())
        {
          whilecounter++;
        }else
        {
          break;
        }
      }

      if(arg.charAt(whilecounter) == '\"')
      {
        argStructureCounter++;
        whilecounter++;
      }else
      {
        errors++;
        errorMessage = "ERROR: -8";
        return "";
      }
    }

    if(argStructure.charAt(argStructureCounter) == 'q')// '
    {
      while(arg.charAt(whilecounter) != '(' && arg.charAt(whilecounter) != ')' && arg.charAt(whilecounter) != '\"' && arg.charAt(whilecounter) != '\'' && arg.charAt(whilecounter) != ',' && 
            arg.charAt(whilecounter) != '[' && arg.charAt(whilecounter) != ']' && arg.charAt(whilecounter) != '{' && arg.charAt(whilecounter) != '}' && arg.charAt(whilecounter) != ';')
      {

        if(argStructure.charAt(argStructureCounter-1) == ',' || argStructure.charAt(argStructureCounter-1) == '(')
        {
          if(arg.charAt(whilecounter) != ' ')
          {
            errors++;
            errorMessage = "ERROR: -81";
            return "";
          }
        }

        if(whilecounter < arg.length())
        {
          whilecounter++;
        }else
        {
          break;
        }
      }

      if(arg.charAt(whilecounter) == '\'')
      {
        argStructureCounter++;
        whilecounter++;
      }else
      {
        errors++;
        errorMessage = "ERROR: -3";
        return "";
      }
    }

    if(argStructure.charAt(argStructureCounter) == 'N')// numbers, expressions and/or variables
    {
      uint8_t dotCounter = 0;
      String expression;
      contentField++;

      int correction = 0;

      if(argStructure.endsWith("p"))
      {
        correction = 1;
      }

      while(arg.charAt(whilecounter) != '\"' && arg.charAt(whilecounter) != '\'' && arg.charAt(whilecounter) != ',' && 
            arg.charAt(whilecounter) != '[' && arg.charAt(whilecounter) != ']' && arg.charAt(whilecounter) != '{' && arg.charAt(whilecounter) != '}' && arg.charAt(whilecounter) != ';' && whilecounter < arg.length() - correction)
      {

        if(arg.charAt(whilecounter) == '1' || arg.charAt(whilecounter) == '2' || arg.charAt(whilecounter) == '3' || arg.charAt(whilecounter) == '4' || arg.charAt(whilecounter) == '5' || arg.charAt(whilecounter) == '6' || 
           arg.charAt(whilecounter) == '7' || arg.charAt(whilecounter) == '8' || arg.charAt(whilecounter) == '9' || arg.charAt(whilecounter) == '0' || arg.charAt(whilecounter) == '.' || arg.charAt(whilecounter) == ' ' || 
           arg.charAt(whilecounter) == '(' || arg.charAt(whilecounter) == ')' || arg.charAt(whilecounter) == '*' || arg.charAt(whilecounter) == '+' || arg.charAt(whilecounter) == '/' || arg.charAt(whilecounter) == '-' || 
           isAlpha(arg.charAt(whilecounter)) || arg.charAt(whilecounter) == '_')
        {         

          expression.concat(arg.charAt(whilecounter));

        }else
        {
          errors++;
          errorMessage = "ERROR -14: INVALID CHARACTER AT FIELD " + String(contentField) + ".";
          return "";
        }

        if(whilecounter < arg.length())
        {
          whilecounter++;
        }else
        {
          break;
        }
      }


      if(expression.length() == 0)
      {
        errors++;
        errorMessage = "ERROR: -12";
        return "";
      }

      expression.trim();

      if(contentField == returnContentField)
      {
        int lastError = errors;
        mathsSolver(expression); //BE AWARE THIS CHECK COULD SLOW THINGS DOWN

        if(lastError != errors)
        {
          errors++;
          errorMessage = errorMessage + " ON FIELD " + String(contentField) + ".";
          return "";
        }

        return String(mathsSolver(expression));
      }

      //Serial.print("\n");
      //Serial.print(number);
      //Serial.print("\n");
      argStructureCounter++;
    }

    if(argStructure.charAt(argStructureCounter) == 'T') // TEXT
    {
      contentField++;
      String argString;
      while(arg.charAt(whilecounter) != '\"' && whilecounter < arg.length())
      {
        argString.concat(arg.charAt(whilecounter));
        whilecounter++;
      }

      argString.trim();
      if(contentField == returnContentField)
      {
        return argString;
      }

      argStructureCounter++;
    }

    if(argStructure.charAt(argStructureCounter) == 't') // CHAR
    {
      contentField++;
      String argPreChar;
      while(arg.charAt(whilecounter) != '\'' && whilecounter < arg.length())
      {
        argPreChar.concat(arg.charAt(whilecounter));
        whilecounter++;
      }

      argPreChar.trim();
      if(argPreChar.length() != 1)
      {
        errors++;
        errorMessage = "ERROR: -70";
        return "";
      }

      char argChar = argPreChar.charAt(0);

      if(contentField == returnContentField)
      {
        return String(argChar);
      }

      argStructureCounter++;
    }
  }

  return "\nUhh... This shouldn\'t have happened...";
}

static String tokens[100];
static uint8_t tokenTypes[100];

uint8_t tokenTreeSize;

bool isValidSymbol(char ch)
{
  char symbols[32] = {'['   ,     ']'     ,   '('    ,    ')'   ,   ';'    ,   '-'   ,    '='    ,   '_'   ,    '+'   ,   '{'   ,   '}'    ,   '\\'   ,   '/'   ,   '!'   ,   '@'  ,   '#'  ,   '$'   ,   '%'   ,   '^'   ,   '&'   ,   '*'   ,   ':'   ,   ','   ,   '>'    ,    '<'   ,   '\''   ,    '\"'   ,   '|'   ,  '?'    ,  '`'    ,  '~'    ,  '.'};

  for(int i = 0; i < 32; i++)
  {
    if(ch == symbols[i])
    {
      return true;
    }
  }
  return false;
}

uint8_t codeTokenizer(String code)
{
  for(int i = 0; i < tokenTreeSize; i++)
  {
    tokens[i] = "";
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
      tokens[tokenPtr] = String(code.charAt(codePtr));
      tokenPtr++;
      codePtr++;
    }else if(openQuotation == true)
    {
      while(code.charAt(codePtr) != '\"' && codePtr < code.length())
      {
        tokens[tokenPtr] += String(code.charAt(codePtr));
        codePtr++;
      }

      tokenTreeSize++;
      tokenPtr++;
    }else if(isValidSymbol(code.charAt(codePtr)))
    {

      tokenTreeSize++;
      tokens[tokenPtr] = String(code.charAt(codePtr));
      tokenPtr++;
      codePtr++;
      
    }else if(isAlpha(code.charAt(codePtr)) || isDigit(code.charAt(codePtr)) || code.charAt(codePtr) == '.' || code.charAt(codePtr) == '_')
    {
      while(isAlpha(code.charAt(codePtr)) || isDigit(code.charAt(codePtr)) || code.charAt(codePtr) == '.' || code.charAt(codePtr) == '_')
      {
        tokens[tokenPtr] += String(code.charAt(codePtr));
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

void clearVBuffer(uint8_t color)
{
  memset(vga_data_array, color, TXCOUNT);
}

char gridBuffer_char[40][60];
uint8_t gridBuffer_charColor[40][60];
uint16_t gridBuffer_backColor[40][60];


int char_grid_buffer_V_draw_offset = 0;


double PRINT(String code)
{
  code.trim();
  String arg;

  if(code.startsWith("PRINT"))
  {
    for(int i = 5; i < code.length() ; i++)
    {
      arg.concat(code.charAt(i));
    }

    arg.trim();

    int lastError = errors;

    String print = ARGUMENT_STRUCTURE_ANALYZER(arg, "PQTQp", 1);

    if(print.length() > 0)
    {
      //cursor(print, false);
      return 1;
    }

    print = ARGUMENT_STRUCTURE_ANALYZER(arg, "Pqtqp", 1);

    if(print.length() > 0)
    {
      //cursor(print, false);
      return 2;
    }

    print = ARGUMENT_STRUCTURE_ANALYZER(arg, "PNp", 1);

    if(print.length() > 0)
    {
      //cursor(print, false);
      return 2;
    }
  }

  return -1;
}

int variableFormatValidator(String varName)
{
  varName.trim();

  if(varName.length() > 8)
  {
    return -1;
  }

  bool letterFirst = false;

  for(int i = 0; i < varName.length(); i++)
  {
    if(isDigit(varName.charAt(i)) == false && isAlpha(varName.charAt(i)) == false && varName.charAt(i) != '_')
    {
      return -2;
    }

    if(isDigit(varName.charAt(i)) && letterFirst == false)
    {
      return -3;
    }

    if(isAlpha(varName.charAt(0)) || varName.charAt(0) == '_')
    {
      letterFirst = true;
    }
  }

  return 1;
}

int8_t varTypeLUT(String varType)
{
  String varTypes[11] = {"DOUBLE", "UINT64_T", "INT64_T", "UINT32_T", "INT32_T", "UINT16_T", "INT16_T", "UINT8_T", "INT8_T", "BOOL", "CHAR"};

  for(int i = 0; i < 11; i++)
  {
    if(varType == varTypes[i])
    {
      return i;
    }
  }

  return -1;
}

int8_t LEXER(String code)
{
  code.trim();
  //code.toUpperCase();
  codeTokenizer(code);

  if(tokens[0] == "graphics_mode_no_terminal" && tokenTreeSize == 1) // toggle graphics mode WITHOUT terminal
  {
    graphics_mode = true;
    terminal_on = false;
  }else if(tokens[0] == "draw_line" && tokenTreeSize == 7) // draw line
  {
    draw_line_half_res_buffer(tokens[1].toInt(), tokens[2].toInt(), tokens[3].toInt(), tokens[4].toInt(), tokens[5].toInt(), tokens[6].toInt());
  }else if(tokens[0] == "SD_INFO" && tokenTreeSize == 1) // show SD card info
  {
    FSInfo fs_info;
    SDFS.info(fs_info);

    echo_message("=>SD CARD INFO:");
    echo_message("    DETECTED:    " + String(sdInitialized));
    echo_message("    CARD SIZE:   " + String((float)SD.size64() / 1000));
    echo_message("    TOTAL KBYTES: " + String((float)fs_info.totalBytes / 1000));
    echo_message("    USED KBYTES:  " + String((float)fs_info.usedBytes / 1000));

  }else if(tokens[0] == "SD_FILES" && tokenTreeSize == 1) // show SD card files
  {
    File dir = SD.open("/");

    echo_message("=>SD CARD FILES:");

    int fileCounter = 0;

    while(true)
    {
      File entry = dir.openNextFile();

      if(entry == false)
      {
        break;
      }

      if(entry.isDirectory() == false)
      {
        echo_message("    " + String(fileCounter+1) + ". \"" + String(entry.name()) + "\"");
        fileCounter++;
      }

      entry.close();
    }

    dir.close();

  }else if(tokens[0] == "LOAD_RGB8_SD" && tokens[1] == "\"" && tokens[3] == "\"" && tokenTreeSize == 4) // load RGB8 from SD card
  {
    if(tokens[2].endsWith(".rgb8"))
    {
      File sdFile = SD.open(tokens[2]);
      int counter = 0;

      if(sdFile == true)
      {
        echo_message("=>LOADING RGB8 " + tokens[2] + " INTO HALF RES");
        echo_message("  BUFFER...");

        /*while (sdFile.available()) 
        {
          sdFile.seek(counter);
          uint8_t byte = sdFile.read();
          vga_data_array[counter] = byte;
          counter++;
        }
        while(true){}*/

        for(int i = 0; i < 240; i++)
        {
          for(int j = 0; j < 320; j++)
          {
            uint8_t byte;

            if(sdFile.available())
            {
              byte = sdFile.read(); 
            }else
            {
              echo_message("=>ERROR LOADING IMAGE. ");
              i = 240;
              j = 320;
            }

            //half_res_buffer[((i*320) + j)] = byte;
            
            if(i == 239 && j == 319)
            {
              terminal_on = false;
              graphics_mode = true;
              echo_message("=>LOADED. ");
            }
          }
        }

        
        
      }else 
      {
        echo_message("=>NO FILE NAMED " + tokens[2] + " FOUND IN SD.");
        echo_message("");
      }

      sdFile.close();

    }else
    {
      echo_message("=>NOT AN RGB8 FILE");
    }

  }else if(tokens[0] == "FILE_EDITOR" && tokenTreeSize == 1) // Saves text into SD card
  {
    terminal_aspects2.cur.locationX = 1;
    UI = 1;

  }else if(tokens[0] == "LOAD_PROG_SD" && tokens[1] == "\"" && tokens[3] == "\"" && tokenTreeSize == 5) // Loads .BIN file from SD into RAM
  {
    if(tokens[2].endsWith(".bin"))
    {
      File sdFile = SD.open(tokens[2]);
      int counter = 0;

      if(sdFile == true)
      {
        if(tokens[4] == "RAM")
        {
          runFromRam = true;

          load_bytecode(tokens[2]);
        }else if(tokens[4] == "SD")
        {
          runFromRam = false;
          
        }else
        {
          echo_message("=>INVALID ARGUMENT \"" + tokens[4] + "\"");
          echo_message("");
        }
        
      }else 
      {
        echo_message("=>NO FILE NAMED \"" + tokens[2] + "\" FOUND IN SD.");
        echo_message("");
      }

      sdFile.close();
    }else
    {
      echo_message("=>TARGET FILE MUST BE .bin");
      echo_message("");
    }

  }else if(tokens[0] == "RUN" && tokenTreeSize == 1) // Loads .BIN file from SD into RAM
  {
    run_bytecode();

  }else if(tokens[0] == "ASSEMBLE_PROG_SD" && tokens[1] == "\"" && tokens[3] == "\"" && tokenTreeSize == 5) // Assembles instruction text into .bin bytecode program
  {
    if(tokens[2].endsWith(".txt"))
    {
      File sdFile = SD.open(tokens[2]);

      if(sdFile == true)
      {

        if(tokens[4] == "INSTR_TXT")
        {

          compile_alpha_instructions(tokens[2]);
          echo_message("=>COMPILED \"" + tokens[2] + "\"");
          echo_message("");
          
        }else if(tokens[4] == "HIGH_LEVEL")
        {
          
        }else
        {
          echo_message("=>INVALID ARGUMENT \"" + tokens[4] + "\"");
          echo_message("");
        }
        
      }else 
      {
        echo_message("=>NO FILE NAMED \"" + tokens[2] + "\" FOUND IN SD.");
        echo_message("");
      }

      sdFile.close();
    }else
    {
      echo_message("=>TARGET FILE MUST BE .txt");
      echo_message("");
    }

  }else if(tokens[0] == "CLR" && tokenTreeSize == 1) // Clears terminal
  {
    memset(gridBuffer_char, ' ', sizeof(gridBuffer_char));
    terminal_aspects.cur.locationX = leftBorder;
    terminal_aspects.cur.locationY = 0;
    echo_message("====== UNNAMED RP2350 COMPUTER");
    echo_message("====== VARIABLE MEMORY: " + String(memory_size*4) + " B");
    echo_message("====== BYTECODE PROG MEMORY: " + String(program_mem_size) + " B");
    echo_message("");
  

  }
  

  return 1;
}