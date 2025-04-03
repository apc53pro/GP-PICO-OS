#include <SPI.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/dma.h"
#include "pico/platform.h"
#include "globals.h"

#include "hsync.pio.h"
#include "vsync.pio.h"
#include "rgb.pio.h"
#include "mms.h"

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

      if(CHECK_VARIABLE_EXISTS(outStack[outStackPtr], "NULL") == -1 && alphaIncluded == true)
      {
        //Serial.print("\nVariable does not exist: ");
        
        //Serial.print(outStack[outStackPtr]);
        return -6;
      }

      if(alphaIncluded == true)
      {
        
        outStack[outStackPtr] = String(READ_VARIABLE(outStack[outStackPtr], -1));
        
      }else if(negateNum == true)
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

String tokens[100];
//uint8_t tokenTypes[100];

uint8_t tokenTreeSize;

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

  bool alphaIncluded = false;

  while(codePtr < code.length())
  {
    if(code.charAt(codePtr) == ' ')
    {
      //tokenPtr++;
      //tokenTreeSize++;
      codePtr++;
    }else if(code.charAt(codePtr) == '=' || code.charAt(codePtr) == '(' || code.charAt(codePtr) == ')' || code.charAt(codePtr) == '+' || code.charAt(codePtr) == '-' || code.charAt(codePtr) == '*' || code.charAt(codePtr) == '/' || code.charAt(codePtr) == ';')
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

  return 0;
}

String cursorSymbol = ">>";
int cursor(String string, bool changeCursorSymbol)
{
  if(changeCursorSymbol == false)
  {
    Serial.print("\n" + cursorSymbol + " " + string);
  }else
  {
    cursorSymbol = string;
  }
  return 1;
}

void drawPixel(uint16_t pixelX, uint16_t pixelY, uint8_t pixelColor)
{
  if(pixelY == 0)
  {pixelY = 1;}

  uint32_t pixelAddress = ((pixelY-1) * 640) + pixelX;

  vga_data_array[pixelAddress] = pixelColor;
}

void drawLetter(uint16_t baseX, uint16_t baseY, char letter, uint8_t color, uint8_t backColor, bool useBackColor)
{
  
  for(int i = 0; i < 16; i++)
  {
    for(int j = 0; j < 16; j++)
    {
      if(0b1000000000000000 & font[letter-32][i] << j)
      {
        
        drawPixel(baseX + j, (baseY + i), color);
        
      }else if(useBackColor == true)
      {
        drawPixel(baseX + j, (baseY + i), backColor);
      }
    }
  }
  
}

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
      cursor(print, false);
      return 1;
    }

    print = ARGUMENT_STRUCTURE_ANALYZER(arg, "Pqtqp", 1);

    if(print.length() > 0)
    {
      cursor(print, false);
      return 2;
    }

    print = ARGUMENT_STRUCTURE_ANALYZER(arg, "PNp", 1);

    if(print.length() > 0)
    {
      cursor(print, false);
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

    if(isAlpha(varName.charAt(i)) || varName.charAt(i) == '_')
    {
      letterFirst = true;
    }
  }

  return 1;
}

uint8_t varTypeLUT(String varType)
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

  if(varTypeLUT(tokens[0]) > -1 && tokens[2] == "=" && tokens[tokenTreeSize-1] == ";")
  {
    int varType = varTypeLUT(tokens[0]);

    String expr;

    for(int i = 3; i < tokenTreeSize-1; i++)
    {
      expr.concat(tokens[i]);
    }

    if(variableFormatValidator(tokens[1]) == 1)
    {
      
      if(CHECK_VARIABLE_EXISTS(tokens[1], "NULL") == -1)
      {
        if(varType == 0)
        { 
          WRITE_DOUBLE_INTERNAL(tokens[1], mathsSolver(expr));
        }else if(varType == 1)
        { 
          WRITE_UINT64_INTERNAL(tokens[1], mathsSolver(expr));
        }else if(varType == 2)
        { 
          WRITE_INT64_INTERNAL(tokens[1], mathsSolver(expr));
        }else if(varType == 3)
        { 
          WRITE_UINT32_INTERNAL(tokens[1], mathsSolver(expr));
        }else if(varType == 4)
        { 
          WRITE_INT32_INTERNAL(tokens[1], mathsSolver(expr));
        }else if(varType == 5)
        { 
          WRITE_UINT16_INTERNAL(tokens[1], mathsSolver(expr));
        }else if(varType == 6)
        { 
          WRITE_INT16_INTERNAL(tokens[1], mathsSolver(expr));
        }else if(varType == 7)
        { 
          WRITE_UINT8_INTERNAL(tokens[1], mathsSolver(expr));
        }else if(varType == 8)
        { 
          WRITE_INT8_INTERNAL(tokens[1], mathsSolver(expr));
        }else if(varType == 9)
        { 
          WRITE_BOOL_INTERNAL(tokens[1], mathsSolver(expr));
        }else if(varType == 10)
        {
          WRITE_CHAR_INTERNAL(tokens[1], mathsSolver(expr));
        }
        
        Serial.print("\nWrote variable " + tokens[1] + " with value " + READ_VARIABLE(tokens[1], -1) + ".");
      }
    }
  }

  //PRINT(code);

  return 1;
}

void setup() 
{

  SPI.begin();
  Serial.begin(115200);
  SPI.beginTransaction(SPISettings(20000000, MSBFIRST, SPI_MODE0));
  pinMode(20, OUTPUT);
  digitalWrite(20, HIGH);

  memset(vga_data_array, 0b11111111, TXCOUNT);
  PIO pio = pio0;
  uint hsync_offset = pio_add_program(pio, &hsync_program);
  uint vsync_offset = pio_add_program(pio, &vsync_program);
  uint rgb_offset = pio_add_program(pio, &rgb_program);

  // Manually select a few state machines from pio instance pio0.
  uint hsync_sm = 0;
  uint vsync_sm = 1;
  uint rgb_sm = 2;

  // Call the initialization functions that are defined within each PIO file.
  // Why not create these programs here? By putting the initialization function in
  // the pio file, then all information about how to use/setup that state machine
  // is consolidated in one place. Here in the C, we then just import and use it.
  hsync_program_init(pio, hsync_sm, hsync_offset, HSYNC);
  vsync_program_init(pio, vsync_sm, vsync_offset, VSYNC);
  rgb_program_init(pio, rgb_sm, rgb_offset, 8);


  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // ============================== PIO DMA Channels =================================================
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  // DMA channels - 0 sends color data, 1 reconfigures and restarts 0
  int rgb_chan_0 = dma_claim_unused_channel(true);
  int rgb_chan_1 = dma_claim_unused_channel(true);

  // Channel Zero (sends color data to PIO VGA machine)
  dma_channel_config c0 = dma_channel_get_default_config(rgb_chan_0);  // default configs
  channel_config_set_transfer_data_size(&c0, DMA_SIZE_8);              // 8-bit txfers
  channel_config_set_read_increment(&c0, true);                        // yes read incrementing
  channel_config_set_write_increment(&c0, false);                      // no write incrementing
  channel_config_set_dreq(&c0, DREQ_PIO0_TX2) ;                        // DREQ_PIO0_TX2 pacing (FIFO)
  channel_config_set_chain_to(&c0, rgb_chan_1);                        // chain to other channel

  dma_channel_configure(
      rgb_chan_0,                 // Channel to be configured
      &c0,                        // The configuration we just created
      &pio->txf[rgb_sm],          // write address (RGB PIO TX FIFO)
      &vga_data_array,            // The initial read address (pixel color array)
      TXCOUNT,                    // Number of transfers; in this case each is 1 byte.
      false                       // Don't start immediately.
  );

  // Channel One (reconfigures the first channel)
  dma_channel_config c1 = dma_channel_get_default_config(rgb_chan_1);   // default configs
  channel_config_set_transfer_data_size(&c1, DMA_SIZE_32);              // 32-bit txfers
  channel_config_set_read_increment(&c1, false);                        // no read incrementing
  channel_config_set_write_increment(&c1, false);                       // no write incrementing
  channel_config_set_chain_to(&c1, rgb_chan_0);                         // chain to other channel

  dma_channel_configure(
      rgb_chan_1,                         // Channel to be configured
      &c1,                                // The configuration we just created
      &dma_hw->ch[rgb_chan_0].read_addr,  // Write address (channel 0 read address)
      &address_pointer,                   // Read address (POINTER TO AN ADDRESS)
      1,                                  // Number of transfers, in this case each is 4 byte
      false                               // Don't start immediately.
  );

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  // Initialize PIO state machine counters. This passes the information to the state machines
  // that they retrieve in the first 'pull' instructions, before the .wrap_target directive
  // in the assembly. Each uses these values to initialize some counting registers.
  pio_sm_put_blocking(pio, hsync_sm, H_ACTIVE);
  pio_sm_put_blocking(pio, vsync_sm, V_ACTIVE);
  pio_sm_put_blocking(pio, rgb_sm, RGB_ACTIVE);


  // Start the two pio machine IN SYNC
  // Note that the RGB state machine is running at full speed,
  // so synchronization doesn't matter for that one. But, we'll
  // start them all simultaneously anyway.
  pio_enable_sm_mask_in_sync(pio, ((1u << hsync_sm) | (1u << vsync_sm) | (1u << rgb_sm)));

  // Start DMA channel 0. Once started, the contents of the pixel color array
  // will be continously DMA's to the PIO machines that are driving the screen.
  // To change the contents of the screen, we need only change the contents
  // of that array.
  dma_start_channel_mask((1u << rgb_chan_0));

  //WRSR(1); //FOR THE LOVE OF GOD REMEMBER TO WRITE 1 INSTEAD OF 0 OR ELSE THE RAM STATUS REGISTER WILL ACTIVATE THE RAM HOLD PIN FEATURE AND FUCK EVERYTHING MAKING FOR UNPREDICTABLE BEHAVIOUR
  //CLEAR_RAM();

  /*---------------------------------------------------TEST 1 TESTS IF DELETING DOUBLE ARRAYS WILL PLAY NICE-------------------------------------------------------
  {
    Serial.print("\nDouble Start Address: ");
    Serial.print(DOUBLE_START_ADDRESS);
    Serial.print("\nDouble End Address: ");
    Serial.print(DOUBLE_END_ADDRESS);


    ALLOC_DOUBLE_ARRAY(4, "FFFFFFFA");
    ALLOC_DOUBLE_ARRAY(2, "FFFFFFFB");
    ALLOC_DOUBLE_ARRAY(20, "FFFFFFFG");
    ALLOC_DOUBLE_ARRAY(25, "FFFFFFFV");
    //ALLOC_DOUBLE_ARRAY(2, "FFFFFFFC");
    //ALLOC_DOUBLE_ARRAY(2, "FFFFFFFD");

    WRITE_INT64_INTERNAL("CAAAAAAA", 266666);
    WRITE_INT64_INTERNAL("FAAAAAAA", 366666);

    for(int i = 0; i <= 120; i++)
    {
      Serial.print("\nValue at Address ");
      Serial.print(i);
      Serial.print(": ");
      Serial.print(INTERNAL_RAM[i]);
    }

    DELETE_ARRAY("FFFFFFFA");
    DELETE_ARRAY("FFFFFFFB");
    DELETE_ARRAY("FFFFFFFG");
    DELETE_ARRAY("FFFFFFFV");
    //DELETE_ARRAY("FFFFFFFC");
    //DELETE_ARRAY("FFFFFFFD");

    for(int i = 0; i <= 120; i++)
    {
      Serial.print("\nValue at Address ");
      Serial.print(i);
      Serial.print(": ");
      Serial.print(INTERNAL_RAM[i]);
    }

    Serial.print("\nDouble Start Address: ");
    Serial.print(DOUBLE_START_ADDRESS);
    Serial.print("\nDouble End Address: ");
    Serial.print(DOUBLE_END_ADDRESS);

    WRITE_DOUBLE_INTERNAL("AAAAAAAA", 666666.666);
    Serial.print("\n");
    Serial.print(READ_VARIABLE("AAAAAAAA"));

    WRITE_DOUBLE_INTERNAL("BAAAAAAA", 166666.666);
    Serial.print("\n");
    Serial.print(READ_VARIABLE("BAAAAAAA"));

    
    Serial.print("\n");
    Serial.print(READ_VARIABLE("CAAAAAAA"));

    Serial.print("\n");
    Serial.print(READ_VARIABLE("FAAAAAAA"));
    ---------------------------------------------------TEST 1 -------------------------------------------------------*/

    /*---------------------------------------------------TEST 2 TESTS IF DELETING DOUBLE VARIABLES WILL PLAY NICE WITH DELETING DOUBLE ARRAYS -------------------------------------------------------
    WRITE_DOUBLE_INTERNAL("AAAAAAAA", 666666.666);
    WRITE_DOUBLE_INTERNAL("XAAAAAAA", 666666.666);
    WRITE_DOUBLE_INTERNAL("TAAAAAAA", 666666.666);
    WRITE_DOUBLE_INTERNAL("VAAAAAAA", 666666.666);

    ALLOC_DOUBLE_ARRAY(4,  "FFFFFFFA");
    //ALLOC_DOUBLE_ARRAY(2,  "FFFFFFFB");
    //ALLOC_DOUBLE_ARRAY(20, "FFFFFFFG");
    ALLOC_DOUBLE_ARRAY(25, "FFFFFFFV");

    DELETE_VARIABLE("AAAAAAAA");
    DELETE_VARIABLE("XAAAAAAA");

    Serial.print("IF THIS IS THE LAST MESSAGE PRINTED, IT CRASHED.");

    DELETE_ARRAY("FFFFFFFA");
    DELETE_ARRAY("FFFFFFFB");
    DELETE_ARRAY("FFFFFFFG");
    DELETE_ARRAY("FFFFFFFV");

    Serial.print("\n");
    Serial.print(READ_VARIABLE("TAAAAAAA"));

    Serial.print("\n");
    Serial.print(READ_VARIABLE("VAAAAAAA"));
    ---------------------------------------------------TEST 2 -------------------------------------------------------*/

    /*WRITE_INT64_INTERNAL("AAAAAAAA", 666666);
    WRITE_INT64_INTERNAL("XAAAAAAA", 666666);

    ALLOC_INT64_ARRAY("AAAAAAAA", 2);
    ALLOC_INT64_ARRAY("BAAAAAAA", 2);

    WRITE_INT64_INTERNAL("BAAAAAAA", 666666);
    WRITE_INT64_INTERNAL("VAAAAAAA", 666666);

    for(int i = 0; i <= 140; i++)
    {
      Serial.print("\nValue at Address ");
      Serial.print(i);
      Serial.print(": ");
      Serial.print(INTERNAL_RAM[i]);
    }

    DELETE_ARRAY("AAAAAAAA");
    DELETE_ARRAY("BAAAAAAA");

    for(int i = 0; i <= 140; i++)
    {
      Serial.print("\nValue at Address ");
      Serial.print(i);
      Serial.print(": ");
      Serial.print(INTERNAL_RAM[i]);
    }

    //---------------------------------------------------TESTING DOUBLE
    Serial.print("DOUBLE: ");
    WRITE_DOUBLE_INTERNAL("0CCCCCCC", 101230);
    ALLOC_DOUBLE_ARRAY("0DDDDDDD", 1024);
    STORE_IN_DOUBLE("0DDDDDDD", 1000, 45000);
    STORE_IN_DOUBLE("0DDDDDDD", 1001, 25000);
    Serial.print(READ_VARIABLE("0DDDDDDD", 1000));
    Serial.print(" ");
    Serial.print(READ_VARIABLE("0DDDDDDD", 1001));
    Serial.print(" ");
    Serial.print(READ_VARIABLE("0DDDDDDD", 1000));
    Serial.print(" ");
    Serial.print(READ_VARIABLE("0CCCCCCC", -1));  

    Serial.print("\n");
    //---------------------------------------------------

    //---------------------------------------------------TESTING INT64
    Serial.print("INT64: ");
    WRITE_INT64_INTERNAL("1CCCCCCC", 101230);
    ALLOC_INT64_ARRAY("1DDDDDDD", 1024);
    STORE_IN_INT64("1DDDDDDD", 1000, 45000);
    STORE_IN_INT64("1DDDDDDD", 1001, 25000);
    Serial.print(READ_VARIABLE("1DDDDDDD", 1000));
    Serial.print(" ");
    Serial.print(READ_VARIABLE("1DDDDDDD", 1001));
    Serial.print(" ");
    Serial.print(READ_VARIABLE("1DDDDDDD", 1000));
    Serial.print(" ");
    Serial.print(READ_VARIABLE("1CCCCCCC", -1));  

    Serial.print("\n");
    //---------------------------------------------------

    //---------------------------------------------------TESTING UINT64
    Serial.print("UINT64: ");
    WRITE_UINT64_INTERNAL("2CCCCCCC", 85000);
    ALLOC_UINT64_ARRAY("2DDDDDDD", 1024);
    STORE_IN_UINT64("2DDDDDDD", 1000, 25000);
    STORE_IN_UINT64("2DDDDDDD", 1001, 45000);
    Serial.print(READ_VARIABLE("2DDDDDDD", 1000));
    Serial.print(" ");
    Serial.print(READ_VARIABLE("2DDDDDDD", 1001));
    Serial.print(" ");
    Serial.print(READ_VARIABLE("2DDDDDDD", 1000));
    Serial.print(" ");
    Serial.print(READ_VARIABLE("2CCCCCCC", -1));  

    Serial.print("\n");

    //---------------------------------------------------

    //---------------------------------------------------TESTING INT32
    Serial.print("INT32: ");
    WRITE_INT32_INTERNAL("3CCCCCCC", 85000);
    ALLOC_INT32_ARRAY("3DDDDDDD", 1024);
    STORE_IN_INT32("3DDDDDDD", 1000, 25000);
    STORE_IN_INT32("3DDDDDDD", 1001, 45000);
    Serial.print(READ_VARIABLE("3DDDDDDD", 1000));
    Serial.print(" ");
    Serial.print(READ_VARIABLE("3DDDDDDD", 1001));
    Serial.print(" ");
    Serial.print(READ_VARIABLE("3DDDDDDD", 1000));
    Serial.print(" ");
    Serial.print(READ_VARIABLE("3CCCCCCC", -1));  

    Serial.print("\n");

    //---------------------------------------------------

    //---------------------------------------------------TESTING UINT32
    Serial.print("UINT32: ");
    WRITE_UINT32_INTERNAL("4CCCCCCC", 85000);
    ALLOC_UINT32_ARRAY("4DDDDDDD", 1024);
    STORE_IN_UINT32("4DDDDDDD", 1000, 25000);
    STORE_IN_UINT32("4DDDDDDD", 1001, 45000);
    Serial.print(READ_VARIABLE("4DDDDDDD", 1000));
    Serial.print(" ");
    Serial.print(READ_VARIABLE("4DDDDDDD", 1001));
    Serial.print(" ");
    Serial.print(READ_VARIABLE("4DDDDDDD", 1000));
    Serial.print(" ");
    Serial.print(READ_VARIABLE("4CCCCCCC", -1));  

    Serial.print("\n");

    //---------------------------------------------------

    //---------------------------------------------------TESTING INT16
    Serial.print("INT16: ");
    WRITE_INT16_INTERNAL("5CCCCCCC", 30000);
    ALLOC_INT16_ARRAY("5DDDDDDD", 1024);
    STORE_IN_INT16("5DDDDDDD", 1000, 25000);
    STORE_IN_INT16("5DDDDDDD", 1001, 12000);
    Serial.print(READ_VARIABLE("5DDDDDDD", 1000));
    Serial.print(" ");
    Serial.print(READ_VARIABLE("5DDDDDDD", 1001));
    Serial.print(" ");
    Serial.print(READ_VARIABLE("5DDDDDDD", 1000));
    Serial.print(" ");
    Serial.print(READ_VARIABLE("5CCCCCCC", -1));  

    Serial.print("\n");

    //---------------------------------------------------

    //---------------------------------------------------TESTING UINT16
    Serial.print("UINT16: ");
    WRITE_UINT16_INTERNAL("6CCCCCCC", 30000);
    ALLOC_UINT16_ARRAY("6DDDDDDD", 1024);
    STORE_IN_UINT16("6DDDDDDD", 1000, 25000);
    STORE_IN_UINT16("6DDDDDDD", 1001, 12000);
    Serial.print(READ_VARIABLE("6DDDDDDD", 1000));
    Serial.print(" ");
    Serial.print(READ_VARIABLE("6DDDDDDD", 1001));
    Serial.print(" ");
    Serial.print(READ_VARIABLE("6DDDDDDD", 1000));
    Serial.print(" ");
    Serial.print(READ_VARIABLE("6CCCCCCC", -1));  

    Serial.print("\n");

  } //---------------------------------------------------
  */
  //---------------------------------------------------TESTING INT8
  Serial.print("INT8: ");
  WRITE_INT8_INTERNAL("7CCCCCCC", 120);
  ALLOC_INT8_ARRAY("7DDDDDDD", 1024);
  STORE_IN_INT8("7DDDDDDD", 1000, -100);
  STORE_IN_INT8("7DDDDDDD", 1001, 50);
  Serial.print(READ_VARIABLE("7DDDDDDD", 1000));
  Serial.print(" ");
  Serial.print(READ_VARIABLE("7DDDDDDD", 1001));
  Serial.print(" ");
  Serial.print(READ_VARIABLE("7DDDDDDD", 1000));
  Serial.print(" ");
  Serial.print(READ_VARIABLE("7CCCCCCC", -1));  

  Serial.print("\n");

  //---------------------------------------------------

  //---------------------------------------------------TESTING UINT8
  Serial.print("UINT8: ");
  WRITE_UINT8_INTERNAL("8CCCCCCC", 255);
  ALLOC_UINT8_ARRAY("8DDDDDDD", 1024);
  STORE_IN_UINT8("8DDDDDDD", 1000, 0);
  STORE_IN_UINT8("8DDDDDDD", 1001, 50);
  Serial.print(READ_VARIABLE("8DDDDDDD", 1000));
  Serial.print(" ");
  Serial.print(READ_VARIABLE("8DDDDDDD", 1001));
  Serial.print(" ");
  Serial.print(READ_VARIABLE("8DDDDDDD", 1000));
  Serial.print(" ");
  Serial.print(READ_VARIABLE("8CCCCCCC", -1));  

  Serial.print("\n");

  //---------------------------------------------------

  //---------------------------------------------------TESTING BOOL
  Serial.print("BOOL: ");
  WRITE_BOOL_INTERNAL("9CCCCCCC", true);
  ALLOC_BOOL_ARRAY("9DDDDDDD", 1024);
  STORE_IN_BOOL("9DDDDDDD", 1000, false);
  STORE_IN_BOOL("9DDDDDDD", 1001, true);
  Serial.print(READ_VARIABLE("9DDDDDDD", 1000));
  Serial.print(" ");
  Serial.print(READ_VARIABLE("9DDDDDDD", 1001));
  Serial.print(" ");
  Serial.print(READ_VARIABLE("9DDDDDDD", 1000));
  Serial.print(" ");
  Serial.print(READ_VARIABLE("9CCCCCCC", -1));  

  Serial.print("\n");

  //---------------------------------------------------

  //---------------------------------------------------TESTING CHAR
  Serial.print("CHAR: ");
  WRITE_CHAR_INTERNAL("10CCCCCC", 'A');
  ALLOC_CHAR_ARRAY("10DDDDDD", 1024);
  STORE_IN_CHAR("10DDDDDD", 1000, 'B');
  STORE_IN_CHAR("10DDDDDD", 1001, 'C');
  Serial.print(READ_VARIABLE("10DDDDDD", 1000));
  Serial.print(" ");                                                //FIX TRYING TO STORE A VARIABLE IN AN INEXISTANT ARRAY OF A SPECIFIC TYPE ACTUALLY STORING A VALUE IN AN ARRAY OF A DIFFERENT TYPE WITH THE SAME NAME IF IT EXISTS...
  Serial.print(READ_VARIABLE("10DDDDDD", 1001));                    
  Serial.print(" ");
  Serial.print(READ_VARIABLE("10DDDDDD", 1000));
  Serial.print(" ");
  Serial.print(READ_VARIABLE("10CCCCCC", -1));  

  Serial.print("\n");

  //---------------------------------------------------

  
}


bool run = false;
String ARG_STRUCTURE;
String returnType;
String returnContentField;

void loop() 
{
  line = Serial.readString();
  int X = 65;
  int x2 = 1;
  /*while(true)
  {
    //memset(vga_data_array, 255, TXCOUNT);
    drawLetter(x2*20, 200, X, 255, 0, true);
    //drawLetter(random(0, 639), random(0, 479), random(65, 90), 0, random(0, 255), true);

    //delay(10);
    X++;
    x2++;
    if(X == 91)
    {
      while(true){}
    }
  }*/

  if(line.length() > 0)
  {
    int ms = millis();
    LEXER(line);
    Serial.print("\nExec time: " + String(millis() - ms));
    /*if(line.startsWith("~1 "))
    {
      line.trim();
      line.remove(0, 3);
      
      ARG_STRUCTURE = line;
      Serial.print("\nCHANGED ARG_STRUCTURE to ");                                                   
      Serial.print(ARG_STRUCTURE);                                                                   
    }else if(line.startsWith("~3 "))
    {
      line.trim();
      line.remove(0, 3);
      
      returnContentField = line;
      Serial.print("\nCHANGED whichOne to ");
      Serial.print(returnContentField);
    }else
    {
      Serial.print("\n");
      Serial.print(ARGUMENT_STRUCTURE_ANALYZER(line, ARG_STRUCTURE, returnContentField.toInt()));
    }*/

    //Serial.print("\n " + String(mathsSolver(line)));

    /*codeTokenizer(line);
    for(int i = 0; i < tokenTreeSize; i++)
    {
      Serial.print("\nToken: \"" + tokens[i] + "\". Token Type: " + tokenTypes[i]);
    }*/

    //Serial.print("\nVARIABLE: " + String(variableFormatValidator(line)));

    //Serial.print(CHECK_VARIABLE_EXISTS(line, "NULL"));
  }
}
