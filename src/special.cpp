#include "special.h"
#include "defines.h"

Special::Special(void)
:Character()
{}

Special::Special(float x, float y, int num, int frames, Texture *tex,
               direc dir, bool jump, int vs, int hs, FMOD_SYSTEM *sys,
            FMOD_SOUND *so, FMOD_CHANNEL *ch)
:Character(x, y, num, frames, tex, dir, vs, hs, sys, so, ch), loc[0](1), loc[1](1)
{
  loc[0] = x;
  loc[1] = y;
  ((Character *)this)->pause_sound();
}

virtual void use_ability(int dir)
{}

void start_following(Player *p)
{
  following = true;
}

void stop_following(Player *p)
{
  following = false;
}

void go_home()
{
  ((Drawable *)this)->move(loc[0]-get_x(), loc[1]-get_y());
}

// moves the player and also changes the direction
// in which he moves
void Special::move(int x, int y)
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
