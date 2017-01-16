#ifndef _TILES_H
#define _TILES_H

#ifdef __cplusplus
extern "C" {
#endif

  // ------------------- Tile ----------------- //
  void SDLoadTile8x8(const char *tileSetArrName, uint8_t tileSetW,
                            uint8_t ramTileNum, uint8_t tileNum);
  void SDLoadTileSet8x8(const char *tileSetArrName, uint8_t tileSetW,
                                uint8_t ramTileBase, uint8_t tileMin, uint8_t tileMax);
  void drawTile8x8(int16_t posX, int16_t posY, uint8_t tileNum);
  
  //void SDLoadTile(const char *fileName, uint8_t fnNum, ...);
  
  void SDLoadTileMap(const char *fileName);
  void drawBackgroundMap(void);
  
#ifdef __cplusplus
}
#endif

#endif /* _TILES_H */
