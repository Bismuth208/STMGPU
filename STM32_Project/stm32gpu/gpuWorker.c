/*
*
* STM32_GPU Project
* Creation start: 10.04.2016 20:21 (UTC+4)
*
* Created by: Antonov Alexandr (Bismuth208)
*
* For more info look read.me
*
*/

#include <gfx.h>
#include <gfxDMA.h>
#include <spi.h>
#include <usart.h>
#include <systicktimer.h>

#include "gpuWorker.h"
#include "sdWorker.h"
#include "gpuTiles.h"
#include "sprites.h"

//===========================================================================//

static cmdBuffer_t cmdBuffer;
static uint8_t cmdBufferStr[MAX_TEXT_SIZE];

// Actual numbers of commands: SERIAL_BUFFER_SIZE / 9
// if SERIAL_BUFFER_SIZE = 2048 then max commands = 227 (if avg cmd size is 9 bytes)
//===========================================================================//

// This pointers is for low level access to interfeces (USART, SPI, I2C or other)
// communication interface is selected by corresponing GPIO state (jumper; in future of course...)

void (*pFuncSendArrData8)(void *, uint32_t);
void (*pFuncSendData8)(uint8_t);
uint8_t (*pFuncReadData8)(void);
uint16_t (*pFuncAvaliableData)(void);
void (*pFuncWaitCutBuf)(void *, uint16_t);
void (*pFuncFlushBuf)(void);

// to reduce memory usage, malloc memory for buffer after select the interface
//uint8_t (*pFuncInitBuffer)(void*);
//===========================================================================//

// Make sync whith CPU by read from USART buffer 
// sync sequence: 0x42DD
void sync_CPU(void)
{
  print(T_WAIT_SYNC);
  
  bool syncEstablished = false;
  
  while(!syncEstablished) {
    if(pFuncAvaliableData() >= 2) {
      if(pFuncReadData8() == 0x42) {
        if(pFuncReadData8() == 0xDD) {
          
          pFuncFlushBuf();
          syncEstablished = true;
          pFuncSendData8(SYNC_OK);  // sequence right, answer to CPU
          
          print(T_OK T_TFT_SIZE);
          
          // return screen size to CPU
          cmdBuffer.par1 = width();
          cmdBuffer.par2 = height();
          
          pFuncSendArrData8(cmdBuffer.data, 4);
          
          print(T_OK T_GPU_START);
        }
      }
    }
  }
}

void init_GPU_GPIO(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;         // Mode: output "Push-Pull"
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;        // Set speed
  GPIO_InitStruct.GPIO_Pin = GPU_BSY_PIN;
  GPIO_Init(GPU_BSY_PORT, &GPIO_InitStruct);            // Apply settings
  
  
  GPIO_InitStruct.GPIO_Pin = GPU_BSY_LED_PIN;           // LED BSY PIN
  GPIO_Init(GPU_BSY_LED_PORT, &GPIO_InitStruct);        // Apply settings
  
  GPIO_SET_PIN(GPU_BSY_LED_PORT, GPU_BSY_LED_PIN);      // turn off
}

void init_GPU(void)
{
  uint8_t initBufStatus =0;
  
  init_GPU_GPIO();
  
  // setup access to low level interface
  print(T_SELECT_WAY);
  
  if(/* GPIO_WAY_SELECT_HI */ 1) { // set USART
    print(T_USART_WAY);
    
    pFuncSendData8 = sendData8_USART1;
    pFuncSendArrData8 = sendArrData8_USART1;
    pFuncReadData8 = readData8_USART1;
    pFuncAvaliableData = dataAvailable_USART1;
    pFuncWaitCutBuf = waitCutBuf_USART1;
    pFuncFlushBuf = fflush_USART1;
    
    initBufStatus = init_UART1(USART_BAUD_1M);
  } /* else {
    print(T_USER_WAY);
    
    pFuncSendData8 = sendData8_USER;
    pFuncSendArrData8 = sendArrData8_USER;
    pFuncReadData8 = readData8_USER;
    pFuncAvaliableData = dataAvailable_USER;
    pFuncWaitCutBuf = waitCutBuf_USER;
    pFuncFlushBuf = fflush_USER;
    
    initBufStatus = init_USER();
  } */
  
  if(initBufStatus) {
    print(T_INIT_BUF T_OK); // RAM for command buffer allocated
  } else {
    print(T_INIT_BUF T_FAIL);  // RAM for command buffer not allocated
    while(1);
  }
}

__noreturn __task void run_GPU(void)
{
  uint8_t cmd = 0;
  uint8_t bsy = 0;
  uint16_t avaliableData =0;
  
  for(;;) {
    
    avaliableData = pFuncAvaliableData();
    
    // this rules protect GPIO from togling everytime
    // and try to hold buffer always filled by some data
#if USE_BSY_PROTECTION   
    if((avaliableData > CALC_MAX_FILL_SIZE ) && (!bsy)) { // buffer is allmost full, and no bsy flag
      bsy = 1;
      GPIO_RESET_PIN(GPU_BSY_LED_PORT, GPU_BSY_LED_PIN);
      // say to CPU: "I`m bsy, do not send the data!"
#if USE_HARD_BSY
      GPIO_SET_PIN(GPU_BSY_PORT, GPU_BSY_PIN);
#else
      pFuncSendData8(BSY_MSG_CODE_WAIT);
#endif
    } else if((avaliableData < CALC_MIN_FILL_SIZE ) && (bsy)) { // buffer is allmost empty, and bsy flag
      bsy = 0;
      GPIO_SET_PIN(GPU_BSY_LED_PORT, GPU_BSY_LED_PIN);
      // say to CPU: "I`m free, now send the data!"
#if USE_HARD_BSY
      GPIO_RESET_PIN(GPU_BSY_PORT, GPU_BSY_PIN);
#else
      pFuncSendData8(BSY_MSG_CODE_READY);
#endif
    }
#endif /* USE_BSY_PROTECTION */
    
    if(avaliableData) {
      
      cmd = pFuncReadData8(); // read command
      
      switch(cmd)
      {
        // ------------------ Base ------------------ //  
        
      case FLL_SCR: {
        pFuncWaitCutBuf(cmdBuffer.data, 2);
        
        tftFillScreen(cmdBuffer.par1);
      } break;
      
      case DRW_PIXEL: {
        pFuncWaitCutBuf(cmdBuffer.data, 6);
        
        tftDrawPixel(cmdBuffer.par1, cmdBuffer.par2, cmdBuffer.par3);
      } break;
      
      // ------------- Primitives/GFX ------------- //
      
      case FLL_RECT: {
        pFuncWaitCutBuf(cmdBuffer.data, 10);
        
        tftFillRect(cmdBuffer.par1, cmdBuffer.par2, cmdBuffer.par3, cmdBuffer.par4, cmdBuffer.par5);
      } break;
      
      case DRW_RECT: {
        pFuncWaitCutBuf(cmdBuffer.data, 10);
        
        tftDrawRect(cmdBuffer.par1, cmdBuffer.par2, cmdBuffer.par3, cmdBuffer.par4, cmdBuffer.par5);
        //tftDrawRect((gfx_t*)&cmdBuffer);
      } break;
      
      case DRW_ROUND_RECT: {
        pFuncWaitCutBuf(cmdBuffer.data, 12);
        
        drawRoundRect(cmdBuffer.par1, cmdBuffer.par2, cmdBuffer.par3, cmdBuffer.par4, cmdBuffer.par5, cmdBuffer.par6);
      } break;
      
      case FLL_ROUND_RECT: {
        pFuncWaitCutBuf(cmdBuffer.data, 12);
        
        fillRoundRect(cmdBuffer.par1, cmdBuffer.par2, cmdBuffer.par3, cmdBuffer.par4, cmdBuffer.par5, cmdBuffer.par6);
      } break;
      
      case DRW_LINE: {
        pFuncWaitCutBuf(cmdBuffer.data, 10);
        
        tftDrawLine(cmdBuffer.par1, cmdBuffer.par2, cmdBuffer.par3, cmdBuffer.par4, cmdBuffer.par5);
      } break;
      
      case DRW_V_LINE: {
        pFuncWaitCutBuf(cmdBuffer.data, 8);
        
        tftDrawFastVLine(cmdBuffer.par1, cmdBuffer.par2, cmdBuffer.par3, cmdBuffer.par4);
      } break;
      
      case DRW_H_LINE: {
        pFuncWaitCutBuf(cmdBuffer.data, 8);
        
        tftDrawFastHLine(cmdBuffer.par1, cmdBuffer.par2, cmdBuffer.par3, cmdBuffer.par4);
      } break;
      
      case DRW_CIRCLE: {
        pFuncWaitCutBuf(cmdBuffer.data, 8);
        
        drawCircle(cmdBuffer.par1, cmdBuffer.par2, cmdBuffer.par3, cmdBuffer.par4);
      } break;
      
      case FLL_CIRCLE: {
        pFuncWaitCutBuf(cmdBuffer.data, 8);
        
        fillCircle(cmdBuffer.par1, cmdBuffer.par2, cmdBuffer.par3, cmdBuffer.par4);
      } break;
      
      case DRW_TRINGLE: {
        pFuncWaitCutBuf(cmdBuffer.data, 14);
        
        drawTriangle(cmdBuffer.par1, cmdBuffer.par2, cmdBuffer.par3,
                     cmdBuffer.par4, cmdBuffer.par5, cmdBuffer.par6,
                     cmdBuffer.par7);
      } break;
      
      case FLL_TRINGLE: {
        pFuncWaitCutBuf(cmdBuffer.data, 14);
        
        fillTriangle(cmdBuffer.par1, cmdBuffer.par2, cmdBuffer.par3,
                     cmdBuffer.par4, cmdBuffer.par5, cmdBuffer.par6,
                     cmdBuffer.par7);
      } break;
      
      case GET_RESOLUTION: { // maybe only one command what send something to CPU
        cmdBuffer.par1 = height();
        cmdBuffer.par2 = width();
        
        pFuncSendArrData8(cmdBuffer.data, 4); // return screen size to CPU
      } break;
      
      // --------------- Font/Print --------------- //
      
      case DRW_CHAR: {
        pFuncWaitCutBuf(cmdBuffer.data, 10);
        
        drawChar(cmdBuffer.par1, cmdBuffer.par2, cmdBuffer.data[8], cmdBuffer.par3, cmdBuffer.par4, cmdBuffer.data[9]);
      } break;
      
      case DRW_PRNT: {
        pFuncWaitCutBuf(cmdBuffer.data, 1);
        
        pFuncWaitCutBuf(cmdBufferStr, cmdBuffer.data[0]);
        
        print((const char*)cmdBufferStr);
      } break;
      
      case DRW_PRNT_C: {
        pFuncWaitCutBuf(cmdBuffer.data, 1);
        
        printChar(cmdBuffer.data[0]);
      } break;
      
      case DRW_PRNT_POS_C: {
        pFuncWaitCutBuf(cmdBuffer.data, 5);
        
        printCharPos(cmdBuffer.par1, cmdBuffer.par2, cmdBuffer.data[4]);
      } break;
      
      case SET_CURSOR: {
        pFuncWaitCutBuf(cmdBuffer.data, 4);
        
        setCursor(cmdBuffer.par1, cmdBuffer.par2);
      } break;
      
      case SET_TXT_CR: {
        pFuncWaitCutBuf(cmdBuffer.data, 2);
        
        setTextColor(cmdBuffer.par1);
      } break;
      
      case SET_TXT_CR_BG: {
        pFuncWaitCutBuf(cmdBuffer.data, 4);
        
        setTextColorBG(cmdBuffer.par1, cmdBuffer.par2);
      } break;
      
      case SET_TXT_SIZE: {
        pFuncWaitCutBuf(cmdBuffer.data, 1);
        
        setTextSize(cmdBuffer.data[0]);
      } break;
      
      case SET_TXT_WRAP: {
        pFuncWaitCutBuf(cmdBuffer.data, 1);
        
        setTextWrap(cmdBuffer.data[0]);
      } break;
      
      case SET_TXT_437: {
        pFuncWaitCutBuf(cmdBuffer.data, 1);
        
        cp437(cmdBuffer.data[0]);
      } break;
      
      /*
      case SET_TXT_FONT: {
      while(dataAvailable_USART1() <= 1);
      
      setTextFont(readData8_USART1());
      } break;
      */
      
      // ---------------- Low Level --------------- //
      
      case SET_ADR_WIN: {
        pFuncWaitCutBuf(cmdBuffer.data, 8);
        
        tftSetAddrWindow(cmdBuffer.par1, cmdBuffer.par2, cmdBuffer.par3, cmdBuffer.par4);
      } break;
      
      case SET_ROTATION: {
        pFuncWaitCutBuf(cmdBuffer.data, 1);
        
        tftSetRotation(cmdBuffer.data[0]);
      } break;
      
      case SET_SCRL_AREA: {
        pFuncWaitCutBuf(cmdBuffer.data, 4);
        
        tftSetScrollArea(cmdBuffer.par1, cmdBuffer.par2);
      } break;
      
      case WRT_CMD: {
        pFuncWaitCutBuf(cmdBuffer.data, 1);
        
        writeCommand(cmdBuffer.data[0]);
      } break;
      
      case WRT_DATA: {
        pFuncWaitCutBuf(cmdBuffer.data, 1);
        
        writeData(cmdBuffer.data[0]);
      } break;
      
      case WRT_DATA_U16: {
        pFuncWaitCutBuf(cmdBuffer.data, 2);
        
        writeWordData(cmdBuffer.par1);
      } break;
      
      case SET_V_SCRL_ADR: {
        pFuncWaitCutBuf(cmdBuffer.data, 2);
        
        tftScrollAddress(cmdBuffer.par1);
      } break;
      
      case SET_SLEEP: {
        pFuncWaitCutBuf(cmdBuffer.data, 1);
        
        tftSetSleep(cmdBuffer.data[0]);
      } break;
      
      case SET_IDLE: {
        pFuncWaitCutBuf(cmdBuffer.data, 1);
        
        tftSetIdleMode(cmdBuffer.data[0]);
      } break;
      
      case SET_BRIGHTNES: {
        pFuncWaitCutBuf(cmdBuffer.data, 1);
        
        tftSetDispBrightness(cmdBuffer.data[0]);
      } break;
      
      case SET_INVERTION: {
        pFuncWaitCutBuf(cmdBuffer.data, 1);
        
        tftSetInvertion(cmdBuffer.data[0]);
      } break;
      
      /*
      case SET_GAMMA: {
      while(dataAvailable_USART1() <= 1);
      
      setGamma(readData8_USART1());
      } break;
      */
      
      case MAK_SCRL: {
        pFuncWaitCutBuf(cmdBuffer.data, 4);
        
        tftScroll(cmdBuffer.par1, cmdBuffer.par2);
      } break;
      
      case MAK_SCRL_SMTH: {
        pFuncWaitCutBuf(cmdBuffer.data, 5);
        
        tftScrollSmooth(cmdBuffer.par1, cmdBuffer.par2, cmdBuffer.data[5]);
      } break;
      
      case PSH_CR: {
        pFuncWaitCutBuf(cmdBuffer.data, 2);
        
        tftPushColor(cmdBuffer.par1);
      } break;
      
      // ------------------- Tile ----------------- //
      
      case LDD_TLE_8: {
        // get size of file name, tileset width, ram tile number, tile number in tileset
        pFuncWaitCutBuf(cmdBuffer.data, 4);
        // get file name
        pFuncWaitCutBuf(cmdBufferStr, cmdBuffer.data[0]);
        
        SDLoadTileFromSet8x8(cmdBufferStr, cmdBuffer.data[1], cmdBuffer.data[2], cmdBuffer.data[3]);
        //SDLoadTileFromSet8x8(cmdBufferStr, (tileParam_t*)&cmdBuffer);
        // memset(cmdBufferStr, 0x00, cmdBuffer.data[0]); // remove name
      } break;
      
      case LDD_TLES_8: {
        // get size of file name, tileset width, ram tile number, max number of tiles to load
        pFuncWaitCutBuf(cmdBuffer.data, 4);
        // get file name
        pFuncWaitCutBuf(cmdBufferStr, cmdBuffer.data[0]);
        
        SDLoadTileSet8x8(cmdBufferStr, cmdBuffer.data[1], cmdBuffer.data[2], cmdBuffer.data[3]);
        //SDLoadTileSet8x8(cmdBufferStr, (tileParam_t*)&cmdBuffer);
        // memset(cmdBufferStr, 0x00, cmdBuffer.data[0]); // remove name
      } break;    
      
      case LDD_TLES_RG_8: {
        // get size of file name, tileset width, ram tile number,
        // base tile number in tileset, max number of tiles to load
        pFuncWaitCutBuf(cmdBuffer.data, 5);
        // get file name
        pFuncWaitCutBuf(cmdBufferStr, cmdBuffer.data[0]);
        
        SDLoadRegionOfTileSet8x8(cmdBufferStr, cmdBuffer.data[1], cmdBuffer.data[2], cmdBuffer.data[3], cmdBuffer.data[4]);
        //SDLoadRegionOfTileSet8x8(cmdBufferStr, (tileParam_t*)&cmdBuffer);
        // memset(cmdBufferStr, 0x00, cmdBuffer.data[0]); // remove name
      } break;
      
      case DRW_TLE_8_POS: {
        pFuncWaitCutBuf(cmdBuffer.data, 5);
        
        drawTile8x8(cmdBuffer.par1, cmdBuffer.par2, cmdBuffer.data[4]);
      } break;
      
      case LDD_TLE_MAP: {
        // get size of file name
        pFuncWaitCutBuf(cmdBuffer.data, 1);
        
        // get file name
        pFuncWaitCutBuf(cmdBufferStr, cmdBuffer.data[0]);
        
        SDLoadTileMap(cmdBufferStr);
      } break;
      
      case DRW_TLE_MAP: {
        drawBackgroundMap();
      } break;
      
      // ----------------- Sprite ----------------- //
      
      case SET_SPR_POS: {
        pFuncWaitCutBuf(cmdBuffer.data, 6);
        
        setSpritePosition(cmdBuffer.par1, cmdBuffer.par2, cmdBuffer.par3);
      } break;
      
      case SET_SPR_TYPE: {
        pFuncWaitCutBuf(cmdBuffer.data, 2);
        
        setSpriteType(cmdBuffer.data[0], cmdBuffer.data[1]);
      } break;
      
      case SET_SPR_VISBL: {
        pFuncWaitCutBuf(cmdBuffer.data, 2);
        
        setSpriteVisible(cmdBuffer.data[0], cmdBuffer.data[1]);
      } break;
      
      case SET_SPR_TLE: {
        pFuncWaitCutBuf(cmdBuffer.data, 5);
        
        setSpriteTiles(cmdBuffer.data[0], cmdBuffer.data[1], cmdBuffer.data[2], cmdBuffer.data[3], cmdBuffer.data[4]);
      } break;
      
      case SET_SPR_AUT_R: {
        pFuncWaitCutBuf(cmdBuffer.data, 1);
        
        setSpritesAutoRedraw(cmdBuffer.data[0]);
      } break;
      
      case DRW_SPR: {
        pFuncWaitCutBuf(cmdBuffer.data, 1);
        
        drawSprite(cmdBuffer.data[0]);
      } break;
      
      case GET_SRP_COLISN: {
        pFuncWaitCutBuf(cmdBuffer.data, 2);
        
        uint8_t state = getSpriteCollision(cmdBuffer.data[0], cmdBuffer.data[1]);
        
        pFuncSendData8(state);
      } break;
      
      
      case LDD_USR_PAL: {
        //pFuncWaitCutBuf(cmdBuffer.data, 4);
        
        // get file name
        //pFuncWaitCutBuf(cmdBufferStr, cmdBuffer.par1);
        
        //SDLoadPalette(cmdBufferStr, cmdBuffer.par2);
      } break;
      
      // -------------  TO DO: ---------- //
      /*
      *
      *
      *
      *
      *       MAKE SOME MAGIC
      *            HERE
      *
      *
      *
      */
      // -------------------------------- //
      
      default: {
        pFuncFlushBuf();
        // TO DO:
        // make sync;
        // buffer error alert
      } break;
      
      }
    }
  }
}
