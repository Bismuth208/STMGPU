#include <avr/pgmspace.h>

#define FUNC_TO_TEST_COUNT (sizeof(pArrExecGFXFunc)/sizeof(pArrExecGFXFunc[0]))

#define MAX_TILES 70
#define RAM_BASE 0
#define TLE_START 0
#define TILE_SET_W 10 // real number (1-10)
#define TILE_SET_H 7  // real number (1-8)

#define TLE_8X8_SIZE 8 // this is single tile size, each tile 8x8 in px

// sGPU add *.tle extension automatically
const uint8_t tileFileName[] = "pcs8x8"; // PROGMEM

void testDrawTiles(void);
void drawRamTileSet8x8(void);
void fillScreenByTiles(void);
