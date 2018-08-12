#ifndef _TILES_H
#define _TILES_H

// -------------------------------------------------------- //
// in px
#define TILE_8_BASE_SIZE          8
#define TILE_16_BASE_SIZE         16
#define TILE_32_BASE_SIZE         32

#define TILE_ARR_8X8_SIZE       64
#define TILE_ARR_16X16_SIZE     256
#define TILE_ARR_32X32_SIZE     1024

#ifdef STM32F10X_MD // if STM32F103C8T6 20k RAM
 #define TILES_NUM_8x8          80
 #define TILES_NUM_16x16        10
#endif /* STM32F10X_MD */

#ifdef STM32F10X_HD // if STM32F103VET6 64k RAM
 #define TILES_NUM_8x8          160
 #define TILES_NUM_16x16        80
 #define TILES_NUM_32x32        10
#endif /* STM32F10X_HD */

#ifdef STM32F40XX // if STM32F407VET6 128k RAM
 #define TILES_NUM_8x8          192
 #define TILES_NUM_16x16        128
 #define TILES_NUM_32x32        48
#endif /* STM32F40XX */

#define BACKGROUND_SIZE_W       40
#define BACKGROUND_SIZE_H       30 

#define BACKGROUND_SIZE         1200

#define USER_PALETTE_SIZE       256 // 256 colors


#define TILE_8x8_WINDOW_SIZE       7
#define TILE_16x16_WINDOW_SIZE    15
#define TILE_32x32_WINDOW_SIZE    31
// -------------------------------------------------------- //


extern uint16_t currentPaletteArr[USER_PALETTE_SIZE];
extern uint8_t palChanged;

// -------------------------------------------------------- //

// for internal use only
void initRaycasterPointers(void);
void loadDefaultPalette(void);
void loadTile8x8(uint8_t tileNum, uint8_t tileSetW, uint8_t *pTile, const uint8_t *pTileSet);
void loadInternalTileSet(uint8_t tileSetSize, uint8_t tileSetW, const uint8_t *pTileSet);

uint8_t *getArrTilePointer8x8(uint8_t tileNum);
uint8_t *getArrTilePointer16x16(uint8_t tileNum);
uint8_t *getArrTilePointer32x32(uint8_t tileNum);

uint8_t *getMapArrPointer(void);

// draw tiles located in RAM
void drawTile8x8(void *tileData);
void drawTile16x16(void *tileData);
void drawTile32x32(void *tileData);

void drawBackgroundMap(void);

// -------------------------------------------------------- //


#endif /* _TILES_H */
