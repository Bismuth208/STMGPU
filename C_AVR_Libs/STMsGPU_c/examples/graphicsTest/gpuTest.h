#include <avr/pgmspace.h>

const uint8_t Loremipsum[] PROGMEM = "\
Lorem ipsum dolor sit amet, consectetur adipiscing elit. \
Curabitur adipiscing ante sed nibh tincidunt feugiat. \
Maecenas enim massa, fringilla sed malesuada et, malesuada sit amet turpis. \
Sed porttitor neque ut ante pretium vitae malesuada nunc bibendum. \
Nullam aliquet ultrices massa eu hendrerit. Ut sed nisi lorem. \
In vestibulum purus a tortor imperdiet posuere.\n\n";

const uint8_t textSize2[] PROGMEM = "\
Text size 2\n";

const uint8_t textSize3[] PROGMEM = "\
Even bigger 3\n";

const uint8_t textSize4[] PROGMEM = "\
Seruious txt\n";


void testdrawtext(void);
void testlines(void);
void testfastlines(void);
void testdrawrects(void);
void testfillrects(void);
void testfillcircles(void);
void testroundrects(void);
void testtriangles(void);

void drawRandPixels(void);
void drawRandLines(void);
void drawRandRect(void);
void drawRandFillRect(void);
void drawRandTriangles(void);
void drawRandRoundRect(void);
void drawRandRoundFillRect(void);
void drawRandCircle(void);
void drawRandFillCircle(void);
void matrixScreen(void);

void (*pArrExecGFXFunc[])(void) = {
    testdrawtext,
    testlines,
    testfastlines,
    //testdrawrects,
    testfillrects,
    testfillcircles,
    testroundrects,
    testtriangles,

    drawRandPixels,
    drawRandLines,
    drawRandRect,
    drawRandFillRect,
    //drawRandTriangles,
    //drawRandRoundRect,
    //drawRandRoundFillRect,
    drawRandCircle,
    drawRandFillCircle,
    matrixScreen
  };

  #define FUNC_TO_TEST_COUNT (sizeof(pArrExecGFXFunc)/sizeof(pArrExecGFXFunc[0]))
