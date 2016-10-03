/*
 *
 * For STM32_GPU Project
 * Creation start: 10.04.2016 20:21 (UTC+4)
 *
 * Created by: Antonov Alexandr (Bismuth208)
 *
 *
 */

#include "STMGPU_gfx.h"

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

static cmdBuffer_t cmdBuffer;
//static cmdBuffer2_t cmd_T_Buf;
//static uint8_t cmdBufferStr[MAX_TEXT_SIZE];

// ------------------------------------------------------------------------------------ //


STMGPU::STMGPU(int8_t bsyPin)
{
  if(bsyPin) {
    _bsyPin = bsyPin;
    _useSofwareBsy = false;
  } else {
    _bsyPin = -1;
    _useSofwareBsy = true;
  }
}

STMGPU::STMGPU()
{
  _bsyPin = -1;
  _useSofwareBsy = true;
}

void STMGPU::sync(uint32_t baudRate)
{
  uint8_t syncData[2] = { 0x42, 0xDD};

  bool syncEstablished = false;
  
  Serial.begin(baudRate);
  
  if(!_useSofwareBsy) {
    // setup GPU bsy pin
    pinMode(_bsyPin, INPUT);   // set as input
    digitalWrite(_bsyPin, HIGH); // pull-up
  }

  while(!syncEstablished) {
    while(Serial.available()==0){
      Serial.write(syncData, 0x02); // two bytes
      delay(1000);
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
  if(_useSofwareBsy) { // software protection
    if(Serial.read() == BSY_MSG_CODE_WAIT) {
      while (Serial.read() != BSY_MSG_CODE_READY);
    }
  } else { // harware protection
    while(digitalRead(_bsyPin));
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
#if ARDUINO >= 100
size_t STMGPU::write(uint8_t c) {
#else
  void STMGPU::write(uint8_t c) {
#endif
    printChar(c);
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
  cmdBuffer.par5 = c;
  cmdBuffer.par6 = size;
  
  sendCommand(cmdBuffer.data, 13);
   
  /*
  setTextSize(size);
  //setCursor(x, y);
  setTextColorBG(color, bg);
  //printChar(c);
   
  printCharPos(x, y, c);
   */
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
  cmdBuffer.par1 = size;

  sendCommand(cmdBuffer.data, 2);
}

void STMGPU::setTextWrap(bool wrap)
{
  cmdBuffer.cmd = SET_TXT_WRAP;
  cmdBuffer.par1 = wrap;

  sendCommand(cmdBuffer.data, 2);
}

void STMGPU::cp437(bool cp)
{
  cmdBuffer.cmd = SET_TXT_437;
  cmdBuffer.par1 = cp;

  sendCommand(cmdBuffer.data, 2);
}

/*
void print(const char *str)
{
  uint8_t strSize = strlen(str);
  
  cmdBufferStr[0] = DRW_PRNT;
  cmdBufferStr[1] = DRW_PRNT;
  
  memcpy(cmdBufferStr[2], str, strSize);
  
  sendCommand(cmdBufferStr, strSize + 0x02);
}
 
void tftPrintPGR(const char *str)
{
 
}
*/

// make a DDoS to GPU's buffer...
void STMGPU::print(const char *str)
{
  uint16_t strSize = strlen(str);
  
  for (uint16_t count=0; count < strSize; count++) {
    cmdBuffer.cmd = DRW_PRNT_C;
    cmdBuffer.par1 = str[count];
    
    sendCommand(cmdBuffer.data, 2);
  }
}

// make a DDoS to GPU's buffer...
void STMGPU::printPGR(const char *str)
{
  uint16_t strSize = strlen_P(str);
  
  for (uint16_t count=0; count < strSize; count++) {
    cmdBuffer.cmd = DRW_PRNT_C;
    cmdBuffer.par1 = pgm_read_byte(str + count);
    
    sendCommand(cmdBuffer.data, 2);
  }
}

void STMGPU::printChar(uint8_t c)
{
  cmdBuffer.cmd = DRW_PRNT_C;
  cmdBuffer.par1 = c;

  sendCommand(cmdBuffer.data, 2);
}

void STMGPU::printCharPos(int16_t x, int16_t y, uint8_t c)
{
  cmdBuffer.cmd = DRW_PRNT_POS_C;
  cmdBuffer.par1 = x;
  cmdBuffer.par2 = y;
  cmdBuffer.par3 = c;
  
  sendCommand(cmdBuffer.data, 6);
}


// ---------------- Low Level --------------- //
void STMGPU::setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
  cmdBuffer.cmd = SET_ADR_WIN;
  cmdBuffer.par1 = x0;
  cmdBuffer.par2 = x0;
  cmdBuffer.par3 = x1;
  cmdBuffer.par4 = y1;
  
  sendCommand(cmdBuffer.data, 9);
}

void STMGPU::setRotation(uint8_t m)
{
  cmdBuffer.cmd = SET_ROTATION;
  cmdBuffer.par1 = m;
  
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
  cmdBuffer.par1 = enable;
  
  sendCommand(cmdBuffer.data, 2);
}

void STMGPU::setIdleMode(bool mode)
{
  cmdBuffer.cmd = SET_IDLE;
  cmdBuffer.par1 = mode;
  
  sendCommand(cmdBuffer.data, 2);
}

void STMGPU::setDispBrightness(uint8_t brightness)
{
  cmdBuffer.cmd = SET_BRIGHTNES;
  cmdBuffer.par1 = brightness;
  
  sendCommand(cmdBuffer.data, 2);
}

void STMGPU::setInvertion(bool i)
{
  cmdBuffer.cmd = SET_INVERTION;
  cmdBuffer.par1 = i;
  
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
  cmdBuffer.par1 = c;
  
  sendCommand(cmdBuffer.data, 2);
}

void STMGPU::writeData(uint8_t d)
{
  cmdBuffer.cmd = WRT_DATA;
  cmdBuffer.par1 = d;
  
  sendCommand(cmdBuffer.data, 2);
}

void STMGPU::writeWordData(uint16_t c)
{
  cmdBuffer.cmd = WRT_DATA_U16;
  cmdBuffer.par1 = c;
  
  sendCommand(cmdBuffer.data, 3);
}
  
// --------------- Tile/Sprite -------------- //
void STMGPU::SDLoadTileFromSet8x8(const char *tileSetArrName, uint8_t tileSetW,
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
  
void STMGPU::SDLoadTileSet8x8(const char *tileSetArrName, uint8_t tileSetW,
                              uint8_t ramTileBase, uint8_t tileMax)
{
  cmdBuffer.cmd = LDD_TLES_8;
  cmdBuffer.data[1] = strlen(tileSetArrName);
  cmdBuffer.data[2] = tileSetW;
  cmdBuffer.data[3] = ramTileBase;
  cmdBuffer.data[4] = tileMax;
    
  sendCommand(cmdBuffer.data, 5);
  sendCommand((void*)tileSetArrName, cmdBuffer.data[1]); // send name of file
}
  
void STMGPU::SDLoadRegionOfTileSet8x8(const char *tileSetArrName, uint8_t tileSetW,
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
  cmdBuffer.cmd = DRW_TLE_8_POS;
  cmdBuffer.par1 = posX;
  cmdBuffer.par2 = posY;
  cmdBuffer.par3 = tileNum;
    
  sendCommand(cmdBuffer.data, 7);
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
