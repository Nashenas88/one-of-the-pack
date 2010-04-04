#include "ahnold.h"
#include "defines.h"

Ahnold::Ahnold(void)
:Special()
{}

Ahnold::Ahnold(float x, float y, int num, int frames, int abil_frames,
               Texture *tex, direc dir, FMOD_SYSTEM *sys, FMOD_SOUND *music,
               FMOD_CHANNEL *ch)
:Special(x, y, num, frames, abil_frames, tex, dir, 0, 0, sys, music, ch)
{
  loc[0] = x;
  loc[1] = y;
  
  FMOD_RESULT result;
  
  result = FMOD_System_CreateSound(sys, RESOURCES AHNOLD_SFX, FMOD_SOFTWARE, 0, &sfx);
  ERRCHECK(result);
}

void Ahnold::use_ability(Map *m)
{
  float old_speed;
  int hit[2];
  
  old_speed = getHSpeed();
  if (getDirection() == RIGHT)
  {
    setHSpeed(-PUNCH_RANGE);
  }
  else if (getDirection() == LEFT)
  {
    setHSpeed(PUNCH_RANGE);
  }
  
  if (will_collide_tile(m, BREAKABLE, hit))
  {
    m->remove(hit[0], hit[1]);
  }
  // else if (tile_in_dir == MOVEABLE)
  // {
    // m->move_block(get_x() + xd, get_y());
  // }
  
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

// moves the player and also changes the direction
// in which he moves
void Ahnold::move(int x, int y)
{
  ((Drawable *)this)->move(x, y);
  if (x > 0)
  {
    setDirection(RIGHT);
    set_tex_num(SPECIAL_RIGHT);
  }
  else if (x < 0)
  {
    setDirection(LEFT);
    set_tex_num(SPECIAL_RIGHT);
  }
}
