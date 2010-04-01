
#include "special.h"
#include "defines.h"

Special::Special(void)
:Character()
{}

Special::Special(float x, float y, int num, int frames, int abil_frames,
                 Texture *tex, direc dir, int vs, int hs, FMOD_SYSTEM *sys,
                 FMOD_SOUND *so, FMOD_CHANNEL *ch)
:Character(x, y, num, frames, tex, dir, vs, hs, sys, so, ch),
following(false), num_abil_frames(abil_frames)
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

void Special::pause_volume(void)
{
  if (!mute)
  {
    ((Character *)this)->pause_volume();
  }
}

void Special::unpause_volume(void)
{
  if (!mute)
  {
    ((Character *)this)->unpause_volume();
  }
}

void Special::go_home()
{
  ((Drawable *)this)->move(loc[0]-get_x(), loc[1]-get_y());
}
