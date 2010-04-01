#ifndef FLOCK__AHNOLD__H
#define FLOCK__AHNOLD__H

#include "special.h"
#include "defines.h"

class Ahnold : public Special
{
private:
  float loc[2];
  bool following;
  FMOD_SOUND *sfx;
  
public:
  Ahnold(void);
  Ahnold(float x, float y, int num, int frames, int abil_frames, Texture *tex,
         direc dir, FMOD_SYSTEM *sys, FMOD_SOUND *music, FMOD_CHANNEL *ch);
  
  void use_ability(int dir, Map *m);
  void start_following(Player *p);
  void stop_following(Player *p);
  void go_home();
  void move(int x, int y);
};

#endif // FLOCK__AHNOLD__H
