#ifndef _SDLOADER_H
#define _SDLOADER_H


#define T_MOUNT_SD              "Mount sd..."
#define T_OK                    "ok.\n"
#define T_FAIL                  "fail.\n"

#define T_BMP_SET_EXT_NAME      ".bmp"  // 0x00
#define T_PAL_SET_EXT_NAME      ".pal"  // 0x01
#define T_MAP_SET_EXT_NAME      ".map"  // 0x03
#define T_TLE_SET_EXT_NAME      ".tle"  // 0x02
#define T_CSV_SET_EXT_NAME      ".csv"  // 0x04

typedef struct  {
  uint8_t tileSetW;
  uint8_t ramTileBase;
  uint8_t tileMin;
  uint8_t tileMax;
} tileParam_t;

// a pointer to function returning pointer to tile array uint8_t*, 
// and accepted uint8_t as nuber of tile array
typedef uint8_t *(*pfGetFunc_t)(uint8_t);

void init_sdCard(void);

void SDLoadTile(void *fileName, uint8_t *pData, uint16_t tleBaseSize, uint16_t tlesNumSize);
void SDLoadTileSet(void *fileName, uint8_t *pData, uint16_t tleBaseSize, uint16_t tlesNumSize);

void SDLoadTileMap(void *fileName);
void SDLoadPalette(void *fileName);
void SDPrintBMP(uint16_t x, uint16_t y, void *fileName);

#endif /* _SDLOADER_H */
