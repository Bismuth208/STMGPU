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
#include <uart.h>
#include <memHelper.h>
#include <systicktimer.h>
#include <tone.h>
#include <raycast.h>

#include "gpuMain.h"
#include "sdLoader.h"
#include "tiles.h"
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

void setBusyStatus(uint8_t status)
{
  bsy = status;
  
  if(status) {
    GPIO_RESET_PIN(GPU_BSY_LED_PORT, GPU_BSY_LED_PIN);
    // say to CPU: "I`m bsy, do not send the data!"
    if(bsyState) {
      GPIO_SET_PIN(GPU_BSY_PORT, GPU_BSY_PIN);
    } else {
      sendData8_UART1(BSY_MSG_CODE_WAIT);
    }
    
  } else {
    GPIO_SET_PIN(GPU_BSY_LED_PORT, GPU_BSY_LED_PIN);
    // say to CPU: "I`m free, now send the data!"
    if(bsyState) {
      GPIO_RESET_PIN(GPU_BSY_PORT, GPU_BSY_PIN);
    } else {
      sendData8_UART1(BSY_MSG_CODE_READY);
    }
  }
}

__noreturn void run_GPU(void)
{
  uint16_t avaliableData =0;

  for(;;) {
    
    avaliableData = dataAvailable_UART1();
    
    // this rules protect GPIO from togling everytime or send message
    // and try to hold buffer always filled by some data  
    if((avaliableData > CALC_MAX_FILL_SIZE ) && (!bsy)) { // buffer is allmost full, and no bsy flag
      setBusyStatus(1);
    } else if((avaliableData < CALC_MIN_FILL_SIZE ) && (bsy)) { // buffer is allmost empty, and bsy flag
      setBusyStatus(0);
    }
    
    if(avaliableData) {
      
      switch(waitCutByte_UART1()) // read command
      {
      // ------------------ Base ------------------ //  
        
      case FLL_SCR: {
        fillScreen(waitCutWord_UART1());
      } break;
      
      case DRW_PIXEL: {
        waitCutpBuf_UART1(6);
        drawPixel(cmdBuffer.par1, cmdBuffer.par2, cmdBuffer.par3);
      } break;
      
      // ------------- Primitives/GFX ------------- //
      
      case FLL_RECT: {
        waitCutpBuf_UART1(10);
        fillRect(cmdBuffer.par1, cmdBuffer.par2, cmdBuffer.par3, cmdBuffer.par4, cmdBuffer.par5);
      } break;
      
      case DRW_RECT: {
        waitCutpBuf_UART1(10);
        drawRect(cmdBuffer.par1, cmdBuffer.par2, cmdBuffer.par3, cmdBuffer.par4, cmdBuffer.par5);
        //drawRect((gfx_t*)&cmdBuffer);
      } break;
      
      case DRW_ROUND_RECT: {
        waitCutpBuf_UART1(12);
        drawRoundRect(cmdBuffer.par1, cmdBuffer.par2, cmdBuffer.par3, cmdBuffer.par4, cmdBuffer.par5, cmdBuffer.par6);
      } break;
      
      case FLL_ROUND_RECT: {
        waitCutpBuf_UART1(12);
        fillRoundRect(cmdBuffer.par1, cmdBuffer.par2, cmdBuffer.par3, cmdBuffer.par4, cmdBuffer.par5, cmdBuffer.par6);
      } break;
      
      case DRW_LINE: {
        waitCutpBuf_UART1(10);
        drawLine(cmdBuffer.par1, cmdBuffer.par2, cmdBuffer.par3, cmdBuffer.par4, cmdBuffer.par5);
      } break;
      
      case DRW_V_LINE: {
        waitCutpBuf_UART1(8);
        drawFastVLine(cmdBuffer.par1, cmdBuffer.par2, cmdBuffer.par3, cmdBuffer.par4);
      } break;
      
      case DRW_H_LINE: {
        waitCutpBuf_UART1(8);
        drawFastHLine(cmdBuffer.par1, cmdBuffer.par2, cmdBuffer.par3, cmdBuffer.par4);
      } break;
      
      case DRW_CIRCLE: {
        waitCutpBuf_UART1(8);
        drawCircle(cmdBuffer.par1, cmdBuffer.par2, cmdBuffer.par3, cmdBuffer.par4);
      } break;
      
      case FLL_CIRCLE: {
        waitCutpBuf_UART1(8);
        fillCircle(cmdBuffer.par1, cmdBuffer.par2, cmdBuffer.par3, cmdBuffer.par4);
      } break;
      
      case DRW_TRINGLE: {
        waitCutpBuf_UART1(14);
        drawTriangle(cmdBuffer.par1, cmdBuffer.par2, cmdBuffer.par3,
                     cmdBuffer.par4, cmdBuffer.par5, cmdBuffer.par6,
                     cmdBuffer.par7);
      } break;
      
      case FLL_TRINGLE: {
        waitCutpBuf_UART1(14);
        fillTriangle(cmdBuffer.par1, cmdBuffer.par2, cmdBuffer.par3,
                     cmdBuffer.par4, cmdBuffer.par5, cmdBuffer.par6,
                     cmdBuffer.par7);
      } break;
      
      case GET_RESOLUTION: { // maybe only one command what send something to CPU
        cmdBuffer.par1 = height();
        cmdBuffer.par2 = width();
        sendArrData8_UART1(cmdBuffer.data, 4); // return screen size to CPU
      } break;
      
      // --------------- Font/Print --------------- //
      
      case DRW_CHAR: {
        waitCutpBuf_UART1(10);
        drawChar(cmdBuffer.par1, cmdBuffer.par2, cmdBuffer.par3, cmdBuffer.par4, cmdBuffer.data[8], cmdBuffer.data[9]);
      } break;
      
      case DRW_PRNT: {
        uint16_t strSize = waitCutByte_UART1();
        printStr(cmdBufferStr, strSize);
        memset(cmdBufferStr, 0x00, MAX_TEXT_SIZE);
      } break;
      
      case DRW_PRNT_C: {
        printChar(waitCutByte_UART1());
      } break;
      
      case DRW_PRNT_POS_C: {
        waitCutpBuf_UART1(5);
        printCharAt(cmdBuffer.par1, cmdBuffer.par2, cmdBuffer.data[4]);
      } break;
      
      case SET_CURSOR: {
        waitCutpBuf_UART1(4);
        setCursor(cmdBuffer.par1, cmdBuffer.par2);
      } break;
      
      case SET_TXT_CR: {
        setTextColor(waitCutWord_UART1());
      } break;
      
      case SET_TXT_CR_BG: {
        waitCutpBuf_UART1(4);
        setTextColorBG(cmdBuffer.par1, cmdBuffer.par2);
      } break;
      
      case SET_TXT_SIZE: {
        setTextSize(waitCutByte_UART1());
      } break;
      
      case SET_TXT_WRAP: {
        setTextWrap(waitCutByte_UART1());
      } break;
      
      case SET_TXT_437: {
        cp437(waitCutByte_UART1());
      } break;
      
      /*
      case SET_TXT_FONT: {
      setTextFont(waitCutByte_UART1());
      } break;
      */
      
      // ---------------- Low Level --------------- //
      
      case SET_BRGHTNS_F: {
        waitCutpBuf_UART1(3);
        setDispBrightnessFade(cmdBuffer.data[0], cmdBuffer.data[1], cmdBuffer.data[2]);
      };
      
      case SET_ADR_WIN: {
        waitCutpBuf_UART1(8);
        setAddrWindow(cmdBuffer.par1, cmdBuffer.par2, cmdBuffer.par3, cmdBuffer.par4);
      } break;
      
      case SET_ROTATION: {
        setRotation(waitCutByte_UART1());
      } break;
      
      case SET_SCRL_AREA: {
        waitCutpBuf_UART1(4);
        setScrollArea(cmdBuffer.par1, cmdBuffer.par2);
      } break;
      
      case WRT_CMD: {
        writeCommand(waitCutByte_UART1());
      } break;
      
      case WRT_DATA: {
        writeData(waitCutByte_UART1());
      } break;
      
      case WRT_DATA_U16: {
        writeWordData(waitCutWord_UART1());
      } break;
      
      case SET_V_SCRL_ADR: {
        scrollAddress(waitCutWord_UART1());
      } break;
      
      case SET_SLEEP: {
        setSleep(waitCutByte_UART1());
      } break;
      
      case SET_IDLE: {
        setIdleMode(waitCutByte_UART1());
      } break;
      
      case SET_BRIGHTNES: {
        setDispBrightness(waitCutByte_UART1());
      } break;
      
      case SET_INVERTION: {
        setInvertion(waitCutByte_UART1());
      } break;
      
      /*
      case SET_GAMMA: {
        setGamma(waitCutByte_UART1());
      } break;
      */
      
      case MAK_SCRL: {
        waitCutpBuf_UART1(4);
        scrollScreen(cmdBuffer.par1, cmdBuffer.par2);
      } break;
      
      case MAK_SCRL_SMTH: {
        waitCutpBuf_UART1(5);
        scrollScreenSmooth(cmdBuffer.par1, cmdBuffer.par2, cmdBuffer.data[5]);
      } break;
      
      case PSH_CR: {
        pushColor(waitCutWord_UART1());
      } break;
      
      // ------- BSY protect selection ------------ //
        
      case BSY_SELECT: {
        bsyState = waitCutByte_UART1();
      } break;
      
      // ------------------- Tile ----------------- //
      
      // ---- tile 8x8 ---- //
      
      case LDD_TLE_8: {
        // get size of file name, tileset width, ram tile number, tile number in tileset
        waitCutpBuf_UART1(4);
        // get file name
        waitCutBuf_UART1(cmdBufferStr, cmdBuffer.data[0]);
        
        setBusyStatus(1);
        SDLoadTile(cmdBufferStr, &cmdBuffer.data[1], TILE_8_BASE_SIZE, TILES_NUM_8x8);
        memset(cmdBufferStr, 0x00, MAX_TEXT_SIZE);
      } break;
      
      case LDD_TLES_8: {
        // get size of file name, tileset width, ram tile number,
        // base tile number in tileset, max number of tiles to load
        waitCutpBuf_UART1(5);
        // get file name
        waitCutBuf_UART1(cmdBufferStr, cmdBuffer.data[0]);
        
        setBusyStatus(1);
        SDLoadTileSet(cmdBufferStr, &cmdBuffer.data[1], TILE_8_BASE_SIZE, TILES_NUM_8x8);
        memset(cmdBufferStr, 0x00, MAX_TEXT_SIZE);
      } break;
      
      case DRW_TLE_8: {
        waitCutpBuf_UART1(5);
        drawTile8x8(cmdBuffer.data);
      } break;
      
      // ---- tile 16x16 ---- //
      
      case LDD_TLE_16: {
        // get size of file name, tileset width, ram tile number, tile number in tileset
        waitCutpBuf_UART1(4);
        // get file name
        waitCutBuf_UART1(cmdBufferStr, cmdBuffer.data[0]);
        
        setBusyStatus(1);
        SDLoadTile(cmdBufferStr, &cmdBuffer.data[1], TILE_16_BASE_SIZE, TILES_NUM_16x16);
        memset(cmdBufferStr, 0x00, MAX_TEXT_SIZE);
      } break;
      
      case LDD_TLES_16: {
        // get size of file name, tileset width, ram tile number,
        // base tile number in tileset, max number of tiles to load
        waitCutpBuf_UART1(5);
        // get file name
        waitCutBuf_UART1(cmdBufferStr, cmdBuffer.data[0]);
        
        setBusyStatus(1);
        SDLoadTileSet(cmdBufferStr, &cmdBuffer.data[1], TILE_16_BASE_SIZE, TILES_NUM_16x16);
        memset(cmdBufferStr, 0x00, MAX_TEXT_SIZE);
      } break;
      
      case DRW_TLE_16: {
        waitCutpBuf_UART1(5);
        drawTile16x16(cmdBuffer.data);
      } break;
      
      // ---- tile 32x32 ---- //
#ifdef STM32F10X_HD
      case LDD_TLE_32: {
        // get size of file name, tileset width, ram tile number, tile number in tileset
        waitCutpBuf_UART1(4);
        // get file name
        waitCutBuf_UART1(cmdBufferStr, cmdBuffer.data[0]);
        
        setBusyStatus(1);
        SDLoadTile(cmdBufferStr, &cmdBuffer.data[1], TILE_32_BASE_SIZE, TILES_NUM_32x32);
        memset(cmdBufferStr, 0x00, MAX_TEXT_SIZE);
      } break;
      
      case LDD_TLES_32: {
        // get size of file name, tileset width, ram tile number,
        // base tile number in tileset, max number of tiles to load
        waitCutpBuf_UART1(5);
        // get file name
        waitCutBuf_UART1(cmdBufferStr, cmdBuffer.data[0]);
        
        setBusyStatus(1);
        SDLoadTileSet(cmdBufferStr, &cmdBuffer.data[1], TILE_32_BASE_SIZE, TILES_NUM_32x32);
        memset(cmdBufferStr, 0x00, MAX_TEXT_SIZE);
      } break;
      
      case DRW_TLE_32: {
        waitCutpBuf_UART1(5);
        drawTile32x32(cmdBuffer.data);
      } break;
#endif /* STM32F10X_HD */
      
      case LDD_TLE_MAP: {
        // get file name and it`s size
        waitCutBuf_UART1(cmdBufferStr, waitCutByte_UART1());
        
        setBusyStatus(1);
        SDLoadTileMap(cmdBufferStr);
        memset(cmdBufferStr, 0x00, MAX_TEXT_SIZE);
      } break;
      
      case DRW_TLE_MAP: {
        drawBackgroundMap();
      } break;

      
      // ----------------- Sprite ----------------- //
      
      case SET_SPR_POS: {
        waitCutpBuf_UART1(6);
        setSpritePosition(cmdBuffer.par1, cmdBuffer.par2, cmdBuffer.par3);
      } break;
      
      case SET_SPR_TYPE: {
        waitCutpBuf_UART1(2);
        setSpriteType(cmdBuffer.data[0], cmdBuffer.data[1]);
      } break;
      
      case SET_SPR_VISBL: {
        waitCutpBuf_UART1(2);
        setSpriteVisible(cmdBuffer.data[0], cmdBuffer.data[1]);
      } break;
      
      case SET_SPR_TLE: {
        waitCutpBuf_UART1(5);
        setSpriteTiles(cmdBuffer.data);
      } break;
      
      case SET_SPR_AUT_R: {
        setSpritesAutoRedraw(waitCutByte_UART1());
      } break;
      
      case DRW_SPR: {
        drawSprite(waitCutByte_UART1());
      } break;
      
      case GET_SRP_COLISN: {
        waitCutpBuf_UART1(2);
        uint8_t state = getSpriteCollision(cmdBuffer.data[0], cmdBuffer.data[1]);
        sendData8_UART1(state);
      } break;
      
      // ----------------- SD card ---------------- //
      
      case LDD_USR_PAL: {
        // get file name and it`s size
        waitCutBuf_UART1(cmdBufferStr, waitCutByte_UART1());
        
        setBusyStatus(1);
        SDLoadPalette(cmdBufferStr);
        memset(cmdBufferStr, 0x00, MAX_TEXT_SIZE);
      } break;
      
      case DRW_BMP_FIL: {
        waitCutpBuf_UART1(5);
        // get file name
        waitCutBuf_UART1(cmdBufferStr, cmdBuffer.data[4]);
        
        setBusyStatus(1);
        SDPrintBMP( cmdBuffer.par1, cmdBuffer.par2, cmdBufferStr);
        memset(cmdBufferStr, 0x00, MAX_TEXT_SIZE);
        //memset_DMA1(cmdBufferStr, 0x00, MAX_TEXT_SIZE);
      } break;
      
      /*
      case LDD_SND_FIL: {
        waitCutBuf_UART1(cmdBufferStr, waitCutByte_UART1()); // get file name
        
        SDLoadSoundPattern(cmdBufferStr);
      } break;
      */      
      
      // ------------------ Sound ----------------- //
      
      case SND_PLAY_TONE: {
        waitCutpBuf_UART1(4);
        playNote_Sound(cmdBuffer.par1, cmdBuffer.par2);
      } break;
      
      /*
      case SND_PLAY_BUF: {
        playBuf_Sound(waitCutByte_UART1()); // play specified sound pattern
      } break;
      */
      
      // --------------- GUI commands -------------- //

      case SET_WND_CR: {
        waitCutpBuf_UART1(4);
        setColorWindowGUI(cmdBuffer.par1, cmdBuffer.par2);
      } break;
      
       case SET_WND_CR_TXT: {
        waitCutpBuf_UART1(4);
        setTextBGColorGUI(cmdBuffer.par1, cmdBuffer.par2);
      } break;
      
       case SET_WND_TXT_SZ: {
        setGUITextSize(waitCutByte_UART1());
      } break;
      
       case DRW_WND_AT: {
        waitCutpBuf_UART1(8);
        drawWindowGUI(cmdBuffer.par1, cmdBuffer.par2, cmdBuffer.par3, cmdBuffer.par4);
      } break;
      
       case DRW_WND_TXT: {
        waitCutpBuf_UART1(9);
        // get text for window
        waitCutBuf_UART1(cmdBufferStr, cmdBuffer.data[8]);
        
        drawTextWindowGUI(cmdBuffer.par1, cmdBuffer.par2, cmdBuffer.par3, cmdBuffer.par4, cmdBufferStr);
        memset(cmdBufferStr, 0x00, MAX_TEXT_SIZE);
      } break;
      
      
      
      // --------------- '3D' engine --------------- //
      case SET_BACKGRND: {
        waitCutpBuf_UART1(4);
        setFloorSkyColor(cmdBuffer.par1, cmdBuffer.par2);
      } break;
      
      case RENDER_MAP: {
        renderWalls();
      } break;
      
      case MOVE_CAMERA: {
        moveCamera(waitCutByte_UART1());
      } break;
      
      case SET_CAM_POS: {
        waitCutpBuf_UART1(6);
        setCameraPosition(cmdBuffer.par1, cmdBuffer.par2, cmdBuffer.par3);
      } break;
      
      case SET_RENDER_QA: {
        serRenderQuality(waitCutByte_UART1());
      } break;
      
      case SET_TEXTURE_MODE: {
        setTextureQuality(waitCutByte_UART1());
      } break;
      
      case SET_WALL_CLD: {
        setWallCollision(waitCutByte_UART1());
      } break;
      
      case GET_CAM_POS: {
        getCamPosition(cmdBuffer.data);
        sendArrData8_UART1(cmdBuffer.data, 6);
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
        fflush_UART1();
        // TO DO:
        // make sync;
        // buffer error alert
      } break;
      
      }
    }
  }
}
