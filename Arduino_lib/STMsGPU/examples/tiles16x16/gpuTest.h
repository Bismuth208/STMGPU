#define TFT_W gpu.width()
#define TFT_H gpu.height()

#define TEST_SAMPLE_SIZE    12000
#define TEST_SAMPLE_SCREENS 80

#define MAX_TILES 10
#define RAM_BASE 0
#define TLE_START 0
#define TILE_SET_W 10
#define TILE_SET_H 7

#define TILE_SHOW_W 3
#define TILE_SHOW_H 3

#define TLE_16X16_SIZE 16 // this is single tile size, each tile 16x16 in px

void testDrawTiles(void);
void drawRamTileSet16x16(void);
void fillScreenByTiles(void);

// this is pointers to test functions
void (*pArrExecGFXFunc[])(void) = {
    testDrawTiles,
    drawRamTileSet16x16,
    fillScreenByTiles,
  };

#define FUNC_TO_TEST_COUNT (sizeof(pArrExecGFXFunc)/sizeof(pArrExecGFXFunc[0]))
