#ifndef FLOCK__PLAYER__H
#define FLOCK__PLAYER__H

#include "character.h"
#include "defines.h"

enum Player_Dir_Text { PLAYER_RIGHT = 1, PLAYER_LEFT };

class Player : public Character
{
private:
  bool jumping;
  
public:
  Player(void);
  Player(float x, float y, int num, int frames, Texture *tex,
         direc dir, bool jump, int vs, int hs, FMOD_SYSTEM *sys,
         FMOD_SOUND *so, FMOD_CHANNEL *ch);
  
  void pickupItem(int item);
  
  void move(int x, int y);
};

#endif // FLOCK__PLAYER__H
