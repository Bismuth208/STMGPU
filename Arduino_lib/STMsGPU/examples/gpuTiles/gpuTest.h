#include <avr/pgmspace.h>

const char tileFileName[] = "pcs8x8.tle"; // PROGMEM

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
