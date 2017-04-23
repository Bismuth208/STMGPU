/**
* LCDWolf3D
* jaburns (Jeremy A Burns' LCD Raycaster)
* https://www.youtube.com/watch?v=GlCye0TyHrE
* https://github.com/jaburns/lcd-wolf3d  
* http://gendev.spritesmind.net/forum/viewtopic.php?t=25&start=75
*/

#include <stdbool.h>
#include <string.h>
#include <math.h>
//#include "trigonometry.h"

#include <stm32f10x.h>
#include <gfx.h>
#if USE_FSMC
 #include <fsmcdrv.h>
#else
 #include <spi.h>
#endif

#include "raycast.h"

// ---------------------------------- //
float px =  50.0f;
float py = 150.0f;
float pa = -PI_1_2_NUM;
// ---------------------------------- //

// buffer for single vertical line
// using it whith DMA dramatically increase perfomance
uint16_t sliceDMABuf[RENDER_W_HEIGHT];

// step for render quality mode
// show how much vertical lines to skip
uint8_t rndrQAModeStep = RENDER_STEP_SIZE;

// ponter to current palette array in gpuTiles.c
uint16_t *pCurrenPal;

// ponter to level map;
// but to reduce momory usage it actually
// points to mainBackGround array in gpuTiles.c
uint8_t *pLevel;

// in texture mode == 0 draw textures 8x8px
// in texture mode == 1 draw textures 16x16px
// in texture mode == 2 draw textures 32x32px
//uint8_t curTextureModeQA = 1;

// ponters to tileArrYxY array in gpuTiles.c
uint8_t *pTileArray8;
uint8_t *pTileArray16;
uint8_t *pTileArray32;
// base pointer, to all arrays 8px, 16px, 32px; 
// ponter destination is selected by current texture quality mode
uint8_t *pTileArray;


bool yAxisWall = false;
bool checkWallCollision = true;

// setup default values for 16px tiles
float textureSizeOffset = RCE_TEXTURE_SIZE_OFF_16;
uint8_t textureSize = RCE_TEXTURE_SIZE_16;
uint16_t textureTileSize = RCE_TLE_16_SIZE;

// in future will be replaced by texture id
uint16_t skyColor = COLOR_DARKCYAN;
uint16_t floorColor = COLOR_DARKGREEN;
// ---------------------------------- //

// 24x24
const uint8_t level[] = {
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,2,2,4,4,2,3,2,2,1,2,2,2,4,4,1,2,4,2,2,3,3,3,1,
  1,2,0,0,0,0,0,0,0,0,0,2,2,0,0,0,0,0,0,0,0,0,2,1,
  1,3,0,0,0,0,0,0,0,0,0,3,2,0,0,0,0,0,0,0,0,0,4,1,
  1,2,0,0,0,3,2,2,2,0,0,2,2,0,0,5,0,2,0,4,0,0,2,1,
  1,2,0,0,0,2,2,0,0,0,0,2,2,0,0,0,0,0,0,0,0,0,2,1,
  1,3,0,0,0,3,3,0,0,0,0,3,3,0,0,4,0,0,0,4,0,0,2,1,
  1,2,2,0,2,2,2,0,0,0,0,2,2,0,0,0,0,4,4,4,0,0,2,1,
  1,0,2,3,2,0,2,2,0,2,2,2,2,4,2,2,4,0,0,0,2,2,2,1,
  1,0,0,0,0,0,0,4,0,4,4,4,4,4,4,4,0,0,0,0,0,4,4,1,
  1,0,0,0,0,0,0,4,0,0,0,0,0,0,4,0,0,0,0,0,0,0,4,1,
  1,0,0,0,0,0,0,4,0,0,0,0,0,0,4,0,0,0,5,0,0,0,4,1,
  1,0,0,0,0,0,0,4,0,4,4,4,0,0,0,0,0,0,0,0,0,0,4,1,
  1,0,0,0,0,0,0,4,3,4,4,4,4,4,4,4,0,0,0,0,0,4,4,1,
  1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,4,0,0,0,4,4,4,1,
  1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,0,4,0,0,0,1,
  1,2,2,4,4,1,4,4,2,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,
  1,4,0,4,0,0,0,0,1,0,0,0,0,0,0,0,1,3,0,1,0,0,0,1,
  1,4,0,0,0,0,5,0,2,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,
  1,4,0,4,0,0,0,0,2,0,0,0,0,0,0,0,0,1,1,0,0,0,0,1,
  1,4,0,2,4,2,4,4,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
  1,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
  1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
};

// -------------------------------------------------------------------- //


void castRay(float x, float y, float angle, uint16_t slicePosX, slice_t *result)
{
  // origin coordinates
  float origX = x;
  float origY = y;
  
  // calculate FOV
  float offsetAngle = ((float)slicePosX * (PI_1_3_NUM)) / RENDER_W_WIDTH - (PI_1_6_NUM);
  angle += offsetAngle;
  
  // coordinate of the texture block we're sitting in
  float blockX = (float)((uint16_t)(x / BLOCK_SIZE));
  float blockY = (float)((uint16_t)(y / BLOCK_SIZE));
  uint8_t textureId =0;
  
  float cosine = cos(angle);
  float sine   = sin(angle);
  
  // check for minimum and maximum; we're not need so much precision
  if((sine   <  MIN_SINE_VAL) && (sine   >= 0.0f)) sine   =  MIN_SINE_VAL;
  if((sine   > -MIN_SINE_VAL) && (sine   <  0.0f)) sine   = -MIN_SINE_VAL;
  if((cosine <  MIN_SINE_VAL) && (cosine >= 0.0f)) cosine =  MIN_SINE_VAL;
  if((cosine > -MIN_SINE_VAL) && (cosine <  0.0f)) cosine = -MIN_SINE_VAL;
  
  float oneOverCosine = 1.0f / cosine;
  float oneOverSine   = 1.0f / sine;
  
  float xNext_x, xNext_y, xNext_l;
  float yNext_x, yNext_y, yNext_l;
  
  float stepX, stepY;
  stepX = cosine > 0 ? 1.0f : -1.0f;
  stepY = sine > 0 ? 1.0f : -1.0f;
  
  //perform DDA
  do {
    xNext_x = blockX * BLOCK_SIZE_F;
    if( cosine > 0 ) xNext_x += BLOCK_SIZE_F;
    xNext_x -= x;
    xNext_l = xNext_x * oneOverCosine;
    
    yNext_y = blockY * BLOCK_SIZE_F;
    if( sine > 0 ) yNext_y += BLOCK_SIZE_F;
    yNext_y -= y;
    yNext_l = yNext_y * oneOverSine;
    
    if( xNext_l < yNext_l ) {
      yAxisWall = true;
      xNext_y = xNext_l * sine;
      blockX += stepX;
      x += xNext_x;
      y += xNext_y;
    } else {
      yAxisWall = false;
      yNext_x = yNext_l * cosine;
      blockY += stepY;
      x += yNext_x;
      y += yNext_y;
    }
    
    //textureId =  level[ (uint16_t)(blockX + MAP_WIDTH * blockY)];
    textureId =  pLevel[ (uint16_t)(blockX + MAP_WIDTH * blockY)];
  } while(textureId == 0);
  
  // -1 mean normal number in tile RAM, in RAM tiles are located from 0
  // i.e. 0(no texture) not allowed
  result->textureId = textureId -1;
  result->targetBlockX = (uint16_t)blockX;
  result->targetBlockY = (uint16_t)blockY;
  
  if(yAxisWall) {
    result->textureOffset = (uint16_t)( (uint16_t)(y * textureSizeOffset) % textureSize);
  } else {
    result->textureOffset = (uint16_t)( (uint16_t)(x * textureSizeOffset) % textureSize);
  }
  
  // calculate wall projection plane height
  float dx = x - origX;
  float dy = y - origY;
  float dd = sqrt(dx*dx + dy*dy) * cos(offsetAngle);
  
  result->sliceHeight = (uint16_t)(VISPLANEDIST_TIMES_WALLHEIGHT/dd);
}

void drawSlice( uint16_t screenX, slice_t *slice )
{
  uint8_t  colorId =0;
  uint16_t sliceYcount =0;
  uint16_t textureYOffset =0;
  uint16_t sliceHeight = slice->sliceHeight;
  
  uint16_t offsetY   = ( sliceHeight >  RENDER_W_HEIGHT) ? (0) : ((RENDER_W_HEIGHT/2) - (sliceHeight >> 1));
  uint16_t overflowY = ( sliceHeight <= RENDER_W_HEIGHT) ? (0) : ((sliceHeight - RENDER_W_HEIGHT) >> 1);
 
  // precalculate base texture offset 
  uint16_t textureIdSizeOffset = (slice->textureId*textureTileSize + slice->textureOffset*textureSize);
  
  // this is much fster than clear whole screen
  memset(sliceDMABuf, 0x00, RENDER_W_HEIGHT*2);
  
  for(; (sliceYcount < sliceHeight) && (sliceYcount < RENDER_W_HEIGHT); sliceYcount++) {
    
    textureYOffset = ((sliceYcount + overflowY) * textureSize) / sliceHeight;
    colorId = pTileArray[textureIdSizeOffset + textureYOffset];
    sliceDMABuf[sliceYcount+offsetY] = yAxisWall ? SHADOW_Y_SIDE(pCurrenPal[colorId]) : pCurrenPal[colorId];
  }
  
  setVAddrWindow(OFFSET_X+screenX, OFFSET_Y, OFFSET_Y+RENDER_W_HEIGHT);
  SEND_ARR16_FAST(sliceDMABuf, RENDER_W_HEIGHT);
}

bool shouldInterpolate(slice_t *sliceA, slice_t *sliceB)
{
  if(sliceA->textureId == sliceB->textureId) {
    uint16_t xDiff = UNSIGNED_DIFF(sliceA->targetBlockX, sliceB->targetBlockX);
    uint16_t yDiff = UNSIGNED_DIFF(sliceA->targetBlockY, sliceB->targetBlockY);
    
    if(xDiff == 0 && yDiff <= 1) return true;
    if(xDiff <= 1 && yDiff == 0) return true;
  }
 
  return false;
}

void applyMove(float dx, float dy)
{
  px += dx;
  py += dy;
  
  if(checkWallCollision) {
    // Collision detection.  Still glitchy.
    uint16_t testA = (uint16_t)( ( px - HIT_WIDTH ) / BLOCK_SIZE_F );
    uint16_t testB = (uint16_t)( ( py - HIT_WIDTH ) / BLOCK_SIZE_F );
    uint16_t testC = (uint16_t)( ( py + HIT_WIDTH ) / BLOCK_SIZE_F );
    if( ( pLevel[ testA + MAP_WIDTH * testB ] ) || ( pLevel[ testA + MAP_WIDTH * testC ] ) ) {
      px = (float)( testA + 1 ) * BLOCK_SIZE_F + HIT_WIDTH;
    }
    
    testA = (uint16_t)( ( px + HIT_WIDTH ) / BLOCK_SIZE_F );
    if( ( pLevel[ testA + MAP_WIDTH * testB ] ) || ( pLevel[ testA + MAP_WIDTH * testC ] ) ) {
      px = (float)( testA ) * BLOCK_SIZE_F - HIT_WIDTH;
    }
    
    
    testA = (uint16_t)( ( py - HIT_WIDTH ) / BLOCK_SIZE_F );
    testB = (uint16_t)( ( px - HIT_WIDTH ) / BLOCK_SIZE_F );
    testC = (uint16_t)( ( px + HIT_WIDTH ) / BLOCK_SIZE_F );
    if( ( pLevel[ testB + MAP_WIDTH * testA ] ) || ( pLevel[ testC + MAP_WIDTH * testA ] ) ) {
      py = (float)( testA + 1 ) * BLOCK_SIZE_F + HIT_WIDTH;
    }
    
    testA = (uint16_t)( ( py + HIT_WIDTH ) / BLOCK_SIZE_F );
    if( ( pLevel[ testB + MAP_WIDTH * testA ] ) || ( pLevel[ testC + MAP_WIDTH * testA ] ) ) {
      py = (float)( testA ) * BLOCK_SIZE_F - HIT_WIDTH;
    }
  }
}

// this is need because this driver is separatted
// from other sources
void setRayCastPalette(uint16_t *pPal)
{
  // set pointer to color palette
  pCurrenPal = pPal;
}

void setLevelMap(uint8_t *pLevelMap)
{
  pLevel = pLevelMap;
  //memcpy(pLevelMap, level, 576 /*24*24*/);
}

void setTileArrayPonter(uint8_t *pTleArrayRAM, uint8_t type)
{
  switch(type)
  {
  case 1: pTileArray8=pTleArrayRAM; break;
  case 2: pTileArray16=pTleArrayRAM; break;
  case 3: pTileArray32=pTleArrayRAM; break;
  }
  
  pTileArray = pTleArrayRAM;
}

// ----------------------- public ----------------------- //
void renderWalls(void)
{  
  slice_t sliceA, sliceB;
  slice_t sliceX[3];
  
  castRay(px, py, pa, 0, &sliceA);
  
  for(uint16_t x = 1+rndrQAModeStep; x < RENDER_W_WIDTH; x += rndrQAModeStep) {
    castRay(px, py, pa, x, &sliceB);
    
    if(shouldInterpolate(&sliceA , &sliceB)) {
      sliceX[1].textureId     =  sliceA.textureId;
      sliceX[1].textureOffset = (sliceA.textureOffset + sliceB.textureOffset) >> 1;
      sliceX[1].sliceHeight   = (sliceA.sliceHeight   + sliceB.sliceHeight  ) >> 1;
    } else {
      castRay(px, py, pa, x - 4, &sliceX[1]);
    }
    
    sliceX[0].textureId     =  sliceA.textureId;
    sliceX[0].textureOffset = (sliceA.textureOffset + sliceX[1].textureOffset) >> 1;
    sliceX[0].sliceHeight   = (sliceA.sliceHeight   + sliceX[1].sliceHeight  ) >> 1;
    
    sliceX[2].textureId     =  sliceX[1].textureId;
    sliceX[2].textureOffset = (sliceX[1].textureOffset + sliceB.textureOffset) >> 1;
    sliceX[2].sliceHeight   = (sliceX[1].sliceHeight   + sliceB.sliceHeight  ) >> 1;
    
    drawSlice(x-3, &sliceX[0]);
    drawSlice(x-2, &sliceX[1]);
    drawSlice(x-1, &sliceX[2]);
    drawSlice(x,   &sliceB   );
    
    sliceA = sliceB;
  }
}

void setCameraPosition(int16_t posX, int16_t posY, int16_t angle)
{
  px = (float)posX;
  py = (float)posY;
  pa = (float)angle;
}

void getCamPosition(void *pBuf)
{
  uint16_t *pBufArr = (uint16_t*)pBuf;
  pBufArr[0] = (uint16_t)px;
  pBufArr[1] = (uint16_t)py;
  pBufArr[2] = (uint16_t)pa;
}

void moveCamera(uint8_t direction)
{
  if((direction & MOVE_LEFT) == MOVE_LEFT) {
    applyMove(MOVE_SPEED*cos(pa - PI_1_2_NUM), MOVE_SPEED*sin(pa - PI_1_2_NUM));
  }
  
  if((direction & MOVE_RIGHT) == MOVE_RIGHT) {
    applyMove(-MOVE_SPEED*cos(pa - PI_1_2_NUM), -MOVE_SPEED*sin(pa - PI_1_2_NUM));
  }
  
  if((direction & MOVE_UP) == MOVE_UP) {
    applyMove(MOVE_SPEED*cos(pa), MOVE_SPEED*sin(pa));
  }
  
  if((direction & MOVE_DOWN) == MOVE_DOWN) {
    applyMove(-MOVE_SPEED*cos(pa), -MOVE_SPEED*sin(pa));
  }
  
  if((direction & MOVE_CLOCKWISE_R) == MOVE_CLOCKWISE_R) {
    pa -= ROT_ANGLE_STEP;
  }
  
  if((direction &  MOVE_CLOCKWISE_L) == MOVE_CLOCKWISE_L) {
    pa += ROT_ANGLE_STEP;
  }
}

void serRenderQuality(int8_t quality)
{
  if((quality >= RENDER_QA_MIN) || (quality <= RENDER_QA_MAX)) {
    rndrQAModeStep = quality;
  }
}

void setTextureQuality(int8_t quality)
{
  if((quality >= TEXTURE_QA_MIN) || (quality <= TEXTURE_QA_MAX)) {
    switch(quality)
    {
    case TEXTURE_QA_0: { // 8x8px
      textureSizeOffset = RCE_TEXTURE_SIZE_OFF_8;
      textureSize = RCE_TEXTURE_SIZE_8;
      textureTileSize = RCE_TLE_8_SIZE;
      pTileArray = pTileArray8;
    } break;
    
    case TEXTURE_QA_1: { // 16x16px
      textureSizeOffset = RCE_TEXTURE_SIZE_OFF_16;
      textureSize = RCE_TEXTURE_SIZE_16;
      textureTileSize = RCE_TLE_16_SIZE;
      pTileArray = pTileArray16;
    } break;
    
    case TEXTURE_QA_2: { // 32x32px (pro version only!)
      textureSizeOffset = RCE_TEXTURE_SIZE_OFF_32;
      textureSize = RCE_TEXTURE_SIZE_32;
      textureTileSize = RCE_TLE_32_SIZE;
      pTileArray = pTileArray32;
    } break;
    }
    
    //curTextureModeQA = quality;
  }
}

void setWallCollision(bool state)
{
  checkWallCollision = state; 
}

// in future will be replaced by texture id;
// also it will be ranamed to setFloorSkyTexture
void setFloorSkyColor(uint16_t sky, uint16_t floor)
{
  skyColor = sky;
  floorColor = floor;
}
