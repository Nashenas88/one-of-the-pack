#include "beam.h"
#include "defines.h"

Beam::Beam(void)
:Special()
{}

Beam::Beam(float x, float y, int map_x, int map_y, int num, int frames,
               Texture *tex, direc dir, int hs, FMOD_SYSTEM *sys,
               FMOD_SOUND *as, FMOD_CHANNEL *ac)
:Special(x, y, map_x, map_y, num, frames, 0, tex, dir, 0, hs, ENGINEER,
         sys, NULL, NULL, as, ac)
{}

void Beam::use_ability(Map *m)
{
  int hit[2];
  
  if (will_collide_tile(m, BREAKABLE, hit))
  {
    play_effect();
  }
}
