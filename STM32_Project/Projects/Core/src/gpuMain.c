/*
 *
 * STM32_GPU Project
 * Creation start: 10.04.2016 20:21 (UTC+4)
 *
 * Created by: Antonov Alexandr (Bismuth208)
 *
 */

#include <string.h>

#ifdef STM32F40XX
#include <stm32f4xx.h>
#else
#include <stm32f10x.h>
#endif /* STM32F40XX */

#include <uart.h>
#include <memHelper.h>
#include <systicktimer.h>
#include <gfx.h>
#include <tone.h>
#include <raycast.h>

#include "gpuMain.h"
#include "sdLoader.h"
#include "tiles.h"
#include "sprites.h"
#include "gui.h"

//===========================================================================//

// Actual numbers of commands: SERIAL_BUFFER_SIZE / 9 (if avg cmd size is 9 bytes)
// if SERIAL_BUFFER_SIZE = 2048 then max commands = 227
cmdBuffer_t cmdBuffer;
static uint8_t cmdBufferStr[MAX_TEXT_SIZE];

uint8_t bsyGPUBufferFlag = 0;
uint8_t bsyInterfaceFlag = 0; // 0 - use software, 1 - use hardware

#ifdef USART1_USE_PROTOCOL_V1
__IO uint8_t ucResetDataBuff = 0;
#endif

//===========================================================================//
void setBusyStatus(uint8_t status)
{
  bsyGPUBufferFlag = status;

  if (status) {
    // say to CPU: "I`m bsy, do not send the data!"
    if (bsyInterfaceFlag) {
      GPIO_SetBits(GPU_BSY_PORT, GPU_BSY_PIN);
    } else {
      GPU_INTERFACE_SEND_DATA_8(BSY_MSG_CODE_WAIT);
    }
    GPIO_ResetBits(GPU_BSY_LED_PORT, GPU_BSY_LED_PIN);
  } else {
    // say to CPU: "I`m free, now send the data!"
    if (bsyInterfaceFlag) {
      GPIO_ResetBits(GPU_BSY_PORT, GPU_BSY_PIN);
    } else {
      GPU_INTERFACE_SEND_DATA_8(BSY_MSG_CODE_READY);
    }
    GPIO_SetBits(GPU_BSY_LED_PORT, GPU_BSY_LED_PIN);
  }
}

#ifdef USART1_USE_PROTOCOL_V1
void vCallbackFrom_USART1_IRQ(void)
{
  // say to CPU: "I`m bsy, do not send the data!"
  if(bsyInterfaceFlag) {
    GPIO_SetBits(GPU_BSY_PORT, GPU_BSY_PIN);
  } else {
    GPU_INTERFACE_SEND_DATA_8(BSY_MSG_CODE_WAIT);
  }
  GPIO_ResetBits(GPU_BSY_LED_PORT, GPU_BSY_LED_PIN);
  bsyGPUBufferFlag = 1;
  ucResetDataBuff = 1;
}
#endif

__attribute__((noreturn, optimize("O2"))) void run_GPU(void)
{
  uint32_t ulAvaliableData = 0;
//  cmdBuffer_t *pCMD = 0;

  for (;;) {
    ulAvaliableData = GPU_INTERFACE_GET_AVALIABLE_DATA();

#ifdef USART1_USE_PROTOCOL_V1
    if(avaliableData == 0) {
      if (ucResetDataBuff) {
        GPU_INTERFACE_FFLUSH();
        ucResetDataBuff = 0;
        setBusyStatus(0);
      }
    }
#endif

#if 1
#ifdef USART1_USE_PROTOCOL_V1
    if (!ucResetDataBuff)
#endif
    {
      // this rules protect GPIO from toggling every time or send message
      // and try to hold buffer always filled by some data
      if ((ulAvaliableData > CALC_MAX_FILL_SIZE) && (!bsyGPUBufferFlag)) { // buffer is allmost full, and no bsy flag
        setBusyStatus(1);
      } else if ((ulAvaliableData < CALC_MIN_FILL_SIZE) && (bsyGPUBufferFlag)) { // buffer is allmost empty, and bsy flag
        setBusyStatus(0);
      }
    }
#endif

    if (ulAvaliableData) {
#ifdef USART1_USE_PROTOCOL_V1
      switch(readData8_UART1()) // read command
#endif
#ifdef USART1_USE_PROTOCOL_V0
      switch (GPU_INTERFACE_GET_DATA_8())
      // read command
#endif
      {
#if 1
        // ------------------ Base ------------------ //
        case FLL_SCR: {
          fillScreen(GPU_INTERFACE_GET_DATA_16());
        }
        break;

        case DRW_PIXEL: {
          GPU_INTERFACE_GET_P_BUFFER_DATA(6);
          drawPixel(cmdBuffer.par1, cmdBuffer.par2, cmdBuffer.par3);
        }
        break;

          // ------------- Primitives/GFX ------------- //
        case FLL_RECT: {
          GPU_INTERFACE_GET_P_BUFFER_DATA(10);
          fillRect(cmdBuffer.par1, cmdBuffer.par2, cmdBuffer.par3,
              cmdBuffer.par4, cmdBuffer.par5);
        }
        break;

        case DRW_RECT: {
          GPU_INTERFACE_GET_P_BUFFER_DATA(10);
          drawRect(cmdBuffer.par1, cmdBuffer.par2, cmdBuffer.par3,
              cmdBuffer.par4, cmdBuffer.par5);
        }
        break;

        case DRW_ROUND_RECT: {
          GPU_INTERFACE_GET_P_BUFFER_DATA(12);
          drawRoundRect(cmdBuffer.par1, cmdBuffer.par2, cmdBuffer.par3,
              cmdBuffer.par4, cmdBuffer.par5, cmdBuffer.par6);
        }
        break;

        case FLL_ROUND_RECT: {
          GPU_INTERFACE_GET_P_BUFFER_DATA(12);
          fillRoundRect(cmdBuffer.par1, cmdBuffer.par2, cmdBuffer.par3,
              cmdBuffer.par4, cmdBuffer.par5, cmdBuffer.par6);
        }
        break;

        case DRW_LINE: {
          GPU_INTERFACE_GET_P_BUFFER_DATA(10);
          drawLine(cmdBuffer.par1, cmdBuffer.par2, cmdBuffer.par3,
              cmdBuffer.par4, cmdBuffer.par5);
        }
        break;

        case DRW_V_LINE: {
          GPU_INTERFACE_GET_P_BUFFER_DATA(8);
          drawFastVLine(cmdBuffer.par1, cmdBuffer.par2, cmdBuffer.par3,
              cmdBuffer.par4);
        }
        break;

        case DRW_H_LINE: {
          GPU_INTERFACE_GET_P_BUFFER_DATA(8);
          drawFastHLine(cmdBuffer.par1, cmdBuffer.par2, cmdBuffer.par3,
              cmdBuffer.par4);
        }
        break;

        case DRW_CIRCLE: {
          GPU_INTERFACE_GET_P_BUFFER_DATA(8);
          drawCircle(cmdBuffer.par1, cmdBuffer.par2, cmdBuffer.par3,
              cmdBuffer.par4);
        }
        break;

        case FLL_CIRCLE: {
          GPU_INTERFACE_GET_P_BUFFER_DATA(8);
          fillCircle(cmdBuffer.par1, cmdBuffer.par2, cmdBuffer.par3,
              cmdBuffer.par4);
        }
        break;

        case DRW_TRINGLE: {
          GPU_INTERFACE_GET_P_BUFFER_DATA(14);
          drawTriangle(cmdBuffer.par1, cmdBuffer.par2, cmdBuffer.par3,
              cmdBuffer.par4, cmdBuffer.par5, cmdBuffer.par6, cmdBuffer.par7);
        }
        break;

        case FLL_TRINGLE: {
          GPU_INTERFACE_GET_P_BUFFER_DATA(14);
          fillTriangle(cmdBuffer.par1, cmdBuffer.par2, cmdBuffer.par3,
              cmdBuffer.par4, cmdBuffer.par5, cmdBuffer.par6, cmdBuffer.par7);
        }
        break;

        case GET_RESOLUTION: { // maybe only one command what send something to CPU
          cmdBuffer.par1 = (uint16_t) _ulHeight;
          cmdBuffer.par2 = (uint16_t) _ulWidth;
          GPU_INTERFACE_SEND_ARR_DATA_8(cmdBuffer.data, 4); // return screen size to CPU
        }
        break;

          // --------------- Font/Print --------------- //
        case DRW_CHAR: {
          GPU_INTERFACE_GET_P_BUFFER_DATA(10);
          drawChar(cmdBuffer.par1, cmdBuffer.par2, cmdBuffer.par3,
              cmdBuffer.par4, cmdBuffer.data[8], cmdBuffer.data[9]);
        }
        break;

        case DRW_PRNT: {
          uint32_t ulStrSize = GPU_INTERFACE_GET_DATA_8();
          printStr(cmdBufferStr, ulStrSize);
          memset(cmdBufferStr, 0x00, MAX_TEXT_SIZE);
        }
        break;

        case DRW_PRNT_C: {
          printChar(GPU_INTERFACE_GET_DATA_8());
        }
        break;

        case DRW_PRNT_POS_C: {
          GPU_INTERFACE_GET_P_BUFFER_DATA(5);
          printCharAt(cmdBuffer.par1, cmdBuffer.par2, cmdBuffer.data[4]);
        }
        break;

        case SET_CURSOR: {
          GPU_INTERFACE_GET_P_BUFFER_DATA(4);
          setCursor(cmdBuffer.par1, cmdBuffer.par2);
        }
        break;

        case SET_TXT_CR: {
          setTextColor(GPU_INTERFACE_GET_DATA_16());
        }
        break;

        case SET_TXT_CR_BG: {
          GPU_INTERFACE_GET_P_BUFFER_DATA(4);
          setTextColorBG(cmdBuffer.par1, cmdBuffer.par2);
        }
        break;

        case SET_TXT_SIZE: {
          setTextSize(GPU_INTERFACE_GET_DATA_8());
        }
        break;

        case SET_TXT_WRAP: {
          setTextWrap(GPU_INTERFACE_GET_DATA_8());
        }
        break;

        case SET_TXT_437: {
          cp437(GPU_INTERFACE_GET_DATA_8());
        }
        break;

          /*
           case SET_TXT_FONT: {
           setTextFont(GPU_INTERFACE_GET_DATA_8());
           } break;
           */

          // ---------------- Low Level --------------- //
        case SET_BRGHTNS_F: {
          GPU_INTERFACE_GET_P_BUFFER_DATA(3);
          setDispBrightnessFade(cmdBuffer.data[0], cmdBuffer.data[1],
              cmdBuffer.data[2]);
        }
        break;

        case SET_ADR_WIN: {
          GPU_INTERFACE_GET_P_BUFFER_DATA(8);
          setAddrWindow(cmdBuffer.par1, cmdBuffer.par2, cmdBuffer.par3,
              cmdBuffer.par4);
        }
        break;

        case SET_ROTATION: {
          setRotation(GPU_INTERFACE_GET_DATA_8());
        }
        break;

        case SET_SCRL_AREA: {
          GPU_INTERFACE_GET_P_BUFFER_DATA(4);
          setScrollArea(cmdBuffer.par1, cmdBuffer.par2);
        }
        break;

        case WRT_CMD: {
          writeCommand(GPU_INTERFACE_GET_DATA_8());
        }
        break;

        case WRT_DATA: {
          writeData(GPU_INTERFACE_GET_DATA_8());
        }
        break;

        case WRT_DATA_U16: {
          writeWordData(GPU_INTERFACE_GET_DATA_16());
        }
        break;

        case SET_V_SCRL_ADR: {
          scrollAddress(GPU_INTERFACE_GET_DATA_16());
        }
        break;

        case SET_SLEEP: {
          setSleep(GPU_INTERFACE_GET_DATA_8());
        }
        break;

        case SET_IDLE: {
          setIdleMode(GPU_INTERFACE_GET_DATA_8());
        }
        break;

        case SET_BRIGHTNES: {
          setDispBrightness(GPU_INTERFACE_GET_DATA_8());
        }
        break;

        case SET_INVERTION: {
          setInvertion(GPU_INTERFACE_GET_DATA_8());
        }
        break;

          /*
           case SET_GAMMA: {
           setGamma(GPU_GET_DATA_8_INTERFACE());
           } break;
           */

        case MAK_SCRL: {
          GPU_INTERFACE_GET_P_BUFFER_DATA(4);
          scrollScreen(cmdBuffer.par1, cmdBuffer.par2);
        }
        break;

        case MAK_SCRL_SMTH: {
          GPU_INTERFACE_GET_P_BUFFER_DATA(5);
          scrollScreenSmooth(cmdBuffer.par1, cmdBuffer.par2, cmdBuffer.data[5]);
        }
        break;

        case PSH_CR: {
          pushColor(GPU_INTERFACE_GET_DATA_16());
        }
        break;

          // ------------------- Tile ----------------- //
          // ---- tile 8x8 ---- //

        case LDD_TLE_8: {
          // get size of file name, tileset width, ram tile number, tile number in tileset
          GPU_INTERFACE_GET_P_BUFFER_DATA(4);
          // get file name
          GPU_INTERFACE_GET_BUFFER_DATA(cmdBufferStr, cmdBuffer.data[0]);

          setBusyStatus(1);
          SDLoadTile(cmdBufferStr, &cmdBuffer.data[1], TILE_8_BASE_SIZE,
              TILES_NUM_8x8);
          memset(cmdBufferStr, 0x00, MAX_TEXT_SIZE);
        }
        break;

        case LDD_TLES_8: {
          // get size of file name, tileset width, ram tile number,
          // base tile number in tileset, max number of tiles to load
          GPU_INTERFACE_GET_P_BUFFER_DATA(5);
          // get file name
          GPU_INTERFACE_GET_BUFFER_DATA(cmdBufferStr, cmdBuffer.data[0]);

          setBusyStatus(1);
          SDLoadTileSet(cmdBufferStr, &cmdBuffer.data[1], TILE_8_BASE_SIZE,
              TILES_NUM_8x8);
          memset(cmdBufferStr, 0x00, MAX_TEXT_SIZE);
        }
        break;

        case DRW_TLE_8: {
          GPU_INTERFACE_GET_P_BUFFER_DATA(5);
          Tile_t xTile = { .ulPosX = cmdBuffer.par1, .ulPosY = cmdBuffer.par2, .ulIndex = cmdBuffer.par3 };
          drawTile8x8(&xTile);

//        pcmdBuffer = GPU_INTERFACE_GET_P_BUFFER_DATA(&cmdBuffer->data, 5);
//        drawTile8x8(pcmdBuffer->data);
        }
        break;

          // ---- tile 16x16 ---- //

        case LDD_TLE_16: {
          // get size of file name, tileset width, ram tile number, tile number in tileset
          GPU_INTERFACE_GET_P_BUFFER_DATA(4);
          // get file name
          GPU_INTERFACE_GET_BUFFER_DATA(cmdBufferStr, cmdBuffer.data[0]);

          setBusyStatus(1);
          SDLoadTile(cmdBufferStr, &cmdBuffer.data[1], TILE_16_BASE_SIZE,
              TILES_NUM_16x16);
          memset(cmdBufferStr, 0x00, MAX_TEXT_SIZE);
        }
        break;

        case LDD_TLES_16: {
          // get size of file name, tileset width, ram tile number,
          // base tile number in tileset, max number of tiles to load
          GPU_INTERFACE_GET_P_BUFFER_DATA(5);
          // get file name
          GPU_INTERFACE_GET_BUFFER_DATA(cmdBufferStr, cmdBuffer.data[0]);

          setBusyStatus(1);
          SDLoadTileSet(cmdBufferStr, &cmdBuffer.data[1], TILE_16_BASE_SIZE,
              TILES_NUM_16x16);
          memset(cmdBufferStr, 0x00, MAX_TEXT_SIZE);
        }
        break;

        case DRW_TLE_16: {
          GPU_INTERFACE_GET_P_BUFFER_DATA(5);
          Tile_t xTile = { .ulPosX = cmdBuffer.par1, .ulPosY = cmdBuffer.par2, .ulIndex = cmdBuffer.par3 };
          drawTile16x16(&xTile);
        }
        break;

          // ---- tile 32x32 ---- //
#ifdef STM32F40XX // if STM32F401CCU6 64k RAM
        case LDD_TLE_32: {
          // get size of file name, tileset width, ram tile number, tile number in tileset
          GPU_INTERFACE_GET_P_BUFFER_DATA(4);
          // get file name
          GPU_INTERFACE_GET_BUFFER_DATA(cmdBufferStr, cmdBuffer.data[0]);

          setBusyStatus(1);
          SDLoadTile(cmdBufferStr, &cmdBuffer.data[1], TILE_32_BASE_SIZE,
              TILES_NUM_32x32);
          memset(cmdBufferStr, 0x00, MAX_TEXT_SIZE);
        }
        break;

        case LDD_TLES_32: {
          // get size of file name, tileset width, ram tile number,
          // base tile number in tileset, max number of tiles to load
          GPU_INTERFACE_GET_P_BUFFER_DATA(5);
          // get file name
          GPU_INTERFACE_GET_BUFFER_DATA(cmdBufferStr, cmdBuffer.data[0]);

          setBusyStatus(1);
          SDLoadTileSet(cmdBufferStr, &cmdBuffer.data[1], TILE_32_BASE_SIZE,
              TILES_NUM_32x32);
          memset(cmdBufferStr, 0x00, MAX_TEXT_SIZE);
        }
        break;

        case DRW_TLE_32: {
          GPU_INTERFACE_GET_P_BUFFER_DATA(5);
          Tile_t xTile = { .ulPosX = cmdBuffer.par1, .ulPosY = cmdBuffer.par2, .ulIndex = cmdBuffer.par3 };
          drawTile32x32(&xTile);
        }
        break;
#endif /* STM32F401CCU6 */

        case LDD_TLE_MAP: {
          // get file name and it`s size
          GPU_INTERFACE_GET_BUFFER_DATA(cmdBufferStr,
              GPU_INTERFACE_GET_DATA_8());

          setBusyStatus(1);
          SDLoadTileMap(cmdBufferStr);
          memset(cmdBufferStr, 0x00, MAX_TEXT_SIZE);
        }
        break;

        case DRW_TLE_MAP: {
          drawBackgroundMap();
        }
        break;

          // ----------------- Sprite ----------------- //
        case SET_SPR_POS: {
          GPU_INTERFACE_GET_P_BUFFER_DATA(6);
          setSpritePosition(cmdBuffer.par1, cmdBuffer.par2, cmdBuffer.par3);
        }
        break;

        case SET_SPR_TYPE: {
          GPU_INTERFACE_GET_P_BUFFER_DATA(2);
          setSpriteType(cmdBuffer.data[0], cmdBuffer.data[1]);
        }
        break;

        case SET_SPR_VISBL: {
          GPU_INTERFACE_GET_P_BUFFER_DATA(2);
          setSpriteVisible(cmdBuffer.data[0], cmdBuffer.data[1]);
        }
        break;

        case SET_SPR_TLE: {
          GPU_INTERFACE_GET_P_BUFFER_DATA(5);
          setSpriteTiles(cmdBuffer.data);
        }
        break;

        case SET_SPR_AUT_R: {
          setSpritesAutoRedraw(GPU_INTERFACE_GET_DATA_8());
        }
        break;

        case DRW_SPR: {
          drawSprite(GPU_INTERFACE_GET_DATA_8());
        }
        break;

        case GET_SRP_COLISN: {
          GPU_INTERFACE_GET_P_BUFFER_DATA(2);
          uint8_t state = getSpriteCollision(cmdBuffer.data[0],
              cmdBuffer.data[1]);
          GPU_INTERFACE_SEND_DATA_8(state);
        }
        break;

          // ----------------- SD card ---------------- //
        case LDD_USR_PAL: {
          // get file name and it`s size
          GPU_INTERFACE_GET_BUFFER_DATA(cmdBufferStr,
              GPU_INTERFACE_GET_DATA_8());

          setBusyStatus(1);
          SDLoadPalette(cmdBufferStr);
          memset(cmdBufferStr, 0x00, MAX_TEXT_SIZE);
        }
        break;

        case DRW_BMP_FIL: {
          GPU_INTERFACE_GET_P_BUFFER_DATA(5);
          // get file name
          GPU_INTERFACE_GET_BUFFER_DATA(cmdBufferStr, cmdBuffer.data[4]);

          setBusyStatus(1);
          SDPrintBMP(cmdBuffer.par1, cmdBuffer.par2, cmdBufferStr);
          memset(cmdBufferStr, 0x00, MAX_TEXT_SIZE);
          //memset_DMA1(cmdBufferStr, 0x00, MAX_TEXT_SIZE);
        }
        break;

          /*
           case LDD_SND_FIL: {
           // get file name
           GPU_INTERFACE_GET_BUFFER_DATA(cmdBufferStr, GPU_INTERFACE_GET_DATA_8());
           SDLoadSoundPattern(cmdBufferStr);
           } break;
           */

          // ------------------ Sound ----------------- //
        case SND_PLAY_TONE: {
          GPU_INTERFACE_GET_P_BUFFER_DATA(4);
          playNote_Sound(cmdBuffer.par1, cmdBuffer.par2);
        }
        break;

          /*
           case SND_PLAY_BUF: {
           playBuf_Sound(GPU_GET_DATA_8_INTERFACE()); // play specified sound pattern
           } break;
           */

          // --------------- GUI commands -------------- //
        case SET_WND_CR: {
          GPU_INTERFACE_GET_P_BUFFER_DATA(4);
          setColorWindowGUI(cmdBuffer.par1, cmdBuffer.par2);
        }
        break;

        case SET_WND_CR_TXT: {
          GPU_INTERFACE_GET_P_BUFFER_DATA(4);
          setTextBGColorGUI(cmdBuffer.par1, cmdBuffer.par2);
        }
        break;

        case SET_WND_TXT_SZ: {
          setGUITextSize(GPU_INTERFACE_GET_DATA_8());
        }
        break;

        case DRW_WND_AT: {
          GPU_INTERFACE_GET_P_BUFFER_DATA(8);
          drawWindowGUI(cmdBuffer.par1, cmdBuffer.par2, cmdBuffer.par3,
              cmdBuffer.par4);
        }
        break;

        case DRW_WND_TXT: {
          GPU_INTERFACE_GET_P_BUFFER_DATA(9);
          // get text for window
          GPU_INTERFACE_GET_BUFFER_DATA(cmdBufferStr, cmdBuffer.data[8]);

          drawTextWindowGUI(cmdBuffer.par1, cmdBuffer.par2, cmdBuffer.par3,
              cmdBuffer.par4, cmdBufferStr);
          memset(cmdBufferStr, 0x00, MAX_TEXT_SIZE);
        }
        break;

          // --------------- '3D' engine --------------- //
        case SET_BACKGRND: {
          GPU_INTERFACE_GET_P_BUFFER_DATA(4);
          setFloorSkyColor(cmdBuffer.par1, cmdBuffer.par2);
        }
        break;

        case RENDER_MAP: {
          renderWalls();
        }
        break;

        case MOVE_CAMERA: {
          moveCamera(GPU_INTERFACE_GET_DATA_8());
        }
        break;

        case SET_CAM_POS: {
          GPU_INTERFACE_GET_P_BUFFER_DATA(6);
          setCameraPosition(cmdBuffer.par1, cmdBuffer.par2, cmdBuffer.par3);
        }
        break;

        case SET_RENDER_QA: {
          serRenderQuality(GPU_INTERFACE_GET_DATA_8());
        }
        break;

        case SET_TEXTURE_MODE: {
          setTextureQuality(GPU_INTERFACE_GET_DATA_8());
        }
        break;

        case SET_WALL_CLD: {
          setWallCollision(GPU_INTERFACE_GET_DATA_8());
        }
        break;

        case GET_CAM_POS: {
          getCamPosition(cmdBuffer.data);
          GPU_INTERFACE_SEND_ARR_DATA_8(cmdBuffer.data, 6);
        }
        break;

        case SET_RENDER_FPS: {
          setLimitFPS(GPU_INTERFACE_GET_DATA_8());
        }
        break;

          // ---------------- NOT_USED ---------------- //
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
          // -------------- 0x80 - 0xFE --------------- //
          // ------------------ General ----------------- //
        case BSY_SELECT: {
          bsyInterfaceFlag = GPU_INTERFACE_GET_DATA_8();
        }
        break;

        case CMD_GPU_PING: {
          GPU_INTERFACE_SEND_DATA_8(GPU_MSG_CODE_PING);
        }
        break;

        case CMD_GPU_SW_RESET: {
          main();
        }
        break;

        case SET_DBG_GPIO_PIN: {
          uint8_t state = GPU_INTERFACE_GET_DATA_8();
          if (state) {
            GPIO_SetBits(GPU_DEBUG_PORT, GPU_DEBUG_PIN);
          } else {
            GPIO_ResetBits(GPU_DEBUG_PORT, GPU_DEBUG_PIN);
          }
        }
        break;

        default: {
          GPU_INTERFACE_FFLUSH();
          // TO DO:
          // make sync;
          // buffer error alert
        }
        break;
#endif
      }
    }
  }
}
