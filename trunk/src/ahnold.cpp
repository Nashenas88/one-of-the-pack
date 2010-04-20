#include "ahnold.h"
#include "defines.h"

Ahnold::Ahnold(void)
:Special()
{}

Ahnold::Ahnold(float x, float y, int num, int frames, int abil_frames,
               Texture *tex, direc dir, FMOD_SYSTEM *sys, FMOD_SOUND *music,
               FMOD_CHANNEL *ch, FMOD_SOUND *as, FMOD_CHANNEL *ac)
:Special(x, y, num, frames, abil_frames, tex, dir, 0, 0, AHNOLD, sys, music, ch, as, ac)
{
  loc[0] = x;
  loc[1] = y;
}

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
  float old_speed;
  int hit[2];
  
  play_effect();
  
  old_speed = getHSpeed();
  if (getDirection() == RIGHT)
  {
    setHSpeed(PUNCH_RANGE);
  }
  else if (getDirection() == LEFT)
  {
    setHSpeed(-PUNCH_RANGE);
  }
  
  int block = -1;
  if (will_collide_tile(m, BREAKABLE, hit))
  {
    m->remove(hit[0], hit[1]);
  }
  else if (will_collide_moveables_x(m->get_moveables(), -1, &block))
  {
    Moveable *move;
    move = m->get_moveables().at(block);
    if (move->get_gravity())
    {
      move->setHSpeed(getHSpeed() / PUNCH_RANGE * BLOCK_SLIDE_SPEED);
    }
    else
    {
      move->setHSpeed(getHSpeed() / PUNCH_RANGE * HOVER_SLIDE_SPEED);
    }
  }
  
  setHSpeed(old_speed);
}

void Ahnold::start_following(Player *p)
{
  following = true;
}

void Ahnold::stop_following(Player *p)
{
  following = false;
}

void Ahnold::go_home()
{
  ((Drawable *)this)->move(loc[0]-get_x(), loc[1]-get_y());
}
