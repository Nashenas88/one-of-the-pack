#include "ahnold.h"
#include "defines.h"

Ahnold::Ahnold(void)
:Special()
{}

Ahnold::Ahnold(float x, float y, int map_x, int map_y, int num, int frames,
               int abil_frames, Texture *tex, direc dir, FMOD_SYSTEM *sys,
               FMOD_SOUND *music, FMOD_CHANNEL *ch, FMOD_SOUND *as,
               FMOD_CHANNEL *ac)
:Special(x, y, map_x, map_y, num, frames, abil_frames, tex, dir, 0, 0, AHNOLD,
         sys, music, ch, as, ac)
{}

void Ahnold::use_ability(Map *m)
{
  if (get_tex_num() != ABILITY)
  {
    set_tex_num(ABILITY);
    set_cur_frame(1);
  }
}

void Ahnold::enable_ability(Map *m)
{
  float old_speedH, old_speedV;
  int hit[2];
  
  play_effect();
  
  old_speedH = getHSpeed();
  if (getDirection() == RIGHT)
  {
    setHSpeed(PUNCH_RANGE);
  }
  else if (getDirection() == LEFT)
  {
    setHSpeed(-PUNCH_RANGE);
  }
  
  int block = -1;
  old_speedV = getVSpeed();
  setVSpeed(0);
  if (will_collide_tile(m, BREAKABLE1, hit))
  {
    m->breaker(hit[0], hit[1]);
  }
  else if (will_collide_moveables_x(m->get_moveables(), -1, &block))
  {
    Moveable *move;
    move = m->get_moveables().at(block);
    if (move->get_gravity())
    {
      move->setHSpeed(getHSpeed() / PUNCH_RANGE * BLOCK_SLIDE_SPEED, m);
    }
    else
    {
      move->setHSpeed(getHSpeed() / PUNCH_RANGE * HOVER_SLIDE_SPEED, m);
    }
  }
  setVSpeed((int)old_speedV);
  setHSpeed((int)old_speedH);
}
