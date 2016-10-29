#include <avr/pgmspace.h>

#define FUNC_TO_TEST_COUNT (sizeof(pArrExecGFXFunc)/sizeof(pArrExecGFXFunc[0]))

const uint8_t tileFileName[] = "pcs8x8.tle"; // PROGMEM

void testDrawSprites(void);
