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

//===========================================================================//

static cmdBuffer_t cmdBuffer;
static cmdBuffer2_t cmd_T_Buf;
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

void init_GPU(void)
{
  uint8_t initBufStatus =0;
  
  GPIO_InitTypeDef GPIO_InitStruct;
  
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;     // Mode: output "Push-Pull"
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;     // Set speed
  GPIO_InitStruct.GPIO_Pin = GPU_BSY_PIN;
  GPIO_Init(GPIOA, &GPIO_InitStruct);        // Apply settings to port A
  
  
  //GPIO_InitStruct.GPIO_Pin = GPU_BSY_LED; // LED BSY PIN
  //GPIO_Init(GPIOC, &GPIO_InitStruct);        // Apply settings to port С
  
  //GPIO_SET_PIN(GPIOC, GPU_BSY_LED); // turn off
  
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
    print(T_INIT_BUF T_OK);
  } else {
    print(T_INIT_BUF T_FAIL);
    while(1);
  }
}

__noreturn __task void run_GPU(void)
{
  //uint8_t strSize =0;
  //uint8_t count = 0;
  
  uint8_t cmd = 0;
  uint8_t bsy =0;
  uint16_t avaliableData =0;
  
  for(;;) {
    
    avaliableData = pFuncAvaliableData();
    
#if 1   
    // this rules protect GPIO from togling everytime
    // and try to hold buffer always filled by some data
    if((avaliableData > CALC_MAX_FILL_SIZE /*CALC_BUF_FILL(MAX_FILL_BUF)*/) && (!bsy)) { // buffer is allmost full, and no bsy flag
      bsy = 1;
      GPIO_SET_PIN(GPIOA, GPU_BSY_PIN); // say to CPU: "I`m bsy, do not send the data!"
      //GPIO_RESET_PIN(GPIOC, GPU_BSY_LED);
    } else if((avaliableData < CALC_MIN_FILL_SIZE /*CALC_BUF_FILL(MIN_FILL_BUF)*/) && (bsy)) { // buffer is allmost empty, and bsy flag
      bsy = 0;
      GPIO_RESET_PIN(GPIOA, GPU_BSY_PIN); // say to CPU: "I`m free, now send the data!"
      //GPIO_SET_PIN(GPIOC, GPU_BSY_LED);
    }
#endif
    
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
        pFuncWaitCutBuf(cmd_T_Buf.data, 10);
        
        drawChar(cmd_T_Buf.par1, cmd_T_Buf.par2, cmd_T_Buf.par0, cmd_T_Buf.par3, cmd_T_Buf.par4, cmd_T_Buf.par5);
      } break;
      
      /* // fix this in future!
      case DRW_PRNT: {
      while(dataAvailable_USART1() <= 1);
      strSize = readData8_USART1();
      while(dataAvailable_USART1() <= strSize);
      
      for(count =0; count <= strSize; count++) {
      cmdBufferStr[count] = readData8_USART1();
      }
      
      print((const char*)cmdBufferStr);
      } break;
      */
      
      case DRW_PRNT_C: {
        pFuncWaitCutBuf(cmd_T_Buf.data, 1);
        
        printChar(cmd_T_Buf.par0);
      } break;
      
      case DRW_PRNT_POS_C: {
        pFuncWaitCutBuf(cmd_T_Buf.data, 5);
        
        printCharPos(cmd_T_Buf.par1, cmd_T_Buf.par2, cmd_T_Buf.par0);
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
        pFuncWaitCutBuf(cmd_T_Buf.data, 1);
        
        setTextSize(cmd_T_Buf.par0);
      } break;
      
      case SET_TXT_WRAP: {
        pFuncWaitCutBuf(cmd_T_Buf.data, 1);
        
        setTextWrap(cmd_T_Buf.par0);
      } break;
      
      case SET_TXT_437: {
        pFuncWaitCutBuf(cmd_T_Buf.data, 1);
        
        cp437(cmd_T_Buf.par0);
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
        pFuncWaitCutBuf(cmd_T_Buf.data, 1);
        
        tftSetRotation(cmd_T_Buf.par0);
      } break;
      
      case SET_SCRL_AREA: {
        pFuncWaitCutBuf(cmdBuffer.data, 4);
        
        tftSetScrollArea(cmdBuffer.par1, cmdBuffer.par2);
      } break;
      
      case WRT_CMD: {
        pFuncWaitCutBuf(cmd_T_Buf.data, 1);
        
        writeCommand(cmd_T_Buf.par0);
      } break;
      
      case WRT_DATA: {
        pFuncWaitCutBuf(cmd_T_Buf.data, 1);
        
        writeData(cmd_T_Buf.par0);
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
        pFuncWaitCutBuf(cmd_T_Buf.data, 1);
        
        tftSetSleep(cmd_T_Buf.par0);
      } break;
      
      case SET_IDLE: {
        pFuncWaitCutBuf(cmd_T_Buf.data, 1);
        
        tftSetIdleMode(cmd_T_Buf.par0);
      } break;
      
      case SET_BRIGHTNES: {
        pFuncWaitCutBuf(cmd_T_Buf.data, 1);
        
        tftSetDispBrightness(cmd_T_Buf.par0);
      } break;
      
      case SET_INVERTION: {
        pFuncWaitCutBuf(cmd_T_Buf.data, 1);
        
        tftSetInvertion(cmd_T_Buf.par0);
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
        pFuncWaitCutBuf(cmd_T_Buf.data, 5);
        
        tftScrollSmooth(cmd_T_Buf.par1, cmd_T_Buf.par2, cmd_T_Buf.par0);
      } break;
      
      case PSH_CR: {
        pFuncWaitCutBuf(cmdBuffer.data, 2);
        
        tftPushColor(cmdBuffer.par1);
      } break;
      
      // --------------- Tile/Sprite -------------- //
      
      case LDD_TLE_8: {
        // get size of file name, tileset width, ram tile number, tile number in tileset
        pFuncWaitCutBuf(cmdBuffer.data, 4);
        // get file name
        pFuncWaitCutBuf(cmdBufferStr, cmdBuffer.data[0]);
        
        SDLoadTileFromSet8x8(cmdBufferStr, cmdBuffer.data[1], cmdBuffer.data[2], cmdBuffer.data[3]);
        // memset(cmdBufferStr, 0x00, cmdBuffer.data[0]); // remove name
      } break;
      
      case LDD_TLES_8: {
        // get size of file name, tileset width, ram tile number, max number of tiles to load
        pFuncWaitCutBuf(cmdBuffer.data, 4);
        // get file name
        pFuncWaitCutBuf(cmdBufferStr, cmdBuffer.data[0]);
        
        SDLoadTileSet8x8(cmdBufferStr, cmdBuffer.data[1], cmdBuffer.data[2], cmdBuffer.data[3]);
        // memset(cmdBufferStr, 0x00, cmdBuffer.data[0]); // remove name
      } break;    
      
      case LDD_TLES_RG_8: {
        // get size of file name, tileset width, ram tile number,
        // base tile number in tileset, max number of tiles to load
        pFuncWaitCutBuf(cmdBuffer.data, 5);
        // get file name
        pFuncWaitCutBuf(cmdBufferStr, cmdBuffer.data[0]);
        
        SDLoadRegionOfTileSet8x8(cmdBufferStr, cmdBuffer.data[1], cmdBuffer.data[2], cmdBuffer.data[3], cmdBuffer.data[4]);
        // memset(cmdBufferStr, 0x00, cmdBuffer.data[0]); // remove name
      } break;
      
      case DRW_TLE_8_POS: {
        pFuncWaitCutBuf(cmd_T_Buf.data, 5);
        
        drawTile8x8(cmd_T_Buf.par1, cmd_T_Buf.par2, cmd_T_Buf.par0);
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
