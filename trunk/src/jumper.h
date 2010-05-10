#ifndef FLOCK__JUMPER__H
#define FLOCK__JUMPER__H

#include "special.h"
#include "defines.h"

class Jumper : public Special
{
public:
  Jumper(void);
  Jumper(float x, float y, int map_x, int map_y, int num, int frames,
         int abil_frames, Texture *tex, direc dir, FMOD_SYSTEM *sys,
         FMOD_SOUND *music, FMOD_CHANNEL *ch, FMOD_SOUND *as, FMOD_CHANNEL *ac,
         FMOD_SOUND *cs);
  
  void use_ability(Map *m);
};

#endif // FLOCK__JUMPER__H
