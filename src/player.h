#ifndef FLOCK__PLAYER__H
#define FLOCK__PLAYER__H

#include "character.h"
#include "defines.h"

enum Player_Dir_Text { PLAYER_RIGHT = 1, PLAYER_LEFT };

class Player : public Character
{
private:
  bool jumping;
  bool follow;
  int loc[2];
  
public:
  Player(void);
  Player(float x, float y, int num, int frames, Texture *tex, direc dir,
         bool jump, FMOD_SYSTEM *sys, FMOD_SOUND *so, FMOD_CHANNEL *ch);
  ~Player(void);
  
  bool should_follow(void) {return follow;}
  void switch_follow(void) {follow = !follow;}
  void pickupItem(int item);
  void set_checkpoint(int x, int y);
  void reset(Map *m);
};

#endif // FLOCK__PLAYER__H
