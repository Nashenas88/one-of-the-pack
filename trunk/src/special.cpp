
#include "special.h"
#include "defines.h"

Special::Special(void)
:Character()
{}

Special::Special(float x, float y, int num, int frames, Texture *tex,
                 direc dir, int vs, int hs, FMOD_SYSTEM *sys, FMOD_SOUND *so, 
                 FMOD_CHANNEL *ch)
:Character(x, y, num, frames, tex, dir, vs, hs, sys, so, ch)
{
  loc[0] = x;
  loc[1] = y;
  
  set_mute(true);
}

void Special::start_following(Player *p)
{
  following = true;
}

void Special::stop_following(Player *p)
{
  printf("Stop Following\n");
  following = false;
}

void Special::set_mute(bool m)
{
  mute = m;
  FMOD_Channel_SetVolume(get_channel(), mute?0.0f:1.0f);
}

void Special::go_home()
{
  ((Drawable *)this)->move(loc[0]-get_x(), loc[1]-get_y());
}

// moves the player and also changes the direction
// in which he moves
void Special::move(float x, float y)
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
    set_tex_num(SPECIAL_LEFT);
  }
}
