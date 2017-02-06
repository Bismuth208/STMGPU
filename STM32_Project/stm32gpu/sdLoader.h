#ifndef _SDLOADER_H
#define _SDLOADER_H


#define T_MOUNT_SD              "Mount sd..."
#define T_OK                    "ok.\n"
#define T_FAIL                  "fail.\n"

#define T_BMP_SET_EXT_NAME      ".bmp"  // 0x00
#define T_PAL_SET_EXT_NAME      ".pal"  // 0x01
#define T_MAP_SET_EXT_NAME      ".map"  // 0x03
#define T_TLE_SET_EXT_NAME      ".tle"  // 0x02

typedef struct  {
  uint8_t tileSetW;
  uint8_t ramTileBase;
  uint8_t tileMin;
  uint8_t tileMax;
} tileParam_t;

void init_sdCard(void);

void SDLoadTile8x8(void *fileName, uint8_t *pData);
void SDLoadTileSet8x8(void *fileName, uint8_t *pData);

void SDLoadTile16x16(void *fileName, uint8_t *pData);
void SDLoadTileSet16x16(void *fileName, uint8_t *pData);

#ifdef STM32F10X_HD
void SDLoadTile32x32(void *fileName, uint8_t *pData);
void SDLoadTileSet32x32(void *fileName, uint8_t *pData);
#endif

void SDLoadTileMap(void *fileName);
void SDLoadPalette(void *fileName);
void SDPrintBMP(uint16_t x, uint16_t y, void *fileName);

#endif /* _SDLOADER_H */