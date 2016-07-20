#include <stdlib.h>
#include <string.h>

#include <systicktimer.h>

#include <uart.h>
#include <gpu_gfx.h>

#include "gpuTest.h"

#if 0
#define TFT_W 320
#define TFT_H 240
#else
#define TFT_W tftWidth()
#define TFT_H tftHeight()
#endif

#define TEST_SAMPLE_SIZE 2000

static uint32_t nextInt = 1;

void (*pArrExecGFXFunc[])(void) = {
    testDrawTiles,
    drawRamTileSet8x8
  };

uint32_t randNum(void)
{
  nextInt ^= nextInt >> 6;
  nextInt ^= nextInt << 11;
  nextInt ^= nextInt >> 15;
  return ( nextInt * 3538123 );
}

uint16_t getTemp(void)
{
  // The internal temperature has to be used
  // with the internal reference of 1.1V.
  // Channel 8 can not be selected with
  // the analogRead function yet.

  // Set the internal reference and mux.
  ADMUX = (_BV(REFS1) | _BV(REFS0) | _BV(MUX3));
  ADCSRA |= _BV(ADEN);  // enable the ADC

  //_delay_ms(20);            // wait for voltages to become stable.

  ADCSRA |= _BV(ADSC);  // Start the ADC

  // Detect end-of-conversion
  while (bit_is_set(ADCSRA,ADSC));

  // The returned temperature is in degrees Celcius.
  // Reading register "ADCW" takes care of how to read ADCL and ADCH.
  // The offset of 324.31 could be wrong. It is just an indication.
  //return ((ADCW - 327.31 ) / 1.22);
  return ADCW;
}

void initRand(void)
{
  nextInt += getTemp();  // yes, it real "random"!
}

void gpuLoadTiles(void)
{
  // load 90 tiles to GPU's RAM at 0 position from tileFileName,
  // located on SD card
  // 9 - is width of tileSet in tiles ( 9 tiles width == 72 pixels)
  SDLoadTileSet8x8((const char*)tileFileName, 9, 0, 90);
}

// --------------------------------------------------------- //

void testDrawTiles(void)
{
  uint16_t rndPosX, rndPosY;

   for (uint16_t i = 0; i < TEST_SAMPLE_SIZE; i++) {
    rndPosX = randNum() % TFT_W;
    rndPosY = randNum() % TFT_H;
    
    // draw random tile form 0 to 90, at random position
    drawTile8x8(rndPosX, rndPosY, randNum()%90);
  }
}

void drawRamTileSet8x8(void)
{
  int16_t posX, posY;
  uint8_t count =0;
  
  for(uint8_t countY =0; countY <10; countY++) {
    for(uint8_t countX =0; countX <9; countX++) {
      
      posX = (50 + ( countX * 8 ));
      posY = (50 + ( countY * 8 ));
      
      drawTile8x8(posX, posY, count);
      
      ++count;
    }
  }
}


// ---------------------------------------------------------- //
__attribute__ ((noreturn)) int main(void)
{
  initSysTickTimer(); //it`s enable timer0 on atmega328p;
  initRand();

  uartSetup(USART_BAUD_1M);
  sync_gpu();

  tftFillScreen(COLOR_BLACK);
  gpuLoadTiles();

  uint32_t timerCount =0;

  uint8_t count =0;
  uint8_t testsCount = (sizeof(pArrExecGFXFunc)/sizeof(pArrExecGFXFunc[0]));

  for(;;) {

    for (count = 0; count < testsCount; count++) {
      pArrExecGFXFunc[count]();

       _delayMS(1000);  // actual 500
      tftFillScreen(COLOR_BLACK);
    }
  }
}
