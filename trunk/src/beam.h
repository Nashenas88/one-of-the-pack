#ifndef FLOCK__BEAM__H
#define FLOCK__BEAM__H

#include "special.h"
#include "defines.h"

class Beam : public Special
{  
public:
  Beam(void);
  Beam(float x, float y, int num, int frames,
         Texture *tex, direc dir, int hs, FMOD_SYSTEM *sys,
         FMOD_SOUND *as, FMOD_CHANNEL *ac, special_type special);
  
  void use_ability(Map *m);
};

#endif // FLOCK__BEAM__H
