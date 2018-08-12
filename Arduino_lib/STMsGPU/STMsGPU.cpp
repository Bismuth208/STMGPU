/*
 *
 * For STM32_sGPU Project
 * Created: 10.04.2016
 * Last edit: 12.08.2018
 *
 * author: Antonov Alexandr (Bismuth208)
 *
 * For addition info look read.me
 *
 */

#include "STMsGPU.h"

#ifdef __AVR
#include <avr/pgmspace.h>
#elif defined(ESP8266)
#include <pgmspace.h>
#endif

#include <limits.h>
#include "pins_arduino.h"
#include "wiring_private.h"

#define MAX_TEXT_SIZE   30

// ------------------------------------------------------------------------------------ //
#if defined(__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__) \
 || defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
#define rxPin 3
#define txPin 4
 SoftwareSerial gpuSoftwareSerial(rxPin, txPin); // this is physical pin 2 and 3
#endif

// ------------------------------------------------------------------------------------ //

// --------------- Constructor --------------- //
#if !REMOVE_HARDWARE_BSY
STMGPU::STMGPU(int8_t bsyPin):_bsyPin(bsyPin)
{
  _useHardwareBsy = bsyPin ? true : false;
}

STMGPU::STMGPU():_useHardwareBsy(false) {}
#else

STMGPU::STMGPU() {}
#endif /* REMOVE_HARDWARE_BSY */
// ------------------------------------------- //


// this one make sync whith sGPU
void STMGPU::begin(baudSpeed_t baudRate)
{
  bool syncEstablished = false;
  uint8_t syncData[] = {0x42, 0xDD};
  
#if defined(__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__) \
 || defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
  pSerial = &gpuSoftwareSerial;
#elif defined (__STM32F1__) || defined (__STM32F3__) || defined (__STM32F4__)
  pSerial = &Serial1; // PA9 and PA10
#elif defined (__AVR__)
  pSerial = &Serial;
#endif
  
  pSerial->begin(baudRate);
#if !REMOVE_HARDWARE_BSY
  if(_useHardwareBsy) {
    // setup sGPU bsy pin
    pinMode(_bsyPin, INPUT);     // set as input
    digitalWrite(_bsyPin, HIGH); // pull-up; does it really need? or left HI-z?
  }
#endif

  // This command need if you don't reset sGPU and just reconnect to it
  pSerial->write(CMD_GPU_SW_RESET); // in normal reset this command will be ignored
  
  while(!syncEstablished) {
    while(pSerial->available()==0) {
      pSerial->write(syncData, 0x02); // two bytes
      iDelay(1000); //delay(1000); // one transfer per second
    }

    if(pSerial->read() == SYNC_OK) {
      syncEstablished = true;
      
#if USE_GPU_RETURN_RESOLUTION
      while(pSerial->available() < 3); // wait for resolution
      // get _width
      cmdBuffer.data[1] = pSerial->read();
      cmdBuffer.data[2] = pSerial->read();
      // get _height
      cmdBuffer.data[3] = pSerial->read();
      cmdBuffer.data[4] = pSerial->read();
      
      _width  = cmdBuffer.par1;
      _height = cmdBuffer.par2;
      
      pSerial->flush();
#endif
    }
  }
}

// ------------------------------------------------------------------------------------ //
/* this function is abstruction layer
 * this allow to simply change the interface
 * and have some protection rules
 */
void STMGPU::sendCommand(void *buf, uint8_t size)
{
#if !REMOVE_HARDWARE_BSY
  // wait untill sGPU buffer will ready
  if(_useHardwareBsy) { // harware protection
    // yep, stop to doing everything and watch only for this pin!
    while(digitalRead(_bsyPin));
    
  } else { // software protection
    if(pSerial->read() == BSY_MSG_CODE_WAIT) {
      // same story as previous, all power of MCU will be concentrated for this
      while(pSerial->read() != BSY_MSG_CODE_READY);
    }
  }
#else
  if(pSerial->read() == BSY_MSG_CODE_WAIT) {
    // same story as previous, all power of MCU will be concentrated for this
    while(pSerial->read() != BSY_MSG_CODE_READY);
  }
#endif /* REMOVE_HARDWARE_BSY */
  
  // finally, send data to sGPU
  pSerial->write((uint8_t*)buf, size);
}

void STMGPU::sendCommand(uint8_t cmd, uint8_t i, ...)
{
  cmdBuffer.cmd = cmd;
  auto pArr = (uint16_t*)&cmdBuffer.data[1];

  va_list vl;
  va_start(vl, i);

  for(uint8_t n=0; n<i; n++) {
    *pArr++ = va_arg(vl, int16_t);
  }
  va_end(vl);

  sendCommand(cmdBuffer.data, (i<<1)+1);
}

// reque less ROM space and almost equal* to delay()
// *(no yeld() and less precition)
void STMGPU::iDelay(uint16_t duty)
{
  uint32_t currentMillis = millis();
  while((millis() - currentMillis) < duty) {};
}
// ------------------------------------------------------------------------------------ //


// ------------------ Base ------------------ //
void STMGPU::drawPixel(int16_t x, int16_t y, uint16_t color)
{
  cmdBuffer.cmd = DRW_PIXEL;
  cmdBuffer.par1 = x;
  cmdBuffer.par2 = y;
  cmdBuffer.par3 = color;

  sendCommand(cmdBuffer.data, 7);
  // sendCommand(DRW_PIXEL, 3, x, y, color);
}

void STMGPU::fillScreen(uint16_t color)
{
  cmdBuffer.cmd = FLL_SCR;
  cmdBuffer.par1 = color;

  sendCommand(cmdBuffer.data, 3);
  // sendCommand(FLL_SCR, 1, color);
}

// ------------- Primitives/GFX ------------- //

void STMGPU::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
  cmdBuffer.cmd = FLL_RECT;
  cmdBuffer.par1 = x;
  cmdBuffer.par2 = y;
  cmdBuffer.par3 = w;
  cmdBuffer.par4 = h;
  cmdBuffer.par5 = color;

  sendCommand(cmdBuffer.data, 11);
  // sendCommand(FLL_RECT, 5, x, y, w, h, color);
}

void STMGPU::drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
  cmdBuffer.cmd = DRW_RECT;
  cmdBuffer.par1 = x;
  cmdBuffer.par2 = y;
  cmdBuffer.par3 = w;
  cmdBuffer.par4 = h;
  cmdBuffer.par5 = color;

  sendCommand(cmdBuffer.data, 11);
  // sendCommand(DRW_RECT, 5, x, y, w, h, color);
}

void STMGPU::drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t radius, uint16_t color)
{
  cmdBuffer.cmd = DRW_ROUND_RECT;
  cmdBuffer.par1 = x;
  cmdBuffer.par2 = y;
  cmdBuffer.par3 = w;
  cmdBuffer.par4 = h;
  cmdBuffer.par5 = radius;
  cmdBuffer.par6 = color;
  
  sendCommand(cmdBuffer.data, 13);
  // sendCommand(DRW_ROUND_RECT, 6, x, y, w, h, radius, color);
}

void STMGPU::fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t radius, uint16_t color)
{
  cmdBuffer.cmd = FLL_ROUND_RECT;
  cmdBuffer.par1 = x;
  cmdBuffer.par2 = y;
  cmdBuffer.par3 = w;
  cmdBuffer.par4 = h;
  cmdBuffer.par5 = radius;
  cmdBuffer.par6 = color;
  
  sendCommand(cmdBuffer.data, 13);
  // sendCommand(FLL_ROUND_RECT, 6, x, y, w, h, radius, color);
}

void STMGPU::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color)
{
  cmdBuffer.cmd = DRW_LINE;
  cmdBuffer.par1 = x0;
  cmdBuffer.par2 = y0;
  cmdBuffer.par3 = x1;
  cmdBuffer.par4 = y1;
  cmdBuffer.par5 = color;

  sendCommand(cmdBuffer.data, 11);
  // sendCommand(DRW_LINE, 5, x0, y0, x1, y1, color);
}

void STMGPU::drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color)
{
  cmdBuffer.cmd = DRW_V_LINE;
  cmdBuffer.par1 = x;
  cmdBuffer.par2 = y;
  cmdBuffer.par3 = h;
  cmdBuffer.par4 = color;

  sendCommand(cmdBuffer.data, 9);
  // sendCommand(DRW_LINE, 4, x, y, h, color);
}

void STMGPU::drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color)
{
  cmdBuffer.cmd = DRW_H_LINE;
  cmdBuffer.par1 = x;
  cmdBuffer.par2 = y;
  cmdBuffer.par3 = w;
  cmdBuffer.par4 = color;

  sendCommand(cmdBuffer.data, 9);
  // sendCommand(DRW_H_LINE, 4, x, y, w, color);
}

void STMGPU::drawCircle(int16_t x, int16_t y, int16_t r, uint16_t color)
{
  cmdBuffer.cmd = DRW_CIRCLE;
  cmdBuffer.par1 = x;
  cmdBuffer.par2 = y;
  cmdBuffer.par3 = r;
  cmdBuffer.par4 = color;

  sendCommand(cmdBuffer.data, 9);
  // sendCommand(DRW_CIRCLE, 4, x, y, r, color);
}

void STMGPU::fillCircle(int16_t x, int16_t y, int16_t r, uint16_t color)
{
  cmdBuffer.cmd = FLL_CIRCLE;
  cmdBuffer.par1 = x;
  cmdBuffer.par2 = y;
  cmdBuffer.par3 = r;
  cmdBuffer.par4 = color;

  sendCommand(cmdBuffer.data, 9);
  // sendCommand(FLL_CIRCLE, 4, x, y, r, color);
}

void STMGPU::drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
{
  cmdBuffer.cmd = DRW_TRINGLE;
  cmdBuffer.par1 = x0;
  cmdBuffer.par2 = y0;
  cmdBuffer.par3 = x1;
  cmdBuffer.par4 = y1;
  cmdBuffer.par5 = x2;
  cmdBuffer.par6 = y2;
  cmdBuffer.par7 = color;

  sendCommand(cmdBuffer.data, 15);
  // sendCommand(DRW_TRINGLE, 7, x0, y0, x1, y1, x2, y2, color);
}

void STMGPU::fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
{
  cmdBuffer.cmd = FLL_TRINGLE;
  cmdBuffer.par1 = x0;
  cmdBuffer.par2 = y0;
  cmdBuffer.par3 = x1;
  cmdBuffer.par4 = y1;
  cmdBuffer.par5 = x2;
  cmdBuffer.par6 = y2;
  cmdBuffer.par7 = color;

  sendCommand(cmdBuffer.data, 15);
  // sendCommand(FLL_TRINGLE, 7, x0, y0, x1, y1, x2, y2, color);
}

#if USE_GPU_RETURN_RESOLUTION
void STMGPU::getResolution(void)
{
  pSerial->flush();
  pSerial->print(GET_RESOLUTION);
  
  while(pSerial->available() < 3); // wait for resolution
  // get _width
  cmdBuffer.data[1] = pSerial->read();
  cmdBuffer.data[2] = pSerial->read();
  // get _height
  cmdBuffer.data[3] = pSerial->read();
  cmdBuffer.data[4] = pSerial->read();
  
  _width  = cmdBuffer.par1;
  _height = cmdBuffer.par2;
}
#endif

// --------------- Font/Print --------------- //
// make a DDoS to sGPU's buffer...
size_t STMGPU::write(uint8_t c)
{
  cmdBuffer.cmd = DRW_PRNT_C;
  cmdBuffer.data[1] = c;
    
  // DO NOT CHAHNGE TO: pSerial->write(cmdBuffer.data, 2); !
  // ONLY sendCommand(cmdBuffer.data, 2); FUNCTION ALLOWED!
  // OTHERWISE sGPU WILL BE VEEERY UNSTABLE!
  sendCommand(cmdBuffer.data, 2);

  return 1;
}

void STMGPU::drawChar(int16_t x, int16_t y, uint8_t c, uint16_t color, uint16_t bg, uint8_t size)
{
  cmdBuffer.cmd = DRW_CHAR;
  cmdBuffer.par1 = x;
  cmdBuffer.par2 = y;
  cmdBuffer.par3 = color;
  cmdBuffer.par4 = bg;
  cmdBuffer.data[9] = c;
  cmdBuffer.data[10] = size;
  
  sendCommand(cmdBuffer.data, 11);
}

void STMGPU::setCursor(int16_t x, int16_t y)
{
  cmdBuffer.cmd = SET_CURSOR;
  cmdBuffer.par1 = x;
  cmdBuffer.par2 = y;

  sendCommand(cmdBuffer.data, 5);
  // sendCommand(SET_CURSOR, 2, x, y);
}

void STMGPU::setTextColor(uint16_t color)
{
  cmdBuffer.cmd = SET_TXT_CR;
  cmdBuffer.par1 = color;

  sendCommand(cmdBuffer.data, 3);
}

void STMGPU::setTextColor(uint16_t color, uint16_t bg)
{
  cmdBuffer.cmd = SET_TXT_CR_BG;
  cmdBuffer.par1 = color;
  cmdBuffer.par2 = bg;

  sendCommand(cmdBuffer.data, 5);
  // sendCommand(SET_TXT_CR_BG, 2, color, bg);
}

void STMGPU::setTextSize(uint8_t size)
{
  cmdBuffer.cmd = SET_TXT_SIZE;
  cmdBuffer.data[1] = size;
  
  sendCommand(cmdBuffer.data, 2);
  // sendCommand(SET_TXT_SIZE, 1, size);
}

void STMGPU::setTextWrap(bool wrap)
{
  cmdBuffer.cmd = SET_TXT_WRAP;
  cmdBuffer.data[1] = wrap;
  
  sendCommand(cmdBuffer.data, 2);
  // sendCommand(SET_TXT_WRAP, 1, wrap);
}

void STMGPU::cp437(bool cp)
{
  cmdBuffer.cmd = SET_TXT_437;
  cmdBuffer.data[1] = cp;
  
  sendCommand(cmdBuffer.data, 2);
  // sendCommand(SET_TXT_WRAP, 1, cp);
}
  
#if 0
// this is much faster, but how to add this...
size_t STMGPU::print(const char *str)
{
  cmdBuffer.cmd = DRW_PRNT;
  cmdBuffer.data[1] = strlen(str);
  
  sendCommand(cmdBuffer.data, 2);
  sendCommand((void*)str, cmdBuffer.data[1]);
  return 0;
}
#endif
// --------- //
void STMGPU::printAt(int16_t x, int16_t y,  char c)
{
  cmdBuffer.cmd = DRW_PRNT_POS_C;
  cmdBuffer.par1 = x;
  cmdBuffer.par2 = y;
  cmdBuffer.par3 = c;
  
  sendCommand(cmdBuffer.data, 6);
  // sendCommand(DRW_PRNT_POS_C, 3, x, y, c);
}

void STMGPU::printAt(int16_t x, int16_t y, const char *str)
{
  setCursor(x, y);
  print(str);
}

void STMGPU::printAt(int16_t x, int16_t y, const String &str)
{
  setCursor(x, y);
  print(str);
}

void STMGPU::printAt(int16_t x, int16_t y, const __FlashStringHelper *str)
{
  setCursor(x, y);
  print(str);
}


// ---------------- Low Level --------------- //
void STMGPU::setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
  cmdBuffer.cmd = SET_ADR_WIN;
  cmdBuffer.par1 = x0;
  cmdBuffer.par2 = y0;
  cmdBuffer.par3 = x1;
  cmdBuffer.par4 = y1;
  
  sendCommand(cmdBuffer.data, 9);
}

void STMGPU::setRotation(uint8_t m)
{
  cmdBuffer.cmd = SET_ROTATION;
  cmdBuffer.data[1] = m;
  
  sendCommand(cmdBuffer.data, 2);
}

void STMGPU::setScrollArea(uint16_t TFA, uint16_t BFA)
{
  cmdBuffer.cmd = SET_SCRL_AREA;
  cmdBuffer.par1 = TFA;
  cmdBuffer.par2 = BFA;
  
  sendCommand(cmdBuffer.data, 5);
}

void STMGPU::scrollAddress(uint16_t VSP)
{
  cmdBuffer.cmd = SET_V_SCRL_ADR;
  cmdBuffer.par1 = VSP;
  
  sendCommand(cmdBuffer.data, 3);
}

uint16_t STMGPU::scroll(uint16_t lines, uint16_t yStart)
{
  cmdBuffer.cmd = MAK_SCRL;
  cmdBuffer.par1 = lines;
  cmdBuffer.par2 = yStart;
  
  sendCommand(cmdBuffer.data, 5);
  
  return 0; // Fix this in future
}
/*
uint16_t STMGPU::scrollSmooth(uint16_t lines, uint16_t yStart, uint8_t wait)
{
  cmd_T_Buf.cmd = MAK_SCRL_SMTH;
  cmd_T_Buf.par0 = wait;
  cmd_T_Buf.par1 = lines;
  cmd_T_Buf.par2 = yStart;
  
  sendCommand(cmdBuffer.data, 6);
}
 */

void STMGPU::setSleep(bool enable)
{
  cmdBuffer.cmd = SET_SLEEP;
  cmdBuffer.data[1] = enable;
  
  sendCommand(cmdBuffer.data, 2);
}

void STMGPU::setIdleMode(bool mode)
{
  cmdBuffer.cmd = SET_IDLE;
  cmdBuffer.data[1] = mode;
  
  sendCommand(cmdBuffer.data, 2);
}

void STMGPU::setDispBrightness(uint8_t brightness)
{
  cmdBuffer.cmd = SET_BRIGHTNES;
  cmdBuffer.data[1] = brightness;
  
  sendCommand(cmdBuffer.data, 2);
}

void STMGPU::setInvertion(bool i)
{
  cmdBuffer.cmd = SET_INVERTION;
  cmdBuffer.data[1] = i;
  
  sendCommand(cmdBuffer.data, 2);
}

//void setGamma(uint8_t gamma);
// SET_GAMMA

void STMGPU::pushColor(uint16_t color)
{
  cmdBuffer.cmd = PSH_CR;
  cmdBuffer.par1 = color;
  
  sendCommand(cmdBuffer.data, 3);
}

void STMGPU::writeCommand(uint8_t c)
{
  cmdBuffer.cmd = WRT_CMD;
  cmdBuffer.data[1] = c;
  
  sendCommand(cmdBuffer.data, 2);
}

void STMGPU::writeData(uint8_t d)
{
  cmdBuffer.cmd = WRT_DATA;
  cmdBuffer.data[1] = d;
  
  sendCommand(cmdBuffer.data, 2);
}

void STMGPU::writeWordData(uint16_t c)
{
  cmdBuffer.cmd = WRT_DATA_U16;
  cmdBuffer.par1 = c;
  
  sendCommand(cmdBuffer.data, 3);
}
  
// ------------------- Tile ----------------- //
void STMGPU::sendTileData(uint8_t tileType, int16_t posX, int16_t posY, uint8_t tileNum)
{
  cmdBuffer.cmd = tileType;
  cmdBuffer.par1 = posX;
  cmdBuffer.par2 = posY;
  cmdBuffer.data[5] = tileNum;
  
  sendCommand(cmdBuffer.data, 6);
}
  
void STMGPU::loadTileBase(uint8_t tileType, const char *tileSetArrName, uint8_t tileSetW,
                           uint8_t ramTileNum, uint8_t tileNum)
{
  cmdBuffer.cmd = tileType;
  cmdBuffer.data[1] = strlen(tileSetArrName);
  cmdBuffer.data[2] = tileSetW;
  cmdBuffer.data[3] = ramTileNum;
  cmdBuffer.data[4] = tileNum;
    
  sendCommand(cmdBuffer.data, 5);
  sendCommand((void*)tileSetArrName, cmdBuffer.data[1]); // send name of file
}
  
void STMGPU::loadTileBase(uint8_t tileType, const char *tileSetArrName, uint8_t tileSetW,
                            uint8_t ramTileBase, uint8_t tileMin, uint8_t tileMax)
{
  cmdBuffer.cmd = tileType;
  cmdBuffer.data[1] = strlen(tileSetArrName);
  cmdBuffer.data[2] = tileSetW;
  cmdBuffer.data[3] = ramTileBase;
  cmdBuffer.data[4] = tileMin;
  cmdBuffer.data[5] = tileMax;
  
  sendCommand(cmdBuffer.data, 6);
  sendCommand((void*)tileSetArrName, cmdBuffer.data[1]); // send name of file
}
  
  // ---- tile 8x8 ---- //
void STMGPU::loadTile8x8(const char *tileSetArrName, uint8_t tileSetW,
                                  uint8_t ramTileNum, uint8_t tileNum)
{
  loadTileBase(LDD_TLE_8, tileSetArrName, tileSetW, ramTileNum, tileNum);
}
  
void STMGPU::loadTileSet8x8(const char *tileSetArrName, uint8_t tileSetW,
                                      uint8_t ramTileBase, uint8_t tileMin, uint8_t tileMax)
{
  loadTileBase(LDD_TLES_8, tileSetArrName, tileSetW, ramTileBase, tileMin, tileMax);
}

void STMGPU::drawTile8x8(int16_t posX, int16_t posY, uint8_t tileNum)
{
  sendTileData(DRW_TLE_8, posX, posY, tileNum);
}
  
  // ---- tile 16x16 ---- //
void STMGPU::loadTile16x16(const char *tileSetArrName, uint8_t tileSetW,
                                  uint8_t ramTileNum, uint8_t tileNum)
{
  loadTileBase(LDD_TLE_16, tileSetArrName, tileSetW, ramTileNum, tileNum);
}
  
void STMGPU::loadTileSet16x16(const char *tileSetArrName, uint8_t tileSetW,
                                      uint8_t ramTileBase, uint8_t tileMin, uint8_t tileMax)
{
  loadTileBase(LDD_TLES_16, tileSetArrName, tileSetW, ramTileBase, tileMin, tileMax);
}

void STMGPU::drawTile16x16(int16_t posX, int16_t posY, uint8_t tileNum)
{
  sendTileData(DRW_TLE_16, posX, posY, tileNum);
}
  
  // ---- tile 32x32 ---- //
void STMGPU::loadTile32x32(const char *tileSetArrName, uint8_t tileSetW,
                                  uint8_t ramTileNum, uint8_t tileNum)
{
  loadTileBase(LDD_TLE_32, tileSetArrName, tileSetW, ramTileNum, tileNum);
}
  
void STMGPU::loadTileSet32x32(const char *tileSetArrName, uint8_t tileSetW,
                                      uint8_t ramTileBase, uint8_t tileMin, uint8_t tileMax)
{
  loadTileBase(LDD_TLES_32, tileSetArrName, tileSetW, ramTileBase, tileMin, tileMax);
}

void STMGPU::drawTile32x32(int16_t posX, int16_t posY, uint8_t tileNum)
{
  sendTileData(DRW_TLE_32, posX, posY, tileNum);
}

// universal tile draw 8, 16, 32
/*
void STMGPU::drawTile(int16_t posX, int16_t posY, uint8_t tileType, uint8_t tileNum)
{
  cmdBuffer.cmd = DRW_TLE_U;
  cmdBuffer.par1 = posX;
  cmdBuffer.par2 = posY;
  cmdBuffer.data[5] = tileType;
  cmdBuffer.data[6] = tileNum;
  
  sendCommand(cmdBuffer.data, 7);
}
*/  

void STMGPU::loadTileMap(const char *fileName)
{
  cmdBuffer.cmd = LDD_TLE_MAP;
  cmdBuffer.data[1] = strlen(fileName);
  
  sendCommand(cmdBuffer.data, 2);
  sendCommand((void*)fileName, cmdBuffer.data[1]); // send name of file
}

void STMGPU::drawTileMap(void)
{
  cmdBuffer.cmd = DRW_TLE_MAP;
  
  sendCommand(cmdBuffer.data, 1);
}

// ----------------- Sprite ----------------- //
void STMGPU::setSpritePosition(uint8_t sprNum, uint16_t posX, uint16_t posY)
{
  cmdBuffer.cmd = SET_SPR_POS;
  cmdBuffer.par1 = (uint16_t)sprNum;
  cmdBuffer.par2 = posX;
  cmdBuffer.par3 = posY;
  
  sendCommand(cmdBuffer.data, 7);
}

void STMGPU::setSpriteType(uint8_t sprNum, uint8_t type)
{
  cmdBuffer.cmd = SET_SPR_TYPE;
  cmdBuffer.data[1] = sprNum;
  cmdBuffer.data[2] = type;
  
  sendCommand(cmdBuffer.data, 3);
}

void STMGPU::setSpriteVisible(uint8_t sprNum, uint8_t state)
{
  cmdBuffer.cmd = SET_SPR_VISBL;
  cmdBuffer.data[1] = sprNum;
  cmdBuffer.data[2] = state;
  
  sendCommand(cmdBuffer.data, 3);
}

void STMGPU::setSpriteTiles(uint8_t sprNum, uint8_t tle1, uint8_t tle2, uint8_t tle3, uint8_t tle4)
{
  cmdBuffer.cmd = SET_SPR_TLE;
  cmdBuffer.data[1] = sprNum;
  cmdBuffer.data[2] = tle1;
  cmdBuffer.data[3] = tle2;
  cmdBuffer.data[4] = tle3;
  cmdBuffer.data[5] = tle4;
  
  sendCommand(cmdBuffer.data, 6);
}

void STMGPU::setSpritesAutoRedraw(uint8_t state)
{
  cmdBuffer.cmd = SET_SPR_AUT_R;
  cmdBuffer.data[1] = state;
  
  sendCommand(cmdBuffer.data, 2);
}

void STMGPU::drawSprite(uint8_t sprNum)
{
  cmdBuffer.cmd = DRW_SPR;
  cmdBuffer.data[1] = sprNum;
  
  sendCommand(cmdBuffer.data, 2);
}
  
// in future i fix that...
void STMGPU::drawSprite(uint8_t sprNum, uint16_t posX, uint16_t posY)
{
  setSpritePosition(sprNum, posX, posY);
  
  cmdBuffer.cmd = DRW_SPR;
  cmdBuffer.data[1] = sprNum;
  
  sendCommand(cmdBuffer.data, 2);
}

bool STMGPU::getSpriteCollision(uint8_t sprNum1, uint8_t sprNum2)
{
  cmdBuffer.cmd = GET_SRP_COLISN;
  cmdBuffer.data[1] = sprNum1;
  cmdBuffer.data[2] = sprNum2;
  
  sendCommand(cmdBuffer.data, 3);
  
  while(!pSerial->available() ); // wait for state
  return pSerial->read();
}

// ----------------- SD card ---------------- //
void STMGPU::loadPalette(const char *palleteArrName)
{
  cmdBuffer.cmd = LDD_USR_PAL;
  cmdBuffer.data[1] = strlen(palleteArrName);
  
  sendCommand(cmdBuffer.data, 2);
  sendCommand((void*)palleteArrName, cmdBuffer.data[1]); // send name of file
}

void STMGPU::sendBaseBMP(uint16_t x, uint16_t y, uint16_t size)
{
  cmdBuffer.cmd = DRW_BMP_FIL;
  cmdBuffer.par1 = x;
  cmdBuffer.par2 = y;
  cmdBuffer.data[5] = size;
  
  sendCommand(cmdBuffer.data, 6);
}
  
void STMGPU::printBMP(const char *fileName)
{
  sendBaseBMP(0, 0, strlen(fileName));
  sendCommand((void*)fileName, cmdBuffer.data[5]); // send name of file
}
  
void STMGPU::printBMP(const __FlashStringHelper *str)
{
  register uint8_t c;
  PGM_P p = reinterpret_cast<PGM_P>(str);
  
  sendBaseBMP(0, 0, strlen_P(p));
  
  for(; (c = pgm_read_byte(p)); p++) {
    pSerial->write(c);
  }
}
  
void STMGPU::printBMP(uint16_t x, uint16_t y, const String &str)
{
  sendBaseBMP(x, y, str.length());
  
  for (uint16_t i = 0; i < str.length(); i++) {
    pSerial->write(str[i]);
  }
}
  
void STMGPU::printBMP(uint16_t x, uint16_t y, const char *fileName)
{
  sendBaseBMP(x, y, strlen(fileName));
  sendCommand((void*)fileName, cmdBuffer.data[5]);
}
  
void STMGPU::printBMP(uint16_t x, uint16_t y, const __FlashStringHelper *str)
{
  register uint8_t c;
  PGM_P p = reinterpret_cast<PGM_P>(str);
  
  sendBaseBMP(x, y, strlen_P(p));
  
  for(; (c = pgm_read_byte(p)); p++) {
    pSerial->write(c);
  }
}
  
// ------------------ Sound ----------------- //
void STMGPU::playNote(uint16_t freq, uint16_t duration)
{
  cmdBuffer.cmd = SND_PLAY_TONE;
  cmdBuffer.par1 = freq;
  cmdBuffer.par2 = duration;
  
  sendCommand(cmdBuffer.data, 5);
}

// --------------- GUI commands -------------- //
void STMGPU::setTextSizeGUI(uint8_t size)
{
  cmdBuffer.cmd = SET_WND_TXT_SZ;
  cmdBuffer.data[1] = size;
  
  sendCommand(cmdBuffer.data, 2);
}

void STMGPU::setTextColorGUI(uint16_t text, uint16_t bg)
{
  cmdBuffer.cmd = SET_WND_CR_TXT;
  cmdBuffer.par1 = text;
  cmdBuffer.par2 = bg;
  
  sendCommand(cmdBuffer.data, 5);
}
  
/*
void STMGPU::setTextColorGUI(uint16_t text)
{
  cmdBuffer.cmd = SET_WND_CR_TXT;
  cmdBuffer.par1 = text;
    
  sendCommand(cmdBuffer.data, 3);
}
*/

void STMGPU::setColorWindowGUI(uint16_t frame, uint16_t border)
{
  cmdBuffer.cmd = SET_WND_CR;
  cmdBuffer.par1 = frame;
  cmdBuffer.par2 = border;
  
  sendCommand(cmdBuffer.data, 5);
}
  
void STMGPU::drawWindowGUI(int16_t posX, int16_t posY, int16_t w, int16_t h)
{
  cmdBuffer.cmd = DRW_WND_AT;
  cmdBuffer.par1 = posX;
  cmdBuffer.par2 = posY;
  cmdBuffer.par3 = w;
  cmdBuffer.par4 = h;
  
  sendCommand(cmdBuffer.data, 9);
}

void STMGPU::drawWindowGUI(int16_t posX, int16_t posY,
                               int16_t w, int16_t h, const char *text)
{
  cmdBuffer.cmd = DRW_WND_TXT;
  cmdBuffer.par1 = posX;
  cmdBuffer.par2 = posY;
  cmdBuffer.par3 = w;
  cmdBuffer.par4 = h;
  cmdBuffer.data[9] = strlen(text);
  
  sendCommand(cmdBuffer.data, 10);
  sendCommand((void*)text, cmdBuffer.par5);
}
  
void STMGPU::drawWindowGUI(int16_t posX, int16_t posY,
                               int16_t w, int16_t h, const __FlashStringHelper* str)
{
  PGM_P p = reinterpret_cast<PGM_P>(str);
  
  cmdBuffer.cmd = DRW_WND_TXT;
  cmdBuffer.par1 = posX;
  cmdBuffer.par2 = posY;
  cmdBuffer.par3 = w;
  cmdBuffer.par4 = h;
  cmdBuffer.data[9] = strlen_P(p);
  
  sendCommand(cmdBuffer.data, 10);
  
  for (uint8_t count=0; count < cmdBuffer.data[9]; count++) {
    pSerial->write(pgm_read_byte(p + count));
  }
}
  
// --------------- '3D' engine --------------- //
void STMGPU::renderFrame(void)
{
  // yep... only 1 byte. I make this for protect sGPU from DDoS.
  cmdBuffer.cmd = RENDER_MAP;
  sendCommand(cmdBuffer.data, 1);
}
  
void STMGPU::moveCamera(uint8_t direction)
{
  cmdBuffer.cmd = MOVE_CAMERA;
  cmdBuffer.data[1] = direction;
  
  sendCommand(cmdBuffer.data, 2);
}
  
void STMGPU::setCamPosition(uint16_t posX, uint16_t posY, uint16_t angle)
{
 cmdBuffer.cmd = SET_CAM_POS;
 cmdBuffer.par1 = posX;
 cmdBuffer.par2 = posY;
 cmdBuffer.par3 = angle;
 
 sendCommand(cmdBuffer.data, 7);
}

void STMGPU::setWallCollision(bool state)
{
  cmdBuffer.cmd = SET_WALL_CLD;
  cmdBuffer.data[1] = state;
  
  sendCommand(cmdBuffer.data, 2);
}

void STMGPU::getCamPosition(uint16_t *pArrPos)
{
  cmdBuffer.cmd = GET_CAM_POS;
  sendCommand(cmdBuffer.data, 1);
  
  while(pSerial->available() < 6) { // wait for: posX, posY and angle
    pArrPos[0] = pSerial->read();
    pArrPos[0] |= pSerial->read()<<8;
    
    pArrPos[1] = pSerial->read();
    pArrPos[1] |= pSerial->read()<<8;
    
    pArrPos[2] = pSerial->read();
    pArrPos[2] |= pSerial->read()<<8;
  }
}

void STMGPU::setSkyFloor(uint16_t sky, uint16_t floor)
{
  cmdBuffer.cmd = SET_BACKGRND;
  cmdBuffer.par1 = sky;
  cmdBuffer.par2 = floor;
  
  sendCommand(cmdBuffer.data, 5);
}

// ------------------ General ----------------- //
#if !REMOVE_HARDWARE_BSY
// software or hardware
void STMGPU::setBusyMode(bool state)
{
  cmdBuffer.cmd = BSY_SELECT;
  cmdBuffer.data[1] = state;

  _useHardwareBsy = state ? true : false;
  
  sendCommand(cmdBuffer.data, 2);
}
#endif

void STMGPU::pingCommand(void)
{
  cmdBuffer.cmd = CMD_GPU_PING;
  sendCommand(cmdBuffer.data, 1);

  bool waitPingAnswer = true;
  while(waitPingAnswer) {
      while(pSerial->available()==0);

      if(pSerial->read() == GPU_MSG_CODE_PING) {
        waitPingAnswer = false;
    }
  }
}

void STMGPU::swReset(void)
{
  cmdBuffer.cmd = CMD_GPU_SW_RESET;
  sendCommand(cmdBuffer.data, 1);
}

void STMGPU::setDebugGPIOState(bool state)
{
  cmdBuffer.cmd = SET_DBG_GPIO_PIN;
  cmdBuffer.data[1] = state;
  
  sendCommand(cmdBuffer.data, 2);
}


// -------------------- ___ ---------------------- //

// Pass 8-bit (each) R,G,B, get back 16-bit packed color
uint16_t STMGPU::color565(uint8_t r, uint8_t g, uint8_t b)
{
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}


// Draw a 1-bit image (bitmap) at the specified (x,y) position from the
// provided bitmap buffer (must be PROGMEM memory) using the specified
// foreground color (unset bits are transparent).
void STMGPU::drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap,
                        int16_t w, int16_t h, uint16_t color)
{
  
  int16_t i, j, byteWidth = (w + 7) / 8;
  uint8_t byte;
  
  for(j=0; j<h; j++) {
    for(i=0; i<w; i++) {
      if(i & 7) byte <<= 1;
      else      byte   = pgm_read_byte(bitmap + j * byteWidth + i / 8);
      if(byte & 0x80) drawPixel(x+i, y+j, color);
    }
  }
}

// Draw a 1-bit image (bitmap) at the specified (x,y) position from the
// provided bitmap buffer (must be PROGMEM memory) using the specified
// foreground (for set bits) and background (for clear bits) colors.
void STMGPU::drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap,
                        int16_t w, int16_t h, uint16_t color, uint16_t bg)
{
  
  int16_t i, j, byteWidth = (w + 7) / 8;
  uint8_t byte;
  
  for(j=0; j<h; j++) {
    for(i=0; i<w; i++ ) {
      if(i & 7) byte <<= 1;
      else      byte   = pgm_read_byte(bitmap + j * byteWidth + i / 8);
      if(byte & 0x80) drawPixel(x+i, y+j, color);
      else            drawPixel(x+i, y+j, bg);
    }
  }
}

// drawBitmap() variant for RAM-resident (not PROGMEM) bitmaps.
void STMGPU::drawBitmap(int16_t x, int16_t y, uint8_t *bitmap,
                        int16_t w, int16_t h, uint16_t color)
{
  
  int16_t i, j, byteWidth = (w + 7) / 8;
  uint8_t byte;
  
  for(j=0; j<h; j++) {
    for(i=0; i<w; i++ ) {
      if(i & 7) byte <<= 1;
      else      byte   = bitmap[j * byteWidth + i / 8];
      if(byte & 0x80) drawPixel(x+i, y+j, color);
    }
  }
}

// drawBitmap() variant w/background for RAM-resident (not PROGMEM) bitmaps.
void STMGPU::drawBitmap(int16_t x, int16_t y, uint8_t *bitmap,
                        int16_t w, int16_t h, uint16_t color, uint16_t bg)
{
  
  int16_t i, j, byteWidth = (w + 7) / 8;
  uint8_t byte;
  
  for(j=0; j<h; j++) {
    for(i=0; i<w; i++ ) {
      if(i & 7) byte <<= 1;
      else      byte   = bitmap[j * byteWidth + i / 8];
      if(byte & 0x80) drawPixel(x+i, y+j, color);
      else            drawPixel(x+i, y+j, bg);
    }
  }
}

void STMGPU::drawXBitmap(int16_t x, int16_t y, const uint8_t *bitmap,
                         int16_t w, int16_t h, uint16_t color)
{
  int16_t i, j, byteWidth = (w + 7) / 8;
  
  for(j=0; j<h; j++) {
    for(i=0; i<w; i++ ) {
      if(pgm_read_byte(bitmap + j * byteWidth + i / 8) & (1 << (i % 8))) {
        drawPixel(x+i, y+j, color);
      }
    }
  }
}
