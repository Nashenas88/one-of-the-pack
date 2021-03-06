#include "beam.h"
#include "defines.h"

Beam::Beam(void)
:Special()
{}

Beam::Beam(float x, float y, int num, int frames,
               Texture *tex, direc dir, int hs, FMOD_SYSTEM *sys,
               FMOD_SOUND *as, FMOD_CHANNEL *ac, special_type special,
               FMOD_SOUND *cs)
:Special(x, y, 0, 0, num, frames, 0, tex, dir, 0, hs, special,
         sys, NULL, NULL, as, ac, cs)
{}

void Beam::use_ability(Map *m)
{
  int hit[2];
  
  if (will_collide_tile(m, BREAKABLE1, hit) ||
      will_collide_tile(m, BREAKABLE2, hit) ||
      will_collide_tile(m, BREAKABLE3, hit))
  {
    play_effect();
  }
}
