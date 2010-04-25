#ifndef FLOCK__AHNOLD__H
#define FLOCK__AHNOLD__H

#include "special.h"
#include "defines.h"

class Ahnold : public Special
{  
public:
  Ahnold(void);
  Ahnold(float x, float y, int map_x, int map_y, int num, int frames,
         int abil_frames, Texture *tex, direc dir, FMOD_SYSTEM *sys,
         FMOD_SOUND *music, FMOD_CHANNEL *ch, FMOD_SOUND *as, FMOD_CHANNEL *ac);
  
  void use_ability(Map *m);
  void enable_ability(Map *m);
};

#endif // FLOCK__AHNOLD__H
