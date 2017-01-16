/*
 *
 * For STM32_GPU Project
 * Creation start: 10.04.2016 20:21 (UTC+4)
 *
 * Created by: Antonov Alexandr (Bismuth208)
 *
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


#define SYNC_SEQUENCE   0x42DD
#define SYNC_OK         0xCC

#define BSY_MSG_CODE_WAIT       0xEE
#define BSY_MSG_CODE_READY      0xEA

#define MAX_TEXT_SIZE   30

// ------------------------------------------------------------------------------------ //



// ------------------------------------------------------------------------------------ //

// --------------- Constructor --------------- //
STMGPU::STMGPU(int8_t bsyPin):_bsyPin(bsyPin)
{
  if(bsyPin) {
    _useHardwareBsy = true;
  } else {
    _useHardwareBsy = false;
  }
}

STMGPU::STMGPU():_useHardwareBsy(false) {}
// ------------------------------------------- //

// this one make sync whith GPU
void STMGPU::begin(uint32_t baudRate)
{
  uint8_t syncData[2] = { 0x42, 0xDD};

  bool syncEstablished = false;
  
  Serial.begin(baudRate);
  
  if(_useHardwareBsy) {
    // setup GPU bsy pin
    pinMode(_bsyPin, INPUT);   // set as input
    digitalWrite(_bsyPin, HIGH); // pull-up
  }

  while(!syncEstablished) {
    while(Serial.available()==0){
      Serial.write(syncData, 0x02); // two bytes
      delay(1000); // one transfer per second
    }

    if(Serial.read() == SYNC_OK) {
      syncEstablished = true;
      
      while(Serial.available() < 3); // wait for resolution
      // get _width
      cmdBuffer.data[1] = Serial.read();
      cmdBuffer.data[2] = Serial.read();
      // get _height
      cmdBuffer.data[3] = Serial.read();
      cmdBuffer.data[4] = Serial.read();
      
      _width  = cmdBuffer.par1;
      _height = cmdBuffer.par2;
      
      Serial.flush();
    }
  }
}

// ------------------------------------------------------------------------------------ //
// this function is abstruction layer
// this allow to simply change the interface
void STMGPU::sendCommand(void *buf, uint8_t size)
{
  // wait untill GPU buffer will ready
  if(_useHardwareBsy) { // harware protection
    
    while(digitalRead(_bsyPin));
    
  } else { // software protection
    
    if(Serial.read() == BSY_MSG_CODE_WAIT) {
      while (Serial.read() != BSY_MSG_CODE_READY);
    }
  }
  
  Serial.write((uint8_t*)buf, size);
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
}

void STMGPU::fillScreen(uint16_t color)
{
  cmdBuffer.cmd = FLL_SCR;
  cmdBuffer.par1 = color;

  sendCommand(cmdBuffer.data, 3);
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
}

void STMGPU::drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color)
{
  cmdBuffer.cmd = DRW_V_LINE;
  cmdBuffer.par1 = x;
  cmdBuffer.par2 = y;
  cmdBuffer.par3 = h;
  cmdBuffer.par4 = color;

  sendCommand(cmdBuffer.data, 9);
}

void STMGPU::drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color)
{
  cmdBuffer.cmd = DRW_H_LINE;
  cmdBuffer.par1 = x;
  cmdBuffer.par2 = y;
  cmdBuffer.par3 = w;
  cmdBuffer.par4 = color;

  sendCommand(cmdBuffer.data, 9);
}

void STMGPU::drawCircle(int16_t x, int16_t y, int16_t r, uint16_t color)
{
  cmdBuffer.cmd = DRW_CIRCLE;
  cmdBuffer.par1 = x;
  cmdBuffer.par2 = y;
  cmdBuffer.par3 = r;
  cmdBuffer.par4 = color;

  sendCommand(cmdBuffer.data, 9);
}

void STMGPU::fillCircle(int16_t x, int16_t y, int16_t r, uint16_t color)
{
  cmdBuffer.cmd = FLL_CIRCLE;
  cmdBuffer.par1 = x;
  cmdBuffer.par2 = y;
  cmdBuffer.par3 = r;
  cmdBuffer.par4 = color;

  sendCommand(cmdBuffer.data, 9);
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
}

void STMGPU::getResolution(void)
{
  Serial.flush();
  Serial.print(GET_RESOLUTION);
  
  while(Serial.available() < 3); // wait for resolution
  // get _width
  cmdBuffer.data[1] = Serial.read();
  cmdBuffer.data[2] = Serial.read();
  // get _height
  cmdBuffer.data[3] = Serial.read();
  cmdBuffer.data[4] = Serial.read();
  
  _width  = cmdBuffer.par1;
  _height = cmdBuffer.par2;
}

int16_t STMGPU::height(void)
{
  return _height;
}

int16_t STMGPU::width(void)
{
  return _width;
}

// --------------- Font/Print --------------- //
// make a DDoS to GPU's buffer...
#if ARDUINO >= 100
size_t STMGPU::write(uint8_t c) {
#else
  void STMGPU::write(uint8_t c) {
#endif
    cmdBuffer.cmd = DRW_PRNT_C;
    cmdBuffer.data[1] = c;
    
    sendCommand(cmdBuffer.data, 2);
#if ARDUINO >= 100
    return 1;
#endif
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
}

void STMGPU::setTextSize(uint8_t size)
{
  cmdBuffer.cmd = SET_TXT_SIZE;
  cmdBuffer.data[1] = size;
  
  sendCommand(cmdBuffer.data, 2);
}

void STMGPU::setTextWrap(bool wrap)
{
  cmdBuffer.cmd = SET_TXT_WRAP;
  cmdBuffer.data[1] = wrap;
  
  sendCommand(cmdBuffer.data, 2);
}

void STMGPU::cp437(bool cp)
{
  cmdBuffer.cmd = SET_TXT_437;
  cmdBuffer.data[1] = cp;
  
  sendCommand(cmdBuffer.data, 2);
}
  
#if 0
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
void STMGPU::loadTile8x8(const char *tileSetArrName, uint8_t tileSetW,
                                  uint8_t ramTileNum, uint8_t tileNum)
{
  cmdBuffer.cmd = LDD_TLE_8;
  cmdBuffer.data[1] = strlen(tileSetArrName);
  cmdBuffer.data[2] = tileSetW;
  cmdBuffer.data[3] = ramTileNum;
  cmdBuffer.data[4] = tileNum;
    
  sendCommand(cmdBuffer.data, 5);
  sendCommand((void*)tileSetArrName, cmdBuffer.data[1]); // send name of file
}
  
void STMGPU::loadTileSet8x8(const char *tileSetArrName, uint8_t tileSetW,
                                      uint8_t ramTileBase, uint8_t tileMin, uint8_t tileMax)
{
  cmdBuffer.cmd = LDD_TLES_RG_8;
  cmdBuffer.data[1] = strlen(tileSetArrName);
  cmdBuffer.data[2] = tileSetW;
  cmdBuffer.data[3] = ramTileBase;
  cmdBuffer.data[4] = tileMin;
  cmdBuffer.data[5] = tileMax;
    
  sendCommand(cmdBuffer.data, 6);
  sendCommand((void*)tileSetArrName, cmdBuffer.data[1]); // send name of file
}

void STMGPU::drawTile8x8(int16_t posX, int16_t posY, uint8_t tileNum)
{
  cmdBuffer.cmd = DRW_TLE_8;
  cmdBuffer.par1 = posX;
  cmdBuffer.par2 = posY;
  cmdBuffer.data[5] = tileNum;
    
  sendCommand(cmdBuffer.data, 6);
}

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
  
  while(!Serial.available() ); // wait for state
  
  return Serial.read();
}

// ----------------- SD card ---------------- //
void STMGPU::printBMP(const char *fileName)
{
  cmdBuffer.cmd = DRW_BMP_FIL;
  cmdBuffer.par1 = 0;
  cmdBuffer.par2 = 0;
  cmdBuffer.data[5] = strlen(fileName);
  
  sendCommand(cmdBuffer.data, 6);
  sendCommand((void*)fileName, cmdBuffer.data[5]); // send name of file
}
  
void STMGPU::printBMP(const __FlashStringHelper* str)
{
  uint8_t c;
  PGM_P p = reinterpret_cast<PGM_P>(str);
    
  cmdBuffer.cmd = DRW_BMP_FIL;
  cmdBuffer.par1 = 0;
  cmdBuffer.par2 = 0;
  cmdBuffer.data[5] = strlen_P(p);
    
  sendCommand(cmdBuffer.data, 6);
    
  while ((c = pgm_read_byte(p)) != 0) {
    Serial.write(c);
    p++;
  }
}
  
void STMGPU::printBMP(uint16_t x, uint16_t y, const String &str)
{
  cmdBuffer.cmd = DRW_BMP_FIL;
  cmdBuffer.par1 = x;
  cmdBuffer.par2 = y;
  cmdBuffer.data[5] = str.length();
  
  sendCommand(cmdBuffer.data, 6);
  
  for (uint16_t i = 0; i < str.length(); i++) {
    Serial.write(str[i]);
  }
}
  
void STMGPU::printBMP(uint16_t x, uint16_t y, const char *fileName)
{
  cmdBuffer.cmd = DRW_BMP_FIL;
  cmdBuffer.par1 = x;
  cmdBuffer.par2 = y;
  cmdBuffer.data[5] = strlen(fileName);
  
  sendCommand(cmdBuffer.data, 6);
  sendCommand((void*)fileName, cmdBuffer.data[5]);
}
  
void STMGPU::printBMP(uint16_t x, uint16_t y, const __FlashStringHelper* str)
{
  uint8_t c;
  PGM_P p = reinterpret_cast<PGM_P>(str);
  
  cmdBuffer.cmd = DRW_BMP_FIL;
  cmdBuffer.par1 = x;
  cmdBuffer.par2 = y;
  cmdBuffer.data[5] = strlen_P(p);
  
  sendCommand(cmdBuffer.data, 6);
  
  while ((c = pgm_read_byte(p)) != 0) {
    Serial.write(c);
    p++;
  }
}
  
// -------------------- ___ ---------------------- //

// Pass 8-bit (each) R,G,B, get back 16-bit packed color
uint16_t STMGPU::color565(uint8_t r, uint8_t g, uint8_t b)
{
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

//void drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color);
//void drawBitmapBG(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color, uint16_t bg);
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
