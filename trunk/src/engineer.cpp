#include "engineer.h"
#include "defines.h"
#include "beam.h"

Engineer::Engineer(void)
:Special()
{}

Engineer::Engineer(float x, float y, int map_x, int map_y, int num, int frames,
               int abil_frames, Texture *tex, direc dir, FMOD_SYSTEM *sys,
               FMOD_SOUND *music, FMOD_CHANNEL *ch, FMOD_SOUND *as,
               FMOD_CHANNEL *ac, FMOD_SOUND *as1)
:Special(x, y, map_x, map_y, num, frames, abil_frames, tex, dir, 0, 0, ENGINEER,
         sys, music, ch, as, ac)
{
  beam_sound = as1;
}

void Engineer::use_ability(Map *m)
{
  if (get_tex_num() != ABILITY)
  {
    set_tex_num(ABILITY);
    set_cur_frame(1);
  }
}

void Engineer::enable_ability(Map *m, vector<Beam*> beams)
{
  play_effect();

  FMOD_CHANNEL *a_channel = 0;

  if (getDirection() == RIGHT)
  {
    beams.push_back(new Beam(get_x()+TILE_WIDTH, get_y(), 0, 0, BEAM_ANIM,
               BEAM_NUM, get_texture(), RIGHT, BEAM_SPEED, get_system(),
               beam_sound, a_channel));
  }
  else if (getDirection() == LEFT)
  {
    beams.push_back(new Beam(get_x()-TILE_WIDTH, get_y(), 0, 0, BEAM_ANIM,
               BEAM_NUM, get_texture(), LEFT, -BEAM_SPEED, get_system(),
               beam_sound, a_channel));
  }
}
