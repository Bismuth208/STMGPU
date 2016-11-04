#ifndef _SPRITES_H
#define _SPRITES_H

#ifdef __cplusplus
extern "C" {
#endif
  
  // ----------------- Sprite ----------------- //
  void setSpritePosition(uint8_t sprNum, uint16_t posX, uint16_t posY);
  void setSpriteType(uint8_t sprNum, uint8_t type);
  void setSpriteVisible(uint8_t sprNum, uint8_t state);
  void setSpriteTiles(uint8_t sprNum, uint8_t tle1, uint8_t tle2, uint8_t tle3, uint8_t tle4);
  void setSpritesAutoRedraw(uint8_t state);
  void drawSprite(uint8_t sprNum);
  
  bool getSpriteCollision(uint8_t sprNum1, uint8_t sprNum2);
  
#ifdef __cplusplus
}
#endif

#endif /* _SPRITES_H */
