#include "paris.h"
#include "defines.h"
#include "beam.h"

Paris::Paris(void)
:Special()
{}

Paris::Paris(float x, float y, int map_x, int map_y, int num, int frames,
               int abil_frames, Texture *tex, direc dir, FMOD_SYSTEM *sys,
               FMOD_SOUND *music, FMOD_CHANNEL *ch, FMOD_SOUND *as,
               FMOD_CHANNEL *ac, FMOD_SOUND *as1, FMOD_SOUND *cs)
:Special(x, y, map_x, map_y, num, frames, abil_frames, tex, dir, 0, 0, PARIS,
         sys, music, ch, as, ac, cs), kiss_sound(as1)
{}

void Paris::use_ability(Map *m)
{
  if (get_tex_num() != ABILITY)
  {
    set_tex_num(ABILITY);
    set_cur_frame(1);
  }
}

Beam *Paris::enable_ability(Map *m)
{
  play_effect();

  FMOD_CHANNEL *a_channel = 0;
  
  if (getDirection() == RIGHT)
  {
    return new Beam(get_x() + TILE_WIDTH / 2, get_y(), BEAM_NUM, PARIS_BEAM_ANIM,
                    get_texture(), RIGHT, BEAM_SPEED, get_system(),
                    kiss_sound, a_channel, (special_type)get_type(),
                    kiss_sound);
  }
  else if (getDirection() == LEFT)
  {
    return new Beam(get_x() - TILE_WIDTH / 2, get_y(), BEAM_NUM, PARIS_BEAM_ANIM,
                    get_texture(), LEFT, -BEAM_SPEED, get_system(),
                    kiss_sound, a_channel, (special_type)get_type(),
                    kiss_sound);
  }
  return NULL;
}
