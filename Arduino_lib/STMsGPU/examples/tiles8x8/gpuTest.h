#define MAX_SPEED 1 

// can save ROM and increase speed, but its unfair!
// use it if you know end resolution
#if MAX_SPEED
 #define TFT_W 320
 #define TFT_H 240
#else
 #define TFT_W gpu.width()
 #define TFT_H gpu.height()
#endif /* MAX_SPEED */

#define TEST_SAMPLE_SIZE    12000
#define TEST_SAMPLE_SCREENS 20 // this is equal to 24000 tiles

// this is need to load textures from *.tle file 
// located on SD card - correctly
#define MAX_TILES 70
#define RAM_BASE 0
#define TLE_START 0
#define TILE_SET_W 10 // this is width of tileSet in tiles ( one tile width == 8 pixels)
#define TILE_SET_H 7  // real number (1-8)

#define TLE_8X8_SIZE 8 // this is single tile size, each tile 8x8 in px

void testDrawTiles(void);
void drawRamTileSet8x8(void);
void fillScreenByTiles(void);

// this is pointers to test functions
void (*pArrTestFunc[])(void) = {
    testDrawTiles,
    drawRamTileSet8x8,
    fillScreenByTiles,
  };

#define FUNC_TO_TEST_COUNT (sizeof(pArrTestFunc)/sizeof(pArrTestFunc[0]))
