#define TFT_W gpu.width()
#define TFT_H gpu.height()

#define TEST_SAMPLE_SIZE    12000
#define TEST_SAMPLE_SCREENS 20 // this is equal to 24000 tiles

#define MAX_TILES 70
#define TILE_SET_W 10 // real number (1-10)
#define TILE_SET_H 7  // real number (1-8)

#define TLE_8X8_SIZE 8 // this is single tile size, each tile 8x8 in px

void testDrawTiles(void);
void drawRamTileSet8x8(void);
void fillScreenByTiles(void);

// this is pointers to test functions
void (*pArrExecGFXFunc[])(void) = {
    testDrawTiles,
    drawRamTileSet8x8,
    fillScreenByTiles,
  };

  #define FUNC_TO_TEST_COUNT (sizeof(pArrExecGFXFunc)/sizeof(pArrExecGFXFunc[0]))
