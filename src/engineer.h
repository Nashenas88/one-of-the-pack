#ifndef FLOCK__ENGINEER__H
#define FLOCK__ENGINEER__H

#include "special.h"
#include "defines.h"
#include "beam.h"

class Engineer : public Special
{  
private:
  FMOD_SOUND *beam_sound;
public:
  Engineer(void);
  Engineer(float x, float y, int map_x, int map_y, int num, int frames,
         int abil_frames, Texture *tex, direc dir, FMOD_SYSTEM *sys,
         FMOD_SOUND *music, FMOD_CHANNEL *ch, FMOD_SOUND *as, FMOD_CHANNEL *ac, 
         FMOD_SOUND *as1);
  
  void use_ability(Map *m);
  Beam *enable_ability(Map *m);
};

#endif // FLOCK__ENGINEER__H
