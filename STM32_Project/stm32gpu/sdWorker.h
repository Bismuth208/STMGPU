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

void SDLoadTileFromSet8x8(uint8_t *tileSetArrName, uint8_t tileSetW, uint8_t ramTileNum, uint8_t tileNum);
//void SDLoadTileFromSet8x8(uint8_t *tileSetArrName, tileParam_t *params);
void SDLoadTileSet8x8(uint8_t *tileSetArrName, uint8_t tileSetW, uint8_t ramTileBase, uint8_t tileMax);
//void SDLoadTileSet8x8(uint8_t *tileSetArrName, tileParam_t *params);
void SDLoadRegionOfTileSet8x8(uint8_t *tileSetArrName, uint8_t tileSetW, uint8_t ramTileBase, uint8_t tileMin, uint8_t tileMax);
//void SDLoadRegionOfTileSet8x8(uint8_t *tileSetArrName, tileParam_t *params);

void SDLoadTileMap(uint8_t *tileMapArrName);

#endif /* _SDWORKER_H */