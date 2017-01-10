#ifndef _GPUTTILES_H
#define _GPUTTILES_H

// -------------------------------------------------------- //
#define TILE_BASE_SIZE          8       // in px

#define TILE_ARR_8X8_SIZE       64
#define TILE_ARR_16X16_SIZE     256
#define TILE_ARR_32X32_SIZE     1024

#ifdef STM32F10X_MD
#define TILES_NUM_8x8          70
#define TILES_NUM_16x16        10
#endif
#ifdef STM32F10X_HD
// if STM32F103VET6 64k RAM
#define TILES_NUM_8x8          160
#define TILES_NUM_16x16        80
#define TILES_NUM_32x32        10
#endif

#define BACKGROUND_SIZE_W       40
#define BACKGROUND_SIZE_H       30 

#define BACKGROUND_SIZE         1200

#define USER_PALETTE_SIZE       256 // 256 colors
// -------------------------------------------------------- //



typedef struct {
  uint8_t *pTilesIdArr;
  uint8_t tilesCount;
} tileGroup_t;

typedef struct {
  uint8_t  lasttileNum;          // last tile id in tileArrYxY
  uint8_t  drawed;               // this is problemfix, just left it as is.
  //uint16_t *pLastTileArr;       // pointer to last tile array
} lastTile_t;

extern uint16_t currentPaletteArr[USER_PALETTE_SIZE];

//extern uint8_t tileArr8x8[TILES_NUM_8x8][TILE_ARR_8X8_SIZE];
//extern uint8_t tileArr16x16[TILES_NUM_16x16][TILE_ARR_16X16_SIZE];

// -------------------------------------------------------- //

void loadDefaultPalette(void);
void loadTile8x8(uint8_t tileNum, uint8_t tileSetW, uint8_t *pTile, const uint8_t *pTileSet);
void loadLogoTileSet(uint8_t tileSetSize, uint8_t tileSetW, const uint8_t *pTileSet);

uint8_t *getArrTilePointer8x8(uint8_t tileNum);
uint8_t *getArrTilePointer16x16(uint8_t tileNum);

void loadTile8x8(uint8_t tileNum, uint8_t tileSetW, uint8_t *pTile, const uint8_t *pTileSet);

//void drawTile8x8(int16_t posX, int16_t posY, uint8_t tileNum);
void drawTile8x8(void *tile);
void drawTile16x16(int16_t posX, int16_t posY, uint8_t tileNum);

void drawBackgroundMap(void);

uint8_t *getMapArrPointer(void);
// -------------------------------------------------------- //


#endif /* _GPUTTILES_H */