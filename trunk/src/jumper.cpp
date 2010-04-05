#include "jumper.h"
#include "defines.h"

Jumper::Jumper(void)
:Special()
{}

Jumper::Jumper(float x, float y, int num, int frames, int abil_frames,
               Texture *tex, direc dir, FMOD_SYSTEM *sys, FMOD_SOUND *music,
               FMOD_CHANNEL *ch, FMOD_SOUND *as, FMOD_CHANNEL *ac)
:Special(x, y, num, frames, abil_frames, tex, dir, 0, 0, JUMPER, sys, music, ch, as, ac)
{
  loc[0] = x;
  loc[1] = y;
}

void Jumper::use_ability(Map *m)
{
  play_effect();
  setVSpeed(-JUMP_HEIGHT);
}

void Jumper::start_following(Player *p)
{
  following = true;
}

void Jumper::stop_following(Player *p)
{
  following = false;
}

void Jumper::go_home()
{
  ((Drawable *)this)->move(loc[0]-get_x(), loc[1]-get_y());
}

// moves the player and also changes the direction
// in which he moves
void Jumper::move(int x, int y)
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
