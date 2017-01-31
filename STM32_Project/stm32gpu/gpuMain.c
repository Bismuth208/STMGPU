/*
*
* STM32_GPU Project
* Creation start: 10.04.2016 20:21 (UTC+4)
*
* Created by: Antonov Alexandr (Bismuth208)
*
*/

#include <string.h>
#include <stm32f10x.h>

#include <gfx.h>
#include <gfxDMA.h>
#include <usart.h>
#include <memHelper.h>

#include "gpuMain.h"
#include "sdLoader.h"
#include "gpuTiles.h"
#include "sprites.h"
#include "gui.h"

//===========================================================================//

// Actual numbers of commands: SERIAL_BUFFER_SIZE / 9
// if SERIAL_BUFFER_SIZE = 2048 then max commands = 227 (if avg cmd size is 9 bytes)
cmdBuffer_t cmdBuffer;
static uint8_t cmdBufferStr[MAX_TEXT_SIZE];

uint8_t bsy = 0;
uint8_t bsyState = 0; // 0 - use sofware, 1 - use hardware

//===========================================================================//

inline void setBusyStatus(uint8_t status)
{
  bsy = status;
  
  if(status) {
    GPIO_RESET_PIN(GPU_BSY_LED_PORT, GPU_BSY_LED_PIN);
    // say to CPU: "I`m bsy, do not send the data!"
    if(bsyState) {
      GPIO_SET_PIN(GPU_BSY_PORT, GPU_BSY_PIN);
    } else {
      sendData8_USART1(BSY_MSG_CODE_WAIT);
    }
    
  } else {
    GPIO_SET_PIN(GPU_BSY_LED_PORT, GPU_BSY_LED_PIN);
    // say to CPU: "I`m free, now send the data!"
    if(bsyState) {
      GPIO_RESET_PIN(GPU_BSY_PORT, GPU_BSY_PIN);
    } else {
      sendData8_USART1(BSY_MSG_CODE_READY);
    }
  }
}

__noreturn void run_GPU(void)
{
  uint16_t avaliableData =0;

  for(;;) {
    
    avaliableData = dataAvailable_USART1();
    
    // this rules protect GPIO from togling everytime or send message
    // and try to hold buffer always filled by some data  
    if((avaliableData > CALC_MAX_FILL_SIZE ) && (!bsy)) { // buffer is allmost full, and no bsy flag
      setBusyStatus(1);
    } else if((avaliableData < CALC_MIN_FILL_SIZE ) && (bsy)) { // buffer is allmost empty, and bsy flag
      setBusyStatus(0);
    }
    
    if(avaliableData) {
      
      switch(readData8_USART1()) // read command
      {
      // ------------------ Base ------------------ //  
        
      case FLL_SCR: {
        fillScreen(waitCutWord_USART1());
      } break;
      
      case DRW_PIXEL: {
        waitCutBuf_USART1(cmdBuffer.data, 6);
        
        drawPixel(cmdBuffer.par1, cmdBuffer.par2, cmdBuffer.par3);
      } break;
      
      // ------------- Primitives/GFX ------------- //
      
      case FLL_RECT: {
        waitCutBuf_USART1(cmdBuffer.data, 10);
        
        fillRect(cmdBuffer.par1, cmdBuffer.par2, cmdBuffer.par3, cmdBuffer.par4, cmdBuffer.par5);
      } break;
      
      case DRW_RECT: {
        waitCutBuf_USART1(cmdBuffer.data, 10);
        
        drawRect(cmdBuffer.par1, cmdBuffer.par2, cmdBuffer.par3, cmdBuffer.par4, cmdBuffer.par5);
        //drawRect((gfx_t*)&cmdBuffer);
      } break;
      
      case DRW_ROUND_RECT: {
        waitCutBuf_USART1(cmdBuffer.data, 12);
        
        drawRoundRect(cmdBuffer.par1, cmdBuffer.par2, cmdBuffer.par3, cmdBuffer.par4, cmdBuffer.par5, cmdBuffer.par6);
      } break;
      
      case FLL_ROUND_RECT: {
        waitCutBuf_USART1(cmdBuffer.data, 12);
        
        fillRoundRect(cmdBuffer.par1, cmdBuffer.par2, cmdBuffer.par3, cmdBuffer.par4, cmdBuffer.par5, cmdBuffer.par6);
      } break;
      
      case DRW_LINE: {
        waitCutBuf_USART1(cmdBuffer.data, 10);
        
        drawLine(cmdBuffer.par1, cmdBuffer.par2, cmdBuffer.par3, cmdBuffer.par4, cmdBuffer.par5);
      } break;
      
      case DRW_V_LINE: {
        waitCutBuf_USART1(cmdBuffer.data, 8);
        
        drawFastVLine(cmdBuffer.par1, cmdBuffer.par2, cmdBuffer.par3, cmdBuffer.par4);
      } break;
      
      case DRW_H_LINE: {
        waitCutBuf_USART1(cmdBuffer.data, 8);
        
        drawFastHLine(cmdBuffer.par1, cmdBuffer.par2, cmdBuffer.par3, cmdBuffer.par4);
      } break;
      
      case DRW_CIRCLE: {
        waitCutBuf_USART1(cmdBuffer.data, 8);
        
        drawCircle(cmdBuffer.par1, cmdBuffer.par2, cmdBuffer.par3, cmdBuffer.par4);
      } break;
      
      case FLL_CIRCLE: {
        waitCutBuf_USART1(cmdBuffer.data, 8);
        
        fillCircle(cmdBuffer.par1, cmdBuffer.par2, cmdBuffer.par3, cmdBuffer.par4);
      } break;
      
      case DRW_TRINGLE: {
        waitCutBuf_USART1(cmdBuffer.data, 14);
        
        drawTriangle(cmdBuffer.par1, cmdBuffer.par2, cmdBuffer.par3,
                     cmdBuffer.par4, cmdBuffer.par5, cmdBuffer.par6,
                     cmdBuffer.par7);
      } break;
      
      case FLL_TRINGLE: {
        waitCutBuf_USART1(cmdBuffer.data, 14);
        
        fillTriangle(cmdBuffer.par1, cmdBuffer.par2, cmdBuffer.par3,
                     cmdBuffer.par4, cmdBuffer.par5, cmdBuffer.par6,
                     cmdBuffer.par7);
      } break;
      
      case GET_RESOLUTION: { // maybe only one command what send something to CPU
        cmdBuffer.par1 = height();
        cmdBuffer.par2 = width();
        
        sendArrData8_USART1(cmdBuffer.data, 4); // return screen size to CPU
      } break;
      
      // --------------- Font/Print --------------- //
      
      case DRW_CHAR: {
        waitCutBuf_USART1(cmdBuffer.data, 10);
        
        drawChar(cmdBuffer.par1, cmdBuffer.par2, cmdBuffer.par3, cmdBuffer.par4, cmdBuffer.data[8], cmdBuffer.data[9]);
      } break;
      
      case DRW_PRNT: {
        uint16_t strSize = waitCutByte_USART1();
        printStr(cmdBufferStr, strSize);
        memset(cmdBufferStr, 0x00, MAX_TEXT_SIZE);
      } break;
      
      case DRW_PRNT_C: {
        printChar(waitCutByte_USART1());
      } break;
      
      case DRW_PRNT_POS_C: {
        waitCutBuf_USART1(cmdBuffer.data, 5);
        
        printCharAt(cmdBuffer.par1, cmdBuffer.par2, cmdBuffer.data[4]);
      } break;
      
      case SET_CURSOR: {
        waitCutBuf_USART1(cmdBuffer.data, 4);
        
        setCursor(cmdBuffer.par1, cmdBuffer.par2);
      } break;
      
      case SET_TXT_CR: {
        setTextColor(waitCutWord_USART1());
      } break;
      
      case SET_TXT_CR_BG: {
        waitCutBuf_USART1(cmdBuffer.data, 4);
        
        setTextColorBG(cmdBuffer.par1, cmdBuffer.par2);
      } break;
      
      case SET_TXT_SIZE: {
        setTextSize(waitCutByte_USART1());
      } break;
      
      case SET_TXT_WRAP: {
        setTextWrap(waitCutByte_USART1());
      } break;
      
      case SET_TXT_437: {
        cp437(waitCutByte_USART1());
      } break;
      
      /*
      case SET_TXT_FONT: {
      setTextFont(waitCutByte_USART1());
      } break;
      */
      
      // ---------------- Low Level --------------- //
      
      case SET_ADR_WIN: {
        waitCutBuf_USART1(cmdBuffer.data, 8);
        
        setAddrWindow(cmdBuffer.par1, cmdBuffer.par2, cmdBuffer.par3, cmdBuffer.par4);
      } break;
      
      case SET_ROTATION: {
        setRotation(waitCutByte_USART1());
      } break;
      
      case SET_SCRL_AREA: {
        waitCutBuf_USART1(cmdBuffer.data, 4);
        
        setScrollArea(cmdBuffer.par1, cmdBuffer.par2);
      } break;
      
      case WRT_CMD: {
        writeCommand(waitCutByte_USART1());
      } break;
      
      case WRT_DATA: {
        writeData(waitCutByte_USART1());
      } break;
      
      case WRT_DATA_U16: {
        writeWordData(waitCutWord_USART1());
      } break;
      
      case SET_V_SCRL_ADR: {
        scrollAddress(waitCutWord_USART1());
      } break;
      
      case SET_SLEEP: {
        setSleep(waitCutByte_USART1());
      } break;
      
      case SET_IDLE: {
        setIdleMode(waitCutByte_USART1());
      } break;
      
      case SET_BRIGHTNES: {
        setDispBrightness(waitCutByte_USART1());
      } break;
      
      case SET_INVERTION: {
        setInvertion(waitCutByte_USART1());
      } break;
      
      /*
      case SET_GAMMA: {
      while(dataAvailable_USART1() <= 1);
      
      setGamma(readData8_USART1());
      } break;
      */
      
      case MAK_SCRL: {
        waitCutBuf_USART1(cmdBuffer.data, 4);
        
        scrollScreen(cmdBuffer.par1, cmdBuffer.par2);
      } break;
      
      case MAK_SCRL_SMTH: {
        waitCutBuf_USART1(cmdBuffer.data, 5);
        
        scrollScreenSmooth(cmdBuffer.par1, cmdBuffer.par2, cmdBuffer.data[5]);
      } break;
      
      case PSH_CR: {
        pushColor(waitCutWord_USART1());
      } break;
      
      // ------- BSY protect selection ------------ //
        
      case BSY_SELECT: {
        bsyState = waitCutByte_USART1();
      } break;
      
      // ------------------- Tile ----------------- //
      
      // ---- tile 8x8 ---- //
      
      case LDD_TLE_8: {
        // get size of file name, tileset width, ram tile number, tile number in tileset
        waitCutBuf_USART1(cmdBuffer.data, 4);
        // get file name
        waitCutBuf_USART1(cmdBufferStr, cmdBuffer.data[0]);
        
        setBusyStatus(1);
        SDLoadTile8x8(cmdBufferStr, &cmdBuffer.data[1]);
        memset(cmdBufferStr, 0x00, MAX_TEXT_SIZE);
      } break;
      
      case LDD_TLES_8: {
        // get size of file name, tileset width, ram tile number,
        // base tile number in tileset, max number of tiles to load
        waitCutBuf_USART1(cmdBuffer.data, 5);
        // get file name
        waitCutBuf_USART1(cmdBufferStr, cmdBuffer.data[0]);
        
        setBusyStatus(1);
        SDLoadTileSet8x8(cmdBufferStr, &cmdBuffer.data[1]);
        memset(cmdBufferStr, 0x00, MAX_TEXT_SIZE);
      } break;
      
      case DRW_TLE_8: {
        waitCutBuf_USART1(cmdBuffer.data, 5);

        drawTile8x8(cmdBuffer.data);
      } break;
      
      // ---- tile 16x16 ---- //
      
      case LDD_TLE_16: {
        // get size of file name, tileset width, ram tile number, tile number in tileset
        waitCutBuf_USART1(cmdBuffer.data, 4);
        // get file name
        waitCutBuf_USART1(cmdBufferStr, cmdBuffer.data[0]);
        
        setBusyStatus(1);
        SDLoadTile16x16(cmdBufferStr, &cmdBuffer.data[1]);
        memset(cmdBufferStr, 0x00, MAX_TEXT_SIZE);
      } break;
      
      case LDD_TLES_16: {
        // get size of file name, tileset width, ram tile number,
        // base tile number in tileset, max number of tiles to load
        waitCutBuf_USART1(cmdBuffer.data, 5);
        // get file name
        waitCutBuf_USART1(cmdBufferStr, cmdBuffer.data[0]);
        
        setBusyStatus(1);
        SDLoadTileSet16x16(cmdBufferStr, &cmdBuffer.data[1]);
        memset(cmdBufferStr, 0x00, MAX_TEXT_SIZE);
      } break;
      
      case DRW_TLE_16: {
        waitCutBuf_USART1(cmdBuffer.data, 5);
        
        drawTile16x16(cmdBuffer.data);
      } break;
      
      // ---- tile 32x32 ---- //
#ifdef STM32F10X_HD
      case LDD_TLE_32: {
        // get size of file name, tileset width, ram tile number, tile number in tileset
        waitCutBuf_USART1(cmdBuffer.data, 4);
        // get file name
        waitCutBuf_USART1(cmdBufferStr, cmdBuffer.data[0]);
        
        setBusyStatus(1);
        SDLoadTile32x32(cmdBufferStr, &cmdBuffer.data[1]);
        memset(cmdBufferStr, 0x00, MAX_TEXT_SIZE);
      } break;
      
      case LDD_TLES_32: {
        // get size of file name, tileset width, ram tile number,
        // base tile number in tileset, max number of tiles to load
        waitCutBuf_USART1(cmdBuffer.data, 5);
        // get file name
        waitCutBuf_USART1(cmdBufferStr, cmdBuffer.data[0]);
        
        setBusyStatus(1);
        SDLoadTileSet32x32(cmdBufferStr, &cmdBuffer.data[1]);
        memset(cmdBufferStr, 0x00, MAX_TEXT_SIZE);
      } break;
#endif /* STM32F10X_HD */
      
      case LDD_TLE_MAP: {
        // get file name and it`s size
        waitCutBuf_USART1(cmdBufferStr, waitCutByte_USART1());
        
        setBusyStatus(1);
        SDLoadTileMap(cmdBufferStr);
        memset(cmdBufferStr, 0x00, MAX_TEXT_SIZE);
      } break;
      
      case DRW_TLE_MAP: {
        drawBackgroundMap();
      } break;

      
      // ----------------- Sprite ----------------- //
      
      case SET_SPR_POS: {
        waitCutBuf_USART1(cmdBuffer.data, 6);
        
        setSpritePosition(cmdBuffer.par1, cmdBuffer.par2, cmdBuffer.par3);
      } break;
      
      case SET_SPR_TYPE: {
        waitCutBuf_USART1(cmdBuffer.data, 2);
        
        setSpriteType(cmdBuffer.data[0], cmdBuffer.data[1]);
      } break;
      
      case SET_SPR_VISBL: {
        waitCutBuf_USART1(cmdBuffer.data, 2);
        
        setSpriteVisible(cmdBuffer.data[0], cmdBuffer.data[1]);
      } break;
      
      case SET_SPR_TLE: {
        waitCutBuf_USART1(cmdBuffer.data, 5);
        
        setSpriteTiles(cmdBuffer.data);
      } break;
      
      case SET_SPR_AUT_R: {
        setSpritesAutoRedraw(waitCutByte_USART1());
      } break;
      
      case DRW_SPR: {
        drawSprite(waitCutByte_USART1());
      } break;
      
      case GET_SRP_COLISN: {
        waitCutBuf_USART1(cmdBuffer.data, 2);
        
        uint8_t state = getSpriteCollision(cmdBuffer.data[0], cmdBuffer.data[1]);
        
        sendData8_USART1(state);
      } break;
      
      // ----------------- SD card ---------------- //
      
      case LDD_USR_PAL: {
        // get file name and it`s size
        waitCutBuf_USART1(cmdBufferStr, waitCutByte_USART1());
        
        setBusyStatus(1);
        SDLoadPalette(cmdBufferStr);
        memset(cmdBufferStr, 0x00, MAX_TEXT_SIZE);
      } break;
      
      case DRW_BMP_FIL: {
        waitCutBuf_USART1(cmdBuffer.data, 5);
        // get file name
        waitCutBuf_USART1(cmdBufferStr, cmdBuffer.data[4]);
        
        setBusyStatus(1);
        SDPrintBMP( cmdBuffer.par1, cmdBuffer.par2, cmdBufferStr);
        memset(cmdBufferStr, 0x00, MAX_TEXT_SIZE);
      } break;
      
      
      // --------------- GUI commands -------------- //

      case SET_WND_CR: {
        waitCutBuf_USART1(cmdBuffer.data, 4);
        
        setColorWindowGUI(cmdBuffer.par1, cmdBuffer.par2);
      } break;
      
       case SET_WND_CR_TXT: {
        waitCutBuf_USART1(cmdBuffer.data, 4);
        
        setTextBGColorGUI(cmdBuffer.par1, cmdBuffer.par2);
      } break;
      
       case SET_WND_TXT_SZ: {
        
        setGUITextSize(waitCutByte_USART1());
      } break;
      
       case DRW_WND_AT: {
        waitCutBuf_USART1(cmdBuffer.data, 8);
        
        drawWindowGUI(cmdBuffer.par1, cmdBuffer.par2, cmdBuffer.par3, cmdBuffer.par4);
      } break;
      
       case DRW_WND_TXT: {
        waitCutBuf_USART1(cmdBuffer.data, 9);
        // get text for window
        waitCutBuf_USART1(cmdBufferStr, cmdBuffer.data[8]);
        
        drawTextWindowGUI(cmdBuffer.par1, cmdBuffer.par2, cmdBuffer.par3, cmdBuffer.par4, cmdBufferStr);
        memset(cmdBufferStr, 0x00, MAX_TEXT_SIZE);
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
        fflush_USART1();
        // TO DO:
        // make sync;
        // buffer error alert
      } break;
      
      }
    }
  }
}
