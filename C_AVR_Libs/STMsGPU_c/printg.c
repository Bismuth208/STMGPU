#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#include "STMsGPU_c.h"

#define DEFAULT_FLOAT_PRECISION     4

// page 257

// copied from Arduino Print class
void printNumber(unsigned long n, uint8_t base);
void printFloat(double number, uint8_t digits);

// ------------------------------------ //

void printg(char *format, ...)
{
  va_list argList;
  va_start(argList, format);
  
  //char buf[20];
  char tmpChar; // determinate dot
  uint8_t precision =0;
  bool parseExtPar =false;
  
  for(char *pFormat = format; *pFormat; pFormat++) {
    
    if(*pFormat != '%') {
      gpuPrintChar(*pFormat);
      continue;
    }
    
    tmpChar = *(++pFormat); // get precision or param

    if(tmpChar == '.') {
      tmpChar = *++pFormat;
      if(tmpChar == '*') {
        precision = va_arg(argList, int);
      } else {
        precision = (tmpChar - 48); // max num 9 :(
      }
      tmpChar = *++pFormat; // get 'f' or 's' char
      parseExtPar = true; // set flag
    }
    
    switch(tmpChar)
    {
      case 'c': { // single char
        gpuPrintChar(va_arg(argList, char));
      } break;
      case 's': { // parse srting
        if(parseExtPar) {
          gpuPrintLen(va_arg(argList, char*), precision);
        } else {
          gpuPrint(va_arg(argList, char*));
        }
        parseExtPar = false; // clear flag
      }break;
      case 'p': { // parse string in located in PROGMEM
        gpuPrintPGR(va_arg(argList, char*));
      }break;
      case 'd': {
        printNumber(va_arg(argList, int), 10);
      } break;
      case 'l': {
        printNumber(va_arg(argList, long), 10);
      } break;
      case 'f': { // parse float and double
        if(parseExtPar) {
          printFloat(va_arg(argList, double), precision);
        } else {
          printFloat(va_arg(argList, double), DEFAULT_FLOAT_PRECISION);
        }
        parseExtPar = false;
      } break;
      /*
      case 'e': {
        gpuPrint(ftoa( buf, va_arg(argList, double)));
      } break;
       */
      //...
      default: gpuPrintChar(tmpChar); break;
    }
  }
  va_end(argList);
}

// ------------------------------------ //

void printNumber(unsigned long n, uint8_t base)
{
  char buf[8 * sizeof(long) + 1]; // Assumes 8-bit chars plus zero byte.
  char *str = &buf[sizeof(buf) - 1];
  
  *str = '\0';
  
  // prevent crash if called with base == 1
  if (base < 2) base = 10;
  
  do {
    char c = n % base;
    n /= base;
    
    *--str = c < 10 ? c + '0' : c + 'A' - 10;
  } while(n);
  
  gpuPrint(str);
}

void printFloat(double number, uint8_t digits)
{
  if (isnan(number)) return gpuPrint("nan");
  if (isinf(number)) return gpuPrint("inf");
  if (number > 4294967040.0) return gpuPrint ("ovf");  // constant determined empirically
  if (number <-4294967040.0) return gpuPrint ("ovf");  // constant determined empirically
  
  // Handle negative numbers
  if (number < 0.0) {
    gpuPrintChar('-');
    number = -number;
  }
  
  // Round correctly so that print(1.999, 2) prints as "2.00"
  double rounding = 0.5;
  for (uint8_t i=0; i<digits; ++i)
    rounding /= 10.0;
  
  number += rounding;
  
  // Extract the integer part of the number and print it
  unsigned long int_part = (unsigned long)number;
  double remainder = number - (double)int_part;
  printNumber(int_part, 10);
  
  // Print the decimal point, but only if there are digits beyond
  if (digits > 0) {
    gpuPrintChar('.');
  }
  
  // Extract digits from the remainder one at a time
  while (digits-- > 0) {
    remainder *= 10.0;
    unsigned int toPrint = (unsigned int)(remainder);
    printNumber(toPrint, 10);
    remainder -= toPrint;
  }
}

