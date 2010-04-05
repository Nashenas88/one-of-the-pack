#ifndef FLOCK__JUMPER__H
#define FLOCK__JUMPER__H

#include "special.h"
#include "defines.h"

class Jumper : public Special
{
private:
  float loc[2];
  bool following;
  
public:
  Jumper(void);
  Jumper(float x, float y, int num, int frames, int abil_frames, Texture *tex,
         direc dir, FMOD_SYSTEM *sys, FMOD_SOUND *music, FMOD_CHANNEL *ch,
         FMOD_SOUND *as, FMOD_CHANNEL *ac);
  
  void use_ability(Map *m);
  void start_following(Player *p);
  void stop_following(Player *p);
  void go_home();
  void move(int x, int y);
};

#endif // FLOCK__JUMPER__H
