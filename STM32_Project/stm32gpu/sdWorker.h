#ifndef _SDWORKER_H
#define _SDWORKER_H


#define T_MOUNT_SD              "Mount sd... "
#define T_OK                    "ok.\n"
#define T_FAIL                  "fail.\n"
//#define T_TLE_SET_EXT_NAME      ".tle"
//define T_MAP_SET_EXT_NAME     ".map"

typedef union {
  uint8_t data[14];
  struct {
    uint16_t tileSetW;
    uint16_t ramTileBase;
    uint16_t par3;
    uint16_t par4;
  };
} tileParam_t;

void init_sdCard(void);

void SDLoadTile8x8(void *fileName, uint8_t *pData);
void SDLoadTileSet8x8(void *fileName, uint8_t *pData);

void SDLoadTileMap(void *fileName);
void SDLoadPalette(void *fileName);
void SDPrintBMP(uint16_t x, uint16_t y, void *fileName);

#endif /* _SDWORKER_H */